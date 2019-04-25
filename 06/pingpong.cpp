#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

const size_t N = 1000000;
size_t counter = 0;
bool condition = true;
std::mutex mutex;
std::condition_variable ready;

void pingpong(bool is_sender) {
    std::unique_lock<std::mutex> lock(mutex);
    while (counter < N - is_sender)    {
        ready.wait(lock, [&]{return is_sender ? condition : !condition;});
        ++counter;
        condition = !condition;
        if (is_sender)
            std::cout << "ping\n";
        else
            std::cout << "pong\n";
        ready.notify_one();
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::thread t1(pingpong, 1);
    std::thread t2(pingpong, 0);
    t1.join();
    t2.join();
    return 0;
}
