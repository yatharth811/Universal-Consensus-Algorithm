#pragma once
#include "Node.hpp"

class WFUniversal {
  public:
  int nthreads;
  std::vector<Node*> head;
  std::vector<Node*> announce;
  Node* tail;

    WFUniversal(int _nthreads) {
      nthreads = _nthreads;
      tail = new Node();
      head.assign(nthreads, tail);
      announce.assign(nthreads, tail);
      tail->seq = 1;
    }

    ~WFUniversal() {
      delete tail;
    }

    void apply(int id, std::function<void()> &invocation) {
      announce[id] = new Node(invocation);
  
      while (announce[id]->seq == 0) {
        Node* before = Node::max(head);
        Node* help = announce[(before->seq + 1) % nthreads];
        Node* prefer = nullptr;
        int helpId;
        if (help->seq == 0) {
          prefer = help;
          helpId = (before->seq + 1) % nthreads;
        }
        else {
          prefer = announce[id];
          helpId = id;
        }

        Node* after = before->decideNext->decide(id, helpId, prefer);
        before->next = after;
        after->seq = before->seq + 1;
        head[id] = after;
      }

    }

};
