
#include "../Monitor.h" // Monitor类的头文件
#include "LanceNet/base/Mutex.h"
#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <cassert>

// 缓冲区类
class Buffer {
public:
  // 构造函数，接受一个最大容量作为参数
  Buffer(int capacity) : capacity_(capacity) {}

  // 向缓冲区中添加数据
  void put(int data) {
    std::cout << "Producer " << std::this_thread::get_id() << " is trying to put " << data << std::endl;
    // 获取锁
    auto lock = monitor_.LockGurad();
    // 等待缓冲区不满
    lock.waitUntil([this] { return !full(); });
    // 向队列中添加数据
    queue_.push(data);
    std::cout << "Producer " << std::this_thread::get_id() << " has put " << data << std::endl;
    // 唤醒等待缓冲区不空的线程
    lock.notify_one();
  }

  // 从缓冲区中取出数据
  int get() {
    std::cout << "Consumer " << std::this_thread::get_id() << " is trying to get data" << std::endl;
    // 获取锁
    auto lock = monitor_.LockGurad();
    // 等待缓冲区不空
    lock.waitUntil([this] { return !empty(); });
    // 从队列中取出数据
    int data = queue_.front();
    queue_.pop();
    std::cout << "Consumer " << std::this_thread::get_id() << " has got " << data << std::endl;
    // 唤醒等待缓冲区不满的线程
    lock.notify_one();
    // 返回数据
    return data;
  }

  bool empty() {
    // 获取锁
    // auto lock = monitor_->lock();
    return queue_.empty();
  }

  bool full() {
    // auto lock = monitor_->lock();
    return queue_.size() == capacity_;
  }

private:
  int capacity_; // 缓冲区的最大容量
  std::queue<int> queue_; // 缓冲区使用的队列
  LanceNet::Monitor monitor_; // 封装缓冲区的Monitor对象
};

// 生产者线程的函数
void producer(Buffer& buffer, int n) {
  for (int i = 0; i < n; i++) {
    // 随机生成一个数据
    int data = rand() % 100;
    // 向缓冲区中添加数据
    buffer.put(data);
    // 模拟生产时间
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
  }
}

// 消费者线程的函数
void consumer(Buffer& buffer, int n) {
  for (int i = 0; i < n; i++) {
    // 从缓冲区中取出数据
    int data = buffer.get();
    // 模拟消费时间
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
  }
}

int main() {
  Buffer buffer(10);
  // 创建4个生产者线程和4个消费者线程，每个线程生产或消费5个数据
  std::thread p1(producer, std::ref(buffer), 5);
  std::thread p2(producer, std::ref(buffer), 5);
  std::thread p3(producer, std::ref(buffer), 5);
  std::thread p4(producer, std::ref(buffer), 5);
  std::thread c1(consumer, std::ref(buffer), 5);
  std::thread c2(consumer, std::ref(buffer), 5);
  std::thread c3(consumer, std::ref(buffer), 5);
  std::thread c4(consumer, std::ref(buffer), 5);
  // 等待所有线程结束
  p1.join();
  p2.join();
  p3.join();
  p4.join();
  c1.join();
  c2.join();
  c3.join();
  c4.join();

  assert(buffer.empty());
}

