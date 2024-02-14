#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 1000);

template<typename T> 
class Consensus {
  public:
    T* original{0};

    static int CAS(T **ptr, T* oldVal, T* newVal) {
      unsigned char ret;
      __asm__ __volatile__ (
              "  lock\n"
              "  cmpxchgq %[newval], %[mem]\n"
              "  sete %0\n"
              : "=q" (ret), [mem] "+m" (*ptr), "+a" (oldVal)
              : [newval]"r" (newVal)
              : "memory");    // barrier for compiler reordering around this
      return ret;   // ZF result, 1 on success else 0
    }

  public:
    int winner;
    T* decide(int id, T* prefer) {
      auto sleep = distrib(gen);
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep));

      // std::cout << id <<  " + " << *prefer << std::endl;
      if (CAS(&original, 0, prefer)) {
        winner = id;
        return prefer;
      }
      else {
        return original;
      }
    }
};

class Node {
  public:
    std::function<void()> storedFunction;
    Consensus<Node> *decideNext;
    Node *next;
    int seq{0};

    Node(void) {
      decideNext = new Consensus<Node>();
    }

    Node(std::function<void()> &invocation) {
      decideNext = new Consensus<Node>();
      storedFunction = invocation;
    }

    ~Node() {
      delete decideNext;
    }

    static Node* max(const std::vector<Node*> &nodes) {
      Node* mx = nullptr;
      for (Node* node: nodes) {
        if (mx == nullptr) {
          mx = node;
        } 
        else if (mx->seq < node->seq) {
          mx = node;
        }
      }
      return mx;
    }
};


class LFUniversal {
  public:
  int nthreads;
  std::vector<Node*> head;
  Node* tail;

    LFUniversal(int _nthreads) {
      nthreads = _nthreads;
      tail = new Node();
      head.assign(nthreads, tail);
      tail->seq = 1;
    }

    void apply(int id, std::function<void()> &invocation) {
      Node* prefer = new Node(invocation);

      while (prefer->seq == 0) {
        Node* before = Node::max(head);
        Node* after = before->decideNext->decide(id, prefer);
        before->next = after;
        after->seq = before->seq + 1;
        head[id] = after;
      }

    }
};

class myStack {
  public:
    std::vector<int> v{};

    void push(int value) {
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


int main() {
  int nthreads = 1000;
  LFUniversal list(nthreads);
  std::vector<std::thread> threads;
  myStack s;


  Node* first = list.tail;

  auto work = [&] (int id, std::function<void()> invocation) {
    list.apply(id, invocation);
  };

  std::vector<int> props;
  for (int i = 0; i < nthreads; i++) {
    int proposed = distrib(gen);
    std::function<void()> proposedFunction;
    if (i & 1) proposedFunction = std::bind(&myStack::push, &s, proposed);
    else proposedFunction = std::bind(&myStack::pop, &s);
    props.push_back(proposed);
    threads.emplace_back(work, i, proposedFunction);
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


  // for (auto x: props) {
  //  std::cout << x << " ";
  // }
  // std::cout << std::endl;

  // for (auto x: winners) {
  //  std::cout << x << " ";
  // }
  // std::cout << std::endl;

  // s.print();

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
    std::cout << "TEST PASSED" << std::endl;
  }
  else {
    std::cout << "TEST FAILED" << std::endl;
  }

  
}