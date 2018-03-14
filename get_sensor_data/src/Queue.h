#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>

template <typename T>
class Queue
{
 public:

  T pop() 
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      if (cond_.wait_for(mlock, std::chrono::milliseconds(100)) ==  std::cv_status::timeout) {
        throw std::runtime_error("Queue pop has timed out");
      }
    }
    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      if (cond_.wait_for(mlock, std::chrono::milliseconds(100)) ==  std::cv_status::timeout) {
        throw std::runtime_error("Queue pop has timed out");
      }
    }
    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
  Queue()=default;
  Queue(const Queue&) = delete;            // disable copying
  Queue& operator=(const Queue&) = delete; // disable assignment
  
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};
