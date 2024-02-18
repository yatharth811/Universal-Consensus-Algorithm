#pragma once

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
    int winner{-1};
    int helpId{-1};
    T* decide(int id, int helpedId, T* prefer) {
      if (CAS(&original, 0, prefer)) {
        winner = id;
        helpId = helpedId;
        return prefer;
      }
      else {
        return original;
      }
    }
};