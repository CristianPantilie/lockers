My implementation of synchronization objects for Linux POSIX threads. It includes:
- mutex
- sempahore
- rwlock 

They make use of the atomic operations included in the stdatomic library. They are far from an optimal implementation, as they involve
spinlocking, but I don't think I could have done much better with without delving into Assembly code. 

Even so, this project has been instrumental for deepening my understanding of the problem of thread synchronizations and of the basic 
solutions for it. 




TODO:
      - test the implementation with the POSIX test suite
      
      - find a better algorithm for mutexes that doesn't involve spinlocking



Resources used:

      - Operating Systems Essentials, Silberschatz, Galvin, Gagne
      - Advanced Linux Programming, Mark Mitchell
      - Concurrent Programming, Michel Raynal
      - GNU library functions manual and glibc 
