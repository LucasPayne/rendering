#include "multithreading.hpp"
// note: This is not supposed to be any sort of general multithreading module.
// Multithreading is primarily for tiled rendering.

static vector<std::thread> threads;
static bool threads_initialized = false;

// Should not require synchronization, since it is only written to by the main thread, and
// running a thread a few extra times before detecting this doesn't matter.
static bool threads_should_terminate = false;

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
        m_finished = false;
    }
    int next_index_i;
    int next_index_j;
    std::function<void(int,int)> f;
    int count_i;
    int count_j;
    int active_workers;

    bool finished() const {
        return m_finished && active_workers == 0;
    }
    bool work_available() const {
        return next_index_i < count_i;
    }

    // !!! A the work mutex must be held when calling this !!!
    void step() {
        next_index_j ++;
        if (next_index_j == count_j) {
            next_index_j = 0;
            next_index_i ++;
            if (next_index_i == count_i) m_finished = true;
        }
    }

    bool m_finished;
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

static void worker_thread()
{
    std::cout << "Spawned worker thread\n";

    // Spawned threads start here.
    // They simply wait for work on the task list, and if there is any work available, they do it.
    // There needs to be carefully synchronized access to the task list for this to work.

    // Acquire a lock on the work mutex. This starts off with the lock.
    std::unique_lock<std::mutex> lock(work_mutex);

    while (!threads_should_terminate) {
        if (work == NULL) {
            // Nothing to do. Wait until the main thread signifies there is work to do.
            // When the thread is spawned, it should go straight into waiting.

            // https://www.cplusplus.com/reference/condition_variable/condition_variable/wait/
            //     The lock is unlocked, and the thread blocked until a notify on the condition variable,
            //     where the lock is then held again.
            work_condition_variable.wait(lock);
        } else {
            // printf("PARALLEL GOING!\n");getchar();
            // This thread currently holds the work mutex.
            int index_i = work->next_index_i;
            int index_j = work->next_index_j;
            // The next task is the next cell in the grid
            // (if the work is done, this goes out of bounds so that Work::finished() returns false.)
            work->step();
            Work *my_work = work; // Save a pointer to the work. !! Remember to only use this now.
            if (!work->work_available()) {
                // This worker is finishing the last task, so set the work pointer to NULL,
                // so that the other works (and the main thread helping out)
                // don't try to pick up a non-existent task.
                work = NULL;
            }
            my_work->active_workers ++;
            
            // Release the mutex, then do the work.
            lock.unlock();
            my_work->f(index_i, index_j);
            // Afterward, halt and request access to the mutex again.
            lock.lock();
            
            my_work->active_workers --;
            if (my_work->finished()) {
                // After finishing the last task, notify all waiting threads.
                // Worker threads will just wait again since work is NULL.
                // The main thread will then get the lock and detect the work is finished.
                work_condition_variable.notify_all();
                //^... looping again makes this thread go back to waiting.
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
    // Start the other threads.
    // work_condition_variable.notify_all();

    while (work != NULL && !work->finished()) {
        // printf("GOING!\n");getchar();
        // Help out.
        // This thread holds the work mutex.
        int index_i = work->next_index_i;
        int index_j = work->next_index_j;
        // The next task is the next cell in the grid.
        work->step();
        Work *my_work = work;
        if (!work->work_available()) {
            work = NULL;
        }
        my_work->active_workers ++;
        
        // Release the mutex, then do the work.
        lock.unlock();
        // This is the point where any worker thread can jump in and grab the lock.
        my_work->f(index_i, index_j);
        // Afterward, halt and request access to the mutex again.
        lock.lock();
        
        my_work->active_workers --;
    }
    // finished.
    work = NULL;
}

void init_multithreading(bool overriding, unsigned int override_num_threads)
{
    check_not_init();
    std::cout << "Initialized multithreading\n";

    threads_initialized = true;
    threads_should_terminate = false;

    // Create all the other threads, and put their execution context into the worker_thread() function.
    unsigned int num_threads = (unsigned int) num_system_cores();
    if (overriding && 0 < override_num_threads && override_num_threads <= num_threads) {
        // Do not exceed number of system cores.
        num_threads = override_num_threads;
    }
    for (int i = 0; i < num_threads-1; i++) {
        threads.push_back(std::thread(worker_thread));
    }
    // Just let the threads go, do not wait on a barrier or anything.
}
void close_multithreading()
{
    check_init();
    threads_should_terminate = true;
    work_condition_variable.notify_all(); // wake up threads so they can do the !threads_should_terminate check.
    for (std::thread &thread : threads) {
        // This causes the thread context to reach the end of its function (worker_thread()).
        thread.join();
    }
    std::cout << "Closed multithreading\n";
}
