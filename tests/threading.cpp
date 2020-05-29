#include <iostream>
#include <thread>

void foo()
{
    for (int i = 0; i < 1000; i++) {
        std::cout << "foo\n";
    }
}
void nice(int n)
{
    for (int i = 0; i < 1000; i++) {
        std::cout << n << "\n";
    }
}

int main()
{
    std::thread thread_A(foo);
    std::thread thread_B(nice, 3);

    //join :: converge threads of execution?
    thread_A.join();
    thread_B.join();
    return 0;
}
