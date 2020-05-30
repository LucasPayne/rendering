#ifndef MULTITHREADING_H
#define MULTITHREADING_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <functional>
#include "core.hpp"

int num_system_cores();
void init_multithreading();

#endif // MULTITHREADING_H
