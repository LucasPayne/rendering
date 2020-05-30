#include "multithreading.hpp"
// note: This is not supposed to be any sort of general multithreading module.
// Multithreading is primarily for tiled rendering.

static vector<std::thread> threads;
static bool threads_initialized = false;

// "Work" is over a grid of indices and a function that takes a function of index pairs.
// This is because this is really just for tiled rendering.
struct Work {
    Work(std::function<void(int,int)> _f, int _count_i, int _count_j) {
        count_i = _count_i;
        count_j = _count_j;
        next_index_i = 0;
        next_index_j = 0;
        f = _f;
        active_workers = 0;
    }
    int next_index_i;
    int next_index_j;
    std::function<void(int,int)> f;
    int count_i;
    int count_j;
    int active_workers;

    bool finished() const {
        // Finiahsed if all the tasks have been taken and no thread is working on any of the last ones.
        return next_index_i >= count_i && active_workers == 0;
    }

    // !!! A the work mutex must be held when calling this !!!
    void step() {
        next_index_j ++;
        if (next_index_j == count_j) {
            next_index_j = 0;
            next_index_i ++;
        }
    }
};

static Work *work = NULL;
// Access to the work pointer must be synchronized. This is done with mutexes and condition variables.
static std::mutex work_mutex;
static std::condition_variable work_condition_variable;

static inline void check_init()
{
    if (!threads_initialized) {
        std::cerr << "multithreading error: Threads are not initialized.\n";
        exit(EXIT_FAILURE);
    }
}
static inline void check_not_init()
{
    if (threads_initialized) {
        std::cerr << "multithreading error: Threads are already initialized.\n";
        exit(EXIT_FAILURE);
    }
}

int num_system_cores()
{
    // hardware_concurrency() gives a hint only.
    int n = std::thread::hardware_concurrency();
    if (n == 0) return 1;
    return n;
}

/*--------------------------------------------------------------------------------
Condition variables
-------------------
from cppreference std::condition_variable:

[holding a condition_variable]
The condition_variable class is a synchronization primitive that can be used to block
a thread, or multiple threads at the same time, until another thread both modifies a
shared variable (the condition), and notifies the condition_variable.
The thread that intends to modify the variable has to
    - acquire a std::mutex (typically via std::lock_guard)
    - perform the modification while the lock is held
    - execute notify_one or notify_all on the std::condition_variable (the lock does
      not need to be held for notification)

[waiting on a condition variable]
Any thread that intends to wait on std::condition_variable has to
    - acquire a std::unique_lock<std::mutex>, on the same mutex as used to protect the shared variable
    either
        - check the condition, in case it was already updated and notified
        - execute wait, wait_for, or wait_until. The wait operations atomically release
          the mutex and suspend the execution of the thread.
        - When the condition variable is notified, a timeout expires, or a spurious wakeup occurs,
          the thread is awakened, and the mutex is atomically reacquired. The thread should then
          check the condition and resume waiting if the wake up was spurious.
    or
        - use the predicated overload of wait, wait_for, and wait_until, which takes care of the
          three steps above
--------------------------------------------------------------------------------*/

void worker_thread()
{
    std::cout << "Spawned worker thread\n";

    // Spawned threads start here.
    // They simply wait for work on the task list, and if there is any work available, they do it.
    // There needs to be carefully synchronized access to the task list for this to work.

    // Acquire a lock on the work mutex. This starts off with the lock.
    std::unique_lock<std::mutex> lock(work_mutex);

    while (true) { //---should probably have some way to shut down all the threads.
        if (work == NULL) {
            // Nothing to do. Wait until the main thread signifies there is work to do.
            work_condition_variable.wait(lock);
        } else {
            // This thread holds the work mutex.
            int index_i = work->next_index_i;
            int index_j = work->next_index_j;
            // The next task is the next cell in the grid
            // (if the work is done, this goes out of bounds so that Work::finished() returns false.)
            work->step();
            work->active_workers ++;
            
            // Release the mutex, then do the work.
            lock.unlock();
            work->f(index_i, index_j);
            // Afterward, halt and request access to the mutex again.
            lock.lock();
            
            work->active_workers --;
            if (work->finished()) {
                work_condition_variable.notify_all();
            }
        }
    }

    std::cout << "Closed worker thread\n";
}

void parallel_for_2D(std::function<void(int,int)> f, const int &count_i, const int &count_j)
{
    check_init();
    // Create tasks for each of (i,j) for i:[0,count_i), j:[0,count_j).
    // The worker threads will then grab them from the task stack.
    // Each worker just return to waiting for a task, and this (the main thread)
    // can detect when the task stack is empty.
    //----Be careful with synchronization!
    if (work != NULL) {
        std::cerr << "ERROR: parallel_for_2D: Cannot start more work, multithreaded work is already being done.\n";
        exit(EXIT_FAILURE);
    }
    if (threads.empty()) {
        // No spawned threads (only the main thread). Just do exactly what
        // this for loop should do, but single-threaded. (it may be a good way to test if multithreading is correct by spawning no threads
        // and just doing this each time, and comparing).
        for (int i = 0; i < count_i; i++) {
            for (int j = 0; j < count_j; j++) {
                f(i, j);
            }
        }
        return;
    }
    // Create the new work.
    Work new_work = Work(std::move(f), count_i, count_j); // why std::move?
    // Acquire a lock on the global, shared work mutex.
    // (threads also acquire locks on this, and attempt to get access to it for new work.)
/*--------------------------------------------------------------------------------
cppreference std::lock_guard
--------------------------------------------------------------------------------
The class lock_guard is a mutex wrapper that provides a convenient RAII-style mechanism for owning a mutex for
the duration of a scoped block. When a lock_guard object is created, it attempts to take ownership of
the mutex it is given. When control leaves the scope in which the lock_guard object was created, the
lock_guard is destructed and the mutex is released.  The lock_guard class is non-copyable.
--------------------------------------------------------------------------------*/
    {
        std::lock_guard<std::mutex> lock(work_mutex);
        // This thread should now own the mutex. It will be released when this block scope is exited.
        // Set up the new work.
        work = &new_work;
    }

    std::unique_lock<std::mutex> lock(work_mutex);
    work_condition_variable.notify_all();

    while (!work->finished()) {
        //---help out.
        printf("Going ...\n");
    }
    // finished.
}

void init_multithreading()
{
    check_not_init();

    // Create all the other threads, and put their execution context into the worker_thread() function.
    int num_threads = num_system_cores();
    for (int i = 0; i < num_threads-1; i++) {
        threads.push_back(std::thread(worker_thread));
    }
    // Just let the threads go, do not wait on a barrier or anything.
    threads_initialized = true;
}
