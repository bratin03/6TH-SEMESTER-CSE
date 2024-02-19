### SETPGID

```c
SETPGID(2)                    System Calls Manual                   SETPGID(2)

NAME
     setpgid, setpgrp – set process group

SYNOPSIS
     #include <unistd.h>

     int
     setpgid(pid_t pid, pid_t pgid);

     pid_t
     setpgrp(void);

DESCRIPTION
     setpgid() sets the process group of the specified process pid to the
     specified pgid.  If pid is zero, then the call applies to the current
     process.

     If the invoker is not the super-user, then the affected process must have
     the same effective user-id as the invoker or be a descendant of the
     invoking process.

RETURN VALUES
     setpgid() returns 0 when the operation was successful.  If the request
     failed, -1 is returned and the global variable errno indicates the
     reason.

ERRORS
     setpgid() will fail and the process group will not be altered if:

     [EACCES]           The value of the pid argument matches the process ID
                        of a child process of the calling process, and the
                        child process has successfully executed one of the
                        exec functions.

     [EINVAL]           The value of the pgid argument is less than 0 or is
                        not a value supported by the implementation.

     [EPERM]            The process indicated by the pid argument is a session
                        leader.

     [EPERM]            The effective user ID of the requested process is
                        different from that of the caller and the process is
                        not a descendant of the calling process.

     [EPERM]            The value of the pgid argument is valid, but does not
                        match the process ID of the process indicated by the
                        pid argument and there is no process with a process
                        group ID that matches the value of the pgid argument
                        in the same session as the calling process.

     [ESRCH]            The value of the pid argument does not match the
                        process ID of the calling process or of a child
                        process of the calling process.

LEGACY SYNOPSIS
     #include <unistd.h>

     int
     setpgrp(pid_t pid, pid_t pgid);

     The legacy setpgrp() function is a clone of the setpgid() function,
     retained for calling convention compatibility with historical versions of
     BSD.

COMPATIBILITY
     Use of the legacy version of the setpgrp() call will cause compiler
     diagnostics.  Use setpgid() instead.

     Use of private (and conflicting) prototypes for setpgrp() will cause
     compiler diagnostics.  Delete the private prototypes and include
     <unistd.h>.

SEE ALSO
     getpgrp(2), compat(5)

STANDARDS
     The setpgid() function conforms to IEEE Std 1003.1-1988 (“POSIX.1”).

BSD 4                            June 4, 1993                            BSD 4
```

### GETPGID

```c
GETPGRP(2)                    System Calls Manual                   GETPGRP(2)

NAME
     getpgid, getpgrp – get process group

LIBRARY
     Standard C Library (libc, -lc)

SYNOPSIS
     #include <unistd.h>

     pid_t
     getpgid(pid_t pid);

     pid_t
     getpgrp(void);

DESCRIPTION
     The process group of the current process is returned by getpgrp().  The
     process group of the process identified by pid is returned by getpgid().
     If pid is zero, getpgid() returns the process group of the current
     process.

     Process groups are used for distribution of signals, and by terminals to
     arbitrate requests for their input: processes that have the same process
     group as the terminal are foreground and may read, while others will
     block with a signal if they attempt to read.

     This call is thus used by programs such as csh(1) to create process
     groups in implementing job control.  The tcgetpgrp() and tcsetpgrp()
     calls are used to get/set the process group of the control terminal.

RETURN VALUES
     The getpgrp() call always succeeds.  Upon successful completion, the
     getpgid() call returns the process group of the specified process;
     otherwise, it returns a value of -1 and sets errno to indicate the error.

ERRORS
     getpgid() will succeed unless:

     [ESRCH]            There is no process whose process ID equals pid.

SEE ALSO
     getsid(2), setpgid(2), termios(4)

HISTORY
     The getpgrp() function call appeared in 4.0BSD.  The getpgid() function
     call is derived from its usage in System V Release 4.

STANDARDS
     The getpgrp() function call is expected to conform to IEEE Std
     1003.1-1990 (“POSIX.1”).

COMPATIBILITY
     This version of getpgrp() differs from past Berkeley versions by not
     taking a pid_t pid argument.  This incompatibility is required by IEEE
     Std 1003.1-1990 (“POSIX.1”).

     From the IEEE Std 1003.1-1990 (“POSIX.1”) Rationale:

     4.3BSD provides a getpgrp() function that returns the process group ID
     for a specified process.  Although this function is used to support job
     control, all known job-control shells always specify the calling process
     with this function.  Thus, the simpler AT&T System V UNIX getpgrp()
     suffices, and the added complexity of the 4.3BSD getpgrp() has been
     omitted from POSIX.1.  The old functionality is available from the
     getpgid() function.

macOS 14.2                       June 4, 1993                       macOS 14.2
```