#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <chrono>

namespace utils {
    class thread_pool {
    public:
        explicit thread_pool(size_t threads = std::thread::hardware_concurrency()) : stop(false) {
            workers.reserve(threads);

            for(size_t i = 0; i < threads; i++)
                workers.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, [this] {
                                return this->stop || !this->tasks.empty();
                            });

                            if(this->stop && this->tasks.empty())
                                return;

                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                });
        }

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                /*if(stop)
                throw std::runtime_error("enqueue on stopped thread_pool");*/

                tasks.emplace([task] {
                    // FIXME: sleep for test only
                    //std::this_thread::sleep_for(std::chrono::seconds(2));
                    (*task)();
                });
            }

            condition.notify_one();
            return res;
        }

        ~thread_pool() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true;
            }

            condition.notify_all();

            for(auto &worker : workers)
                worker.join();
        }

        thread_pool (const thread_pool &) = delete;
        thread_pool& operator= (const thread_pool &) = delete;

        thread_pool (thread_pool &&) = delete;
        thread_pool& operator= (const thread_pool &&) = delete;

    private:
        std::vector<std::thread>            workers;
        std::queue<std::function<void()>>   tasks;

        std::mutex                          queue_mutex;
        std::condition_variable             condition;
        bool                                stop;
    };
} // namespace utils

/*void test() {
    utils::thread_pool pool;

    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(pool.enqueue([i] {
            std::cout << "hello " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "world " << i << std::endl;
            return i;
        }));
    }

    auto ready = false;
    auto waiting = true;
    while (waiting) {
        for (auto && result: results) {
            ready = true;

            if (result.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                ready = false;

            if (ready) {
                waiting = false;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    exit(0);
}*/
