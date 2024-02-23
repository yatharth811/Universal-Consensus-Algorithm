#pragma once
#include "Node.hpp"

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

    ~LFUniversal() {
      delete tail;
    }

    void apply(int id, std::function<void()> &invocation) {
      Node* prefer = new Node(invocation);

      while (prefer->seq == 0) {
        Node* before = Node::max(head);
        Node* after = before->decideNext->decide(id, id, prefer);
        before->next = after;
        after->seq = before->seq + 1;
        head[id] = after;
      }

    }
};