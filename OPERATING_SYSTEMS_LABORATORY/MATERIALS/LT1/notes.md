## Points to remember

### Pthread
- 1. If some vairable is not declared globally, threads will not be able to access it. So check if declarations are global.
- 2. `pthread_join` is not a blocking call if the thread does not exist. It will return error and won't wait 
- 3. Anything after `pthread_exit` is not executed.
- 4. `pthread_exit` don't have any return value. 
```c
ret = pthread_exit();
```
will give error.
- 5. In case, there is no `pthread_join` after a `pthread_create`, keep in mind the main thread may terminate even before the thread does anything.
- 6. Open file descriptors are shared if declared globally
- 7. In case you join on a thread with `pthread_join(pt, &res_t)` but the thread doesn't return anything, the call will succed but accesing `res_t` will cause segmentation fault
- 8. On call of `pthread_cond_siganl`, the thread must lock the mutex after getting signal on condition variable even if it didn't lock the mutex before the call

### fork
- 1. For parent and children check if they are also executing after the if statement.
- 2. 