#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <vector>
class ThreadPool {
public:
  void Start() {
    const int num_threads = std::thread::hardware_concurrency();
    for (int ii = 0; ii < num_threads; ++ii)
      threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
  }

private:
  void ThreadLoop() {}
  bool should_terminate = false;
  std::mutex queue_mutex;
  std::condition_variable mutex_condition;
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> jobs;
};
