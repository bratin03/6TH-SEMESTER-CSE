## fork()
- Header: `#include <unistd.h>` and `#include <sys/types.h>`
- Prototype: 
```c
pid_t fork(void);
```
## wait(), waitpid()
- Header: `#include <sys/types.h>` and `#include <sys/wait.h>`
- Prototype: 
```c
pid_t wait(int *status); 
pid_t waitpid(pid_t pid, int *status, int options);
```

```c
waitpid(-1, &status, 0) = wait(&status)
```
## exec()
- Header: `#include <unistd.h>`
- Prototype: 
```c
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
```
Example:
```c
execl("/bin/ls", "ls", "-l", (char *)0);
execlp("ls", "ls", "-l", (char *)0);
execle("/bin/ls", "ls", "-l", (char *)0, envp);
argv[0] = "ls";
argv[1] = "-l";
argv[2] = (char *)0;
execv("/bin/ls", argv);
execvp("ls", argv);
execvpe("ls", argv, envp);
```
