#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

const size_t N = 1000000;
size_t counter = 0;
bool condition = false;
std::mutex mutex;
std::condition_variable ready;

void ping() {
    std::unique_lock<std::mutex> lock(mutex);
    while (counter < N-1) {
        ready.wait(lock, []{return !condition;});
        ++counter;
        condition = !condition;
        std::cout << "ping\n";
        ready.notify_one();
    }
}

void pong() {
    std::unique_lock<std::mutex> lock(mutex);
    while (counter < N)    {
        ready.wait(lock, []{return condition;});
        ++counter;
        condition = !condition;
        std::cout << "pong\n";
        ready.notify_one();
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::thread t1(ping);
    std::thread t2(pong);
    t1.join();
    t2.join();
    return 0;
}
