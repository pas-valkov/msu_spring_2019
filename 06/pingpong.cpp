#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

const size_t N = 1000000;
size_t counter = 0;
bool condition = true;
std::mutex mutex;
std::condition_variable ready;

void pingpong(const char* str) {
    std::unique_lock<std::mutex> lock(mutex);
    while (counter < N - (str == "ping"))    {
        ready.wait(lock, [str]{
            return (str == "ping") ? condition : !condition;});
        ++counter;
        condition = !condition;
        std::cout << str << "\n";
        ready.notify_one();
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::thread t1(pingpong, "ping");
    std::thread t2(pingpong, "pong");
    t1.join();
    t2.join();
    return 0;
}
