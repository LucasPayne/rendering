#include "multithreading.hpp"

int num_system_cores()
{
    int n = std::thread::hardware_concurrency();
    if (n == 0) return 1;
    return n;
}
