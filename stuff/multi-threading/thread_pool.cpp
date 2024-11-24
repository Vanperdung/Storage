#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <atomic>
#include <functional>

class ThreadPool {
public:
    ThreadPool(int numThreads) : m_isStop(false)
    {
        for (int i = 0; i < numThreads; i++)
        {
            m_pool.emplace_back([this]() {
                while (true) 
                {
                    std::function<void()> readyTask;

                    // this code block will wait until a task is availabe in queue
                    // after that, it take the task from queue and run it in thread pool 
                    { 
                        std::unique_lock<std::mutex> uLock(this->m_mutex);
                        m_condition.wait(uLock, [this]() {
                            return this->m_isStop || !this->m_pool.empty();
                        });

                        if (this->m_isStop && this->m_pool.empty())
                            return;
                        
                        readyTask = std::move(this->m_waitTasks.front());
                        this->m_waitTasks.pop();
                    }

                    // do the task
                    readyTask(); 
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> uLock(m_mutex);
            m_isStop = true;
        }
        m_condition.notify_all();

        for (std::thread& t : m_pool)
        {
            t.join();
        }
    }

    template <typename Task, typename... Args>
    void enqueue(Task&& task, Args&&... args)
    {
        {
            std::unique_lock<std::mutex> uLock(m_mutex);
            
            // Make a lambda function to wrap the task
            m_waitTasks.emplace([task, args...]() {
                // call the task with its arguments
                task(args...);
            });

            // Notify one task that is available in queue
            m_condition.notify_one();
        }
    }

private:
    std::vector<std::thread> m_pool;
    std::queue<std::function<void()>> m_waitTasks;
    
    std::atomic<bool> m_isStop;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

int main(int argc, char *argv[])
{
    ThreadPool threadPool(10);

    // for (int i = 0; i < 100; i++)
    // {
    //     // push a thread into queue
    //     threadPool.enqueue([i]() {
    //         std::cout << "Task " << i << \
    //                 " is running on thread id = " << \
    //                 std::this_thread::get_id() << \
    //                 std::endl;

    //         std::this_thread::sleep_for(std::chrono::seconds(10));
    //     });
    // }

    return 0;
}