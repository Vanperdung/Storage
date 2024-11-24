#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>

std::condition_variable cv;
std::mutex mutex;
int i = 0;

void waits()
{
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    
    lock.lock();
    std::cout << "waiting..." << std::endl;
    
    cv.wait(lock, []{ return i == 1; });
    std::cout << "...finished waiting (i==1)" << std::endl;
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Notifying all..." << std::endl;
    cv.notify_all();
    
    std::cout << "Notifying all again..." << std::endl;
    std::lock_guard<std::mutex> lock(mutex); // lock this mutex
    i = 1;
    cv.notify_all();
} // unlock mutex

int main(int argc, char *argv[])
{
    std::thread t1(waits), t2(waits), t3(waits), t4(signals);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}