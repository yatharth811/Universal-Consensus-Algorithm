#pragma once
#include <functional>
#include "Consensus.hpp"

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
