#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <cassert>
#include <iostream>
using namespace std;


static int CAS(int *ptr, int oldVal, int newVal)
{
    unsigned char ret;
    __asm__ __volatile__ (
            "  lock\n"
            "  cmpxchgl %[newval], %[mem]\n"
            "  sete %0\n"
            : "=q" (ret), [mem] "+m" (*ptr), "+a" (oldVal)
            : [newval]"r" (newVal)
            : "memory");    // barrier for compiler reordering around this
    return ret;   // ZF result, 1 on success else 0
}
     
int main() {
  
    int num = 20;
    int original = 0;
    std::vector<int> final(num);
  
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(50, 100);
    int winner = -1;
    auto work = [&] (int id, int proposed) {
        auto sleep = distrib(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        int chosen = 0;
        int old = 0;
        if (CAS(&original, old, proposed)) {
            winner = id;
            chosen = proposed;
        } else {
            chosen = original;
        }
        assert(chosen != 0);
        final[id] = chosen;
    };
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num; i++) {
        int proposed = distrib(gen);
        threads.emplace_back(work, i, proposed);
    }
  
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    for (int i = 0; i < num; i++) {
        std::cout << "Thread " << i << " chose value = " << final[i] << endl;
    }
  
    cout << "Winner: " << winner << endl;
    return 0;
}

