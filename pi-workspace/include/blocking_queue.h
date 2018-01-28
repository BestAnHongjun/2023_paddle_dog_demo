/*
 * Copyright (C) 2023 Coder.AN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue
{
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable not_full_cond_;
    std::condition_variable not_empty_cond_;
    size_t capacity_;

public:
    explicit BlockingQueue(size_t capacity) : capacity_(capacity) {}

    void push(const T& item)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (queue_.size() >= capacity_)
        {
            not_full_cond_.wait(lock);
        }

        queue_.push(item);
        not_empty_cond_.notify_all();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (queue_.empty())
        {
            not_empty_cond_.wait(lock);
        }

        T item = queue_.front();
        queue_.pop();
        not_full_cond_.notify_all();
        return item;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};

#endif // BLOCKING_QUEUE_H

