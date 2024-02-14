#pragma once

#include <iostream>
#include <thread>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(50, 100);

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
    T* decide(T* prefer) {
      auto sleep = distrib(gen);
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep));

      // std::cout << id <<  " + " << *prefer << std::endl;
      if (CAS(&original, 0, prefer)) {
        return prefer;
      }
      else {
        return original;
      }
    }
};