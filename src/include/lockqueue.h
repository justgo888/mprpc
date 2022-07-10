#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class LockQueue
{
public:
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(queMtx_);
        lockQue_.push(data);
        cond_.notify_one();
    }
    //一个线程写日志
    T pop()
    {
        std::unique_lock<std::mutex> lock(queMtx_);
        while (lockQue_.empty())
        {
            cond_.wait(lock);
        }
        T data = lockQue_.front();
        lockQue_.pop();
        return data;
    }

private:
    std::queue<T> lockQue_;
    std::mutex queMtx_;
    std::condition_variable cond_;
};

#endif