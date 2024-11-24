#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

std::mutex mutex;
std::condition_variable cv;
bool share_data = false; 

void consumer_handler()
{    
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    
    lock.lock();
    // cv.wait() will unlock the lock when waiting for notifycation
    cv.wait(lock, [] { return share_data; });
    // after receiving notifycation and share_data == true, it calls lock()
    
    std::cout << "consumer_handler accessed" << std::endl;
}

void producer_handler()
{
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    lock.lock();

    share_data = true; 
    std::cout << "producer_handler accessed" << std::endl;
    cv.notify_one();
}

int main(int argc, char *argv[])
{
    std::thread consumer(consumer_handler);
    std::thread producer(producer_handler);

    consumer.join();
    producer.join();

    return 0;
}