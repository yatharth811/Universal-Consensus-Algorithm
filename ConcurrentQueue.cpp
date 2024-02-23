#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <queue>

#include "LFUniversal.hpp"
#include "WFUniversal.hpp"


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 1000);
int nthreads = 1024;

template<typename T>
class myQueue {

  public:
    std::queue<T> v{};

    void push(T value) {
      v.push(value);
    }

    void pop(void) {
      if (!v.empty())
        v.pop();
    }

};


void lockFreeStack() {

  // int nthreads = 100;
  LFUniversal list(nthreads);
  std::vector<std::thread> threads;
  myQueue<int> s;

  Node* first = list.tail;

  std::vector<int> props;
  for (int i = 0; i < nthreads; i++) {
    int proposed = distrib(gen);
    std::function<void()> proposedFunction;
    if (i & 1) proposedFunction = std::bind(&myQueue<int>::push, &s, proposed);
    else proposedFunction = std::bind(&myQueue<int>::pop, &s);
    props.push_back(proposed);
    threads.emplace_back([&] (int id, std::function<void()> invocation) {
        auto sleep = distrib(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        list.apply(id, invocation);
      }, i, proposedFunction
    );
  }

  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  std::vector<int> winners;
  while(first != nullptr) {
    if (first->next != nullptr)
      winners.push_back(first->decideNext->winner);
    first = first->next;
    if (first != nullptr)
      first->storedFunction();
  }

  std::queue<int> z;
  for (auto x: winners) {
    if (x & 1) {
      z.push(props[x]);
    }
    else {
      if (!z.empty())
        z.pop();
    }
  }

  if (z == s.v) {
    std::cout << "LF TEST PASSED" << std::endl;
  }
  else {
    std::cout << "LF TEST FAILED" << std::endl;
  }

}

void waitFreeStack() {

  // int nthreads = 100;
  WFUniversal list(nthreads);
  std::vector<std::thread> threads;
  myQueue<int> s;
  Node* first = list.tail;

  std::vector<int> props;
  for (int i = 0; i < nthreads; i++) {
    int proposed = distrib(gen);
    std::function<void()> proposedFunction;
    if (i & 1) proposedFunction = std::bind(&myQueue<int>::push, &s, proposed);
    else proposedFunction = std::bind(&myQueue<int>::pop, &s);
    props.push_back(proposed);
    threads.emplace_back([&] (int id, std::function<void()> invocation) {
        auto sleep = distrib(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        list.apply(id, invocation);
      }, i, proposedFunction
    );
  }

  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  std::vector<int> winners;
  while(first != nullptr) {
    if (first->next != nullptr)
      winners.push_back(first->decideNext->helpId);
    first = first->next;
    if (first != nullptr)
      first->storedFunction();
  }


  std::queue<int> z;
  for (auto x: winners) {
    if (x & 1) {
      z.push(props[x]);
    }
    else {
      if (!z.empty())
        z.pop();
    }
  }

  if (z == s.v) {
    std::cout << "WF TEST PASSED" << std::endl;
  }
  else {
    std::cout << "WF TEST FAILED" << std::endl;
  }
  
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Pass only thread count as arguments." << std::endl;
    return 0;
  }
  nthreads = std::stoi(argv[1]);
  lockFreeStack();
  waitFreeStack();
}