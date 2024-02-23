#include <iostream>
#include <vector>
#include <thread>
#include <random>

#include "LFUniversal.hpp"
#include "WFUniversal.hpp"


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 1000);
int nthreads = 1024;

template<typename T>
class myStack {

  public:
    std::vector<T> v{};

    void push(T value) {
      v.push_back(value);
    }

    void pop(void) {
      if (!v.empty())
        v.pop_back();
    }

    void print(void) {
      for (auto x: v) {
        std::cout << x << " | ";
      }
      std::cout << std::endl;
    }

};


void lockFreeStack() {

  // int nthreads = 100;
  LFUniversal list(nthreads);
  std::vector<std::thread> threads;
  myStack<int> s;

  Node* first = list.tail;

  std::vector<int> props;
  for (int i = 0; i < nthreads; i++) {
    int proposed = distrib(gen);
    std::function<void()> proposedFunction;
    if (i & 1) proposedFunction = std::bind(&myStack<int>::push, &s, proposed);
    else proposedFunction = std::bind(&myStack<int>::pop, &s);
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

  std::vector<int> z;
  for (auto x: winners) {
    if (x & 1) {
      z.push_back(props[x]);
    }
    else {
      if (!z.empty())
        z.pop_back();
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
  myStack<int> s;
  Node* first = list.tail;

  std::vector<int> props;
  for (int i = 0; i < nthreads; i++) {
    int proposed = distrib(gen);
    std::function<void()> proposedFunction;
    if (i & 1) proposedFunction = std::bind(&myStack<int>::push, &s, proposed);
    else proposedFunction = std::bind(&myStack<int>::pop, &s);
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


  std::vector<int> z;
  for (auto x: winners) {
    if (x & 1) {
      z.push_back(props[x]);
    }
    else {
      if (!z.empty())
        z.pop_back();
    }
  }

  if (z == s.v) {
    std::cout << "WF TEST PASSED" << std::endl;
  }
  else {
    std::cout << "WF TEST FAILED" << std::endl;
  }
  
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Pass only thread count as arguments." << std::endl;
    return 0;
  }
  nthreads = std::stoi(argv[1]);
  lockFreeStack();
  waitFreeStack();
}