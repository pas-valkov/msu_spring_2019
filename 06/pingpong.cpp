#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

const size_t N = 1000000;
size_t counter = 0;
bool condition = true;
std::mutex mutex;
std::condition_variable ready;

void pingpong(bool is_first) {
    std::unique_lock<std::mutex> lock(mutex);
    while (counter < N - int(is_first))    {
        ready.wait(lock, [is_first]{
            return is_first ? condition : !condition;});
        ++counter;
        condition = !condition;
        std::cout << (is_first ? "ping" : "pong") << "\n";
        ready.notify_one();
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::thread t1(pingpong, true);
    std::thread t2(pingpong, false);
    t1.join();
    t2.join();
    return 0;
}
