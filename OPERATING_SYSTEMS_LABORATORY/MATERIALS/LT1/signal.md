### SIGINT: CTRL+C
### SIGQUIT: CTRL+\
### SIGTSTP: CTRL+Z

## signal()
- 'Header': #include <signal.h>
- 'Prototype': 
```c
void (*signal(int signo, void (*func)(int)))(int);
```