#ifndef MULTITHREADING_H
#define MULTITHREADING_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include "core.hpp"

int num_system_cores();
void init_multithreading(bool overriding = false, unsigned int override_num_threads = 1);
void close_multithreading();

void parallel_for_2D(std::function<void(int,int,int)> f, const int &count_i, const int &count_j, bool use_main_thread = true);

#endif // MULTITHREADING_H
