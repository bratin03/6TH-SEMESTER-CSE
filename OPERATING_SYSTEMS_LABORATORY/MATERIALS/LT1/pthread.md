A signal sent to a process will be handled by one thread only.

## pthread_create
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
```
- Methods to exit:
1. `pthread_exit(void *retval);`: the exit status will be available to another thread by calling `pthread_join()`.
2. `return void *retval;`: the exit status will be available to another thread by calling `pthread_join()`. or just return without any value.
3. It is cancelled(to see)
4. Any of the threads calls `exit()`.

ID is stored in `pthread_t` type.
- 1. Signal mask is inherited from the parent thread.
- 2. Set of pending signals is cleared for the new thread.

- Return Value: 0 on success, otherwise an error number is returned.

## pthread_exit
```c
void pthread_exit(void *retval);
```
- The `pthread_exit()` function terminates the calling thread and returns a value via `retval` that (if the thread is joinable) is available to another thread in the same process that calls `pthread_join(3)`.

- Performing a return from the start function of any thread other than the main thread results in an implicit call to `pthread_exit()`, using the function's return value as the thread's exit status.

## pthread_cancel
```c
int pthread_cancel(pthread_t thread);
```
- state and type:
1. `PTHREAD_CANCEL_ENABLE`: The thread is cancelable.
2. `PTHREAD_CANCEL_DISABLE`: The thread is not cancelable.
3. `PTHREAD_CANCEL_DEFERRED`: The thread is cancelable, but the cancel is deferred until the thread next calls a function that is a cancellation point.
4. `PTHREAD_CANCEL_ASYNCHRONOUS`: The thread is cancelable and the cancel is acted upon immediately.

- Return Value: 0 on success, otherwise an error number is returned.

After a canceled thread has terminated, a join with that thread using `pthread_join(3)` will return the `PTHREAD_CANCELED` value in the `status` argument to indicate this.

## pthread_attr_init
```c
int pthread_attr_init(pthread_attr_t *attr);
```
Calling `pthread_attr_init()` on a already initialized `pthread_attr_t` results in undefined behavior. Once a thread object has been destroyed, it can be reinitialized using `pthread_attr_init()`.
- Return Value: 0 on success, otherwise an error number is returned.
- Using `pthread_attr_t` type without initializing it with `pthread_attr_init()` results in undefined behavior. I got segmentation fault.

## pthread_attr_destroy
```c
int pthread_attr_destroy(pthread_attr_t *attr);
```
- Return Value: 0 on success, otherwise an error number is returned.

## pthread_join
```c
int pthread_join(pthread_t thread, void **retval);
```
Calling it twice with the same thread will result in error. No such process.
- Return Value: 0 on success, otherwise an error number is returned.
```

## pthread_detach
```c
int pthread_detach(pthread_t thread);
```
- Return Value: 0 on success, otherwise an error number is returned.
- Once a thread has been detached, it can't be joined or made joinable again.

## pthread_attr_setdetachstate
```c
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
```