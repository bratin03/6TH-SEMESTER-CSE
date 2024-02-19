### SEMGET
```c
SEMGET(2)                                                                             Linux Programmer's Manual                                                                             SEMGET(2)

NAME
       semget - get a System V semaphore set identifier

SYNOPSIS
       #include <sys/types.h>
       #include <sys/ipc.h>
       #include <sys/sem.h>

       int semget(key_t key, int nsems, int semflg);

DESCRIPTION
       The semget() system call returns the System V semaphore set identifier associated with the argument key.  It may be used either to obtain the identifier of a previously created semaphore set
       (when semflg is zero and key does not have the value IPC_PRIVATE), or to create a new set.

       A new set of nsems semaphores is created if key has the value IPC_PRIVATE or if no existing semaphore set is associated with key and IPC_CREAT is specified in semflg.

       If semflg specifies both IPC_CREAT and IPC_EXCL and a semaphore set already exists for key, then semget() fails with errno set to EEXIST.  (This is analogous to the effect of the combination
       O_CREAT | O_EXCL for open(2).)

       Upon  creation, the least significant 9 bits of the argument semflg define the permissions (for owner, group and others) for the semaphore set.  These bits have the same format, and the same
       meaning, as the mode argument of open(2) (though the execute permissions are not meaningful for semaphores, and write permissions mean permission to alter semaphore values).

       When creating a new semaphore set, semget() initializes the set's associated data structure, semid_ds (see semctl(2)), as follows:

              sem_perm.cuid and sem_perm.uid are set to the effective user ID of the calling process.

              sem_perm.cgid and sem_perm.gid are set to the effective group ID of the calling process.

              The least significant 9 bits of sem_perm.mode are set to the least significant 9 bits of semflg.

              sem_nsems is set to the value of nsems.

              sem_otime is set to 0.

              sem_ctime is set to the current time.

       The argument nsems can be 0 (a don't care) when a semaphore set is not being created.  Otherwise, nsems must be greater than 0 and less than or equal to the maximum number of semaphores  per
       semaphore set (SEMMSL).

       If the semaphore set already exists, the permissions are verified.

RETURN VALUE
       If successful, the return value will be the semaphore set identifier (a nonnegative integer), otherwise, -1 is returned, with errno indicating the error.

ERRORS
       On failure, errno will be set to one of the following:

       EACCES A  semaphore  set exists for key, but the calling process does not have permission to access the set, and does not have the CAP_IPC_OWNER capability in the user namespace that governs
              its IPC namespace.

       EEXIST IPC_CREAT and IPC_EXCL were specified in semflg, but a semaphore set already exists for key.

       EINVAL nsems is less than 0 or greater than the limit on the number of semaphores per semaphore set (SEMMSL).

       EINVAL A semaphore set corresponding to key already exists, but nsems is larger than the number of semaphores in that set.

       ENOENT No semaphore set exists for key and semflg did not specify IPC_CREAT.

       ENOMEM A semaphore set has to be created but the system does not have enough memory for the new data structure.

       ENOSPC A semaphore set has to be created but the system limit for the maximum number of semaphore sets (SEMMNI), or the system wide maximum number of semaphores (SEMMNS), would be exceeded.

CONFORMING TO
       SVr4, POSIX.1-2001.

NOTES
       The inclusion of <sys/types.h> and <sys/ipc.h> isn't required on Linux or by any version of POSIX.  However, some old implementations required the inclusion of these header  files,  and  the
       SVID also documented their inclusion.  Applications intended to be portable to such old systems may need to include these header files.

       IPC_PRIVATE  isn't  a  flag field but a key_t type.  If this special value is used for key, the system call ignores all but the least significant 9 bits of semflg and creates a new semaphore
       set (on success).

   Semaphore initialization
       The values of the semaphores in a newly created set are indeterminate.  (POSIX.1-2001 and POSIX.1-2008 are explicit on this point, although POSIX.1-2008 notes that a future  version  of  the
       standard  may  require  an  implementation to initialize the semaphores to 0.)  Although Linux, like many other implementations, initializes the semaphore values to 0, a portable application
       cannot rely on this: it should explicitly initialize the semaphores to the desired values.

       Initialization can be done using semctl(2) SETVAL or SETALL operation.  Where multiple peers do not know who will be the first to initialize the set, checking for a nonzero sem_otime in  the
       associated data structure retrieved by a semctl(2) IPC_STAT operation can be used to avoid races.

   Semaphore limits
       The following limits on semaphore set resources affect the semget() call:

       SEMMNI System-wide limit on the number of semaphore sets.  On Linux systems before version 3.19, the default value for this limit was 128.  Since Linux 3.19, the default value is 32,000.  On
              Linux, this limit can be read and modified via the fourth field of /proc/sys/kernel/sem.

       SEMMSL Maximum number of semaphores per semaphore ID.  On Linux systems before version 3.19, the default value for this limit was 250.  Since Linux 3.19, the default  value  is  32,000.   On
              Linux, this limit can be read and modified via the first field of /proc/sys/kernel/sem.

       SEMMNS System-wide  limit on the number of semaphores: policy dependent (on Linux, this limit can be read and modified via the second field of /proc/sys/kernel/sem).  Note that the number of
              semaphores system-wide is also limited by the product of SEMMSL and SEMMNI.

BUGS
       The name choice IPC_PRIVATE was perhaps unfortunate, IPC_NEW would more clearly show its function.

SEE ALSO
       semctl(2), semop(2), ftok(3), capabilities(7), sem_overview(7), sysvipc(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A description of the project, information about reporting bugs, and the latest version  of  this  page,  can  be  found  at
       https://www.kernel.org/doc/man-pages/.

Linux                                                                                         2019-08-02                                                                                    SEMGET(2)
```

### SEMCTL

```c
SEMCTL(2)                                                                             Linux Programmer's Manual                                                                             SEMCTL(2)

NAME
       semctl - System V semaphore control operations

SYNOPSIS
       #include <sys/types.h>
       #include <sys/ipc.h>
       #include <sys/sem.h>

       int semctl(int semid, int semnum, int cmd, ...);

DESCRIPTION
       semctl()  performs the control operation specified by cmd on the System V semaphore set identified by semid, or on the semnum-th semaphore of that set.  (The semaphores in a set are numbered
       starting at 0.)

       This function has three or four arguments, depending on cmd.  When there are four, the fourth has the type union semun.  The calling program must define this union as follows:

           union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

       The semid_ds data structure is defined in <sys/sem.h> as follows:

           struct semid_ds {
               struct ipc_perm sem_perm;  /* Ownership and permissions */
               time_t          sem_otime; /* Last semop time */
               time_t          sem_ctime; /* Last change time */
               unsigned long   sem_nsems; /* No. of semaphores in set */
           };

       The ipc_perm structure is defined as follows (the highlighted fields are settable using IPC_SET):

           struct ipc_perm {
               key_t          __key; /* Key supplied to semget(2) */
               uid_t          uid;   /* Effective UID of owner */
               gid_t          gid;   /* Effective GID of owner */
               uid_t          cuid;  /* Effective UID of creator */
               gid_t          cgid;  /* Effective GID of creator */
               unsigned short mode;  /* Permissions */
               unsigned short __seq; /* Sequence number */
           };

       Valid values for cmd are:

       IPC_STAT  Copy information from the kernel data structure associated with semid into the semid_ds structure pointed to by arg.buf.  The argument semnum is ignored.  The calling process  must
                 have read permission on the semaphore set.

       IPC_SET   Write  the  values of some members of the semid_ds structure pointed to by arg.buf to the kernel data structure associated with this semaphore set, updating also its sem_ctime mem‐
                 ber.  The following members of the structure are updated: sem_perm.uid, sem_perm.gid, and (the least significant 9 bits of) sem_perm.mode.  The effective UID of the calling process
                 must match the owner (sem_perm.uid) or creator (sem_perm.cuid) of the semaphore set, or the caller must be privileged.  The argument semnum is ignored.

       IPC_RMID  Immediately  remove  the  semaphore  set,  awakening all processes blocked in semop(2) calls on the set (with an error return and errno set to EIDRM).  The effective user ID of the
                 calling process must match the creator or owner of the semaphore set, or the caller must be privileged.  The argument semnum is ignored.

       IPC_INFO (Linux-specific)
                 Return information about system-wide semaphore limits and parameters in the structure pointed to by arg.__buf.  This structure is of type seminfo, defined  in  <sys/sem.h>  if  the
                 _GNU_SOURCE feature test macro is defined:

                     struct  seminfo {
                         int semmap;  /* Number of entries in semaphore
                                         map; unused within kernel */
                         int semmni;  /* Maximum number of semaphore sets */
                         int semmns;  /* Maximum number of semaphores in all
                                         semaphore sets */
                         int semmnu;  /* System-wide maximum number of undo
                                         structures; unused within kernel */
                         int semmsl;  /* Maximum number of semaphores in a
                                         set */
                         int semopm;  /* Maximum number of operations for
                                         semop(2) */
                         int semume;  /* Maximum number of undo entries per
                                         process; unused within kernel */
                         int semusz;  /* Size of struct sem_undo */
                         int semvmx;  /* Maximum semaphore value */
                         int semaem;  /* Max. value that can be recorded for
                                         semaphore adjustment (SEM_UNDO) */
                     };

                 The semmsl, semmns, semopm, and semmni settings can be changed via /proc/sys/kernel/sem; see proc(5) for details.

       SEM_INFO (Linux-specific)
                 Return  a seminfo structure containing the same information as for IPC_INFO, except that the following fields are returned with information about system resources consumed by sema‐
                 phores: the semusz field returns the number of semaphore sets that currently exist on the system; and the semaem field returns the total number of semaphores in all semaphore  sets
                 on the system.

       SEM_STAT (Linux-specific)
                 Return a semid_ds structure as for IPC_STAT.  However, the semid argument is not a semaphore identifier, but instead an index into the kernel's internal array that maintains infor‐
                 mation about all semaphore sets on the system.

       SEM_STAT_ANY (Linux-specific, since Linux 4.17)
                 Return a seminfo structure containing the same information as for SEM_STAT.  However, sem_perm.mode is not checked for read access for semid meaning that any user can  employ  this
                 operation (just as any user may read /proc/sysvipc/sem to obtain the same information).

       GETALL    Return  semval  (i.e., the current value) for all semaphores of the set into arg.array.  The argument semnum is ignored.  The calling process must have read permission on the sema‐
                 phore set.

       GETNCNT   Return the value of semncnt for the semnum-th semaphore of the set (i.e., the number of processes waiting for an increase of semval for the semnum-th semaphore of  the  set).   The
                 calling process must have read permission on the semaphore set.

       GETPID    Return  the value of sempid for the semnum-th semaphore of the set.  This is the PID of the process that last performed an operation on that semaphore (but see NOTES).  The calling
                 process must have read permission on the semaphore set.

       GETVAL    Return the value of semval for the semnum-th semaphore of the set.  The calling process must have read permission on the semaphore set.

       GETZCNT   Return the value of semzcnt for the semnum-th semaphore of the set (i.e., the number of processes waiting for semval of the semnum-th semaphore of the set to become 0).  The  call‐
                 ing process must have read permission on the semaphore set.

       SETALL    Set  semval  for  all  semaphores of the set using arg.array, updating also the sem_ctime member of the semid_ds structure associated with the set.  Undo entries (see semop(2)) are
                 cleared for altered semaphores in all processes.  If the changes to semaphore values would permit blocked semop(2) calls in other processes to proceed, then those processes are wo‐
                 ken up.  The argument semnum is ignored.  The calling process must have alter (write) permission on the semaphore set.

       SETVAL    Set  the  value of semval to arg.val for the semnum-th semaphore of the set, updating also the sem_ctime member of the semid_ds structure associated with the set.  Undo entries are
                 cleared for altered semaphores in all processes.  If the changes to semaphore values would permit blocked semop(2) calls in other processes to proceed, then those processes are wo‐
                 ken up.  The calling process must have alter permission on the semaphore set.

RETURN VALUE
       On failure, semctl() returns -1 with errno indicating the error.

       Otherwise, the system call returns a nonnegative value depending on cmd as follows:

       GETNCNT   the value of semncnt.

       GETPID    the value of sempid.

       GETVAL    the value of semval.

       GETZCNT   the value of semzcnt.

       IPC_INFO  the  index  of  the  highest  used  entry  in  the  kernel's internal array recording information about all semaphore sets.  (This information can be used with repeated SEM_STAT or
                 SEM_STAT_ANY operations to obtain information about all semaphore sets on the system.)

       SEM_INFO  as for IPC_INFO.

       SEM_STAT  the identifier of the semaphore set whose index was given in semid.

       SEM_STAT_ANY
                 as for SEM_STAT.

       All other cmd values return 0 on success.

ERRORS
       On failure, errno will be set to one of the following:

       EACCES The argument cmd has one of the values GETALL, GETPID, GETVAL, GETNCNT, GETZCNT, IPC_STAT, SEM_STAT, SEM_STAT_ANY, SETALL, or SETVAL and the calling process does not have the required
              permissions on the semaphore set and does not have the CAP_IPC_OWNER capability in the user namespace that governs its IPC namespace.

       EFAULT The address pointed to by arg.buf or arg.array isn't accessible.

       EIDRM  The semaphore set was removed.

       EINVAL Invalid value for cmd or semid.  Or: for a SEM_STAT operation, the index value specified in semid referred to an array slot that is currently unused.

       EPERM  The  argument  cmd  has  the  value  IPC_SET  or  IPC_RMID  but  the  effective user ID of the calling process is not the creator (as found in sem_perm.cuid) or the owner (as found in
              sem_perm.uid) of the semaphore set, and the process does not have the CAP_SYS_ADMIN capability.

       ERANGE The argument cmd has the value SETALL or SETVAL and the value to which semval is to be set (for some semaphore of the set) is less than 0 or  greater  than  the  implementation  limit
              SEMVMX.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4.

       POSIX.1 specifies the sem_nsems field of the semid_ds structure as having the type unsigned short, and the field is so defined on most other systems.  It was also so defined on Linux 2.2 and
       earlier, but, since Linux 2.4, the field has the type unsigned long.

NOTES
       The inclusion of <sys/types.h> and <sys/ipc.h> isn't required on Linux or by any version of POSIX.  However, some old implementations required the inclusion of these header  files,  and  the
       SVID also documented their inclusion.  Applications intended to be portable to such old systems may need to include these header files.

       The IPC_INFO, SEM_STAT and SEM_INFO operations are used by the ipcs(1) program to provide information on allocated resources.  In the future these may modified or moved to a /proc filesystem
       interface.

       Various fields in a struct semid_ds were typed as short under Linux 2.2 and have become long under Linux 2.4.  To take advantage of this, a recompilation under glibc-2.1.91 or  later  should
       suffice.  (The kernel distinguishes old and new calls by an IPC_64 flag in cmd.)

       In  some earlier versions of glibc, the semun union was defined in <sys/sem.h>, but POSIX.1 requires that the caller define this union.  On versions of glibc where this union is not defined,
       the macro _SEM_SEMUN_UNDEFINED is defined in <sys/sem.h>.

       The following system limit on semaphore sets affects a semctl() call:

       SEMVMX Maximum value for semval: implementation dependent (32767).

       For greater portability, it is best to always call semctl() with four arguments.

   The sempid value
       POSIX.1 defines sempid as the "process ID of [the] last operation" on a semaphore, and explicitly notes that this value is set by a successful semop(2) call, with  the  implication  that  no
       other interface affects the sempid value.

       While  some implementations conform to the behavior specified in POSIX.1, others do not.  (The fault here probably lies with POSIX.1 inasmuch as it likely failed to capture the full range of
       existing implementation behaviors.)  Various other implementations also update sempid for the other operations that update the value of a semaphore: the SETVAL and SETALL operations, as well
       as the semaphore adjustments performed on process termination as a consequence of the use of the SEM_UNDO flag (see semop(2)).

       Linux  also  updates  sempid  for  SETVAL operations and semaphore adjustments.  However, somewhat inconsistently, up to and including 4.5, Linux did not update sempid for SETALL operations.
       This was rectified in Linux 4.6.

SEE ALSO
       ipc(2), semget(2), semop(2), capabilities(7), sem_overview(7), sysvipc(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A description of the project, information about reporting bugs, and the latest version  of  this  page,  can  be  found  at
       https://www.kernel.org/doc/man-pages/.

Linux                                                                                         2019-08-02                                                                                    SEMCTL(2)
```

### SEMOP

```c
SEMOP(2)                                                                              Linux Programmer's Manual                                                                              SEMOP(2)

NAME
       semop, semtimedop - System V semaphore operations

SYNOPSIS
       #include <sys/types.h>
       #include <sys/ipc.h>
       #include <sys/sem.h>

       int semop(int semid, struct sembuf *sops, size_t nsops);

       int semtimedop(int semid, struct sembuf *sops, size_t nsops,
                      const struct timespec *timeout);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       semtimedop(): _GNU_SOURCE

DESCRIPTION
       Each semaphore in a System V semaphore set has the following associated values:

           unsigned short  semval;   /* semaphore value */
           unsigned short  semzcnt;  /* # waiting for zero */
           unsigned short  semncnt;  /* # waiting for increase */
           pid_t           sempid;   /* PID of process that last

       semop() performs operations on selected semaphores in the set indicated by semid.  Each of the nsops elements in the array pointed to by sops is a structure that specifies an operation to be
       performed on a single semaphore.  The elements of this structure are of type struct sembuf, containing the following members:

           unsigned short sem_num;  /* semaphore number */
           short          sem_op;   /* semaphore operation */
           short          sem_flg;  /* operation flags */

       Flags recognized in sem_flg are IPC_NOWAIT and SEM_UNDO.  If an operation specifies SEM_UNDO, it will be automatically undone when the process terminates.

       The set of operations contained in sops is performed in array order, and atomically, that is, the operations are performed either as a complete unit, or not at all.  The behavior of the sys‐
       tem call if not all operations can be performed immediately depends on the presence of the IPC_NOWAIT flag in the individual sem_flg fields, as noted below.

       Each  operation  is  performed on the sem_num-th semaphore of the semaphore set, where the first semaphore of the set is numbered 0.  There are three types of operation, distinguished by the
       value of sem_op.

       If sem_op is a positive integer, the operation adds this value to the semaphore value (semval).  Furthermore, if SEM_UNDO is specified for this operation,  the  system  subtracts  the  value
       sem_op  from  the semaphore adjustment (semadj) value for this semaphore.  This operation can always proceed—it never forces a thread to wait.  The calling process must have alter permission
       on the semaphore set.

       If sem_op is zero, the process must have read permission on the semaphore set.  This is a "wait-for-zero" operation: if semval is zero, the operation can immediately proceed.  Otherwise,  if
       IPC_NOWAIT  is  specified  in  sem_flg, semop() fails with errno set to EAGAIN (and none of the operations in sops is performed).  Otherwise, semzcnt (the count of threads waiting until this
       semaphore's value becomes zero) is incremented by one and the thread sleeps until one of the following occurs:

       •  semval becomes 0, at which time the value of semzcnt is decremented.

       •  The semaphore set is removed: semop() fails, with errno set to EIDRM.

       •  The calling thread catches a signal: the value of semzcnt is decremented and semop() fails, with errno set to EINTR.

       If sem_op is less than zero, the process must have alter permission on the semaphore set.  If semval is greater than or equal to the absolute value of sem_op, the operation can proceed imme‐
       diately:  the  absolute  value of sem_op is subtracted from semval, and, if SEM_UNDO is specified for this operation, the system adds the absolute value of sem_op to the semaphore adjustment
       (semadj) value for this semaphore.  If the absolute value of sem_op is greater than semval, and IPC_NOWAIT is specified in sem_flg, semop() fails, with errno set to EAGAIN (and none  of  the
       operations  in  sops  is  performed).  Otherwise, semncnt (the counter of threads waiting for this semaphore's value to increase) is incremented by one and the thread sleeps until one of the
       following occurs:

       •  semval becomes greater than or equal to the absolute value of sem_op: the operation now proceeds, as described above.

       •  The semaphore set is removed from the system: semop() fails, with errno set to EIDRM.

       •  The calling thread catches a signal: the value of semncnt is decremented and semop() fails, with errno set to EINTR.

       On successful completion, the sempid value for each semaphore specified in the array pointed to by sops is set to the caller's process ID.  In addition, the sem_otime is set to  the  current
       time.

   semtimedop()
       semtimedop()  behaves identically to semop() except that in those cases where the calling thread would sleep, the duration of that sleep is limited by the amount of elapsed time specified by
       the timespec structure whose address is passed in the timeout argument.  (This sleep interval will be rounded up to the system clock granularity, and kernel scheduling delays mean  that  the
       interval may overrun by a small amount.)  If the specified time limit has been reached, semtimedop() fails with errno set to EAGAIN (and none of the operations in sops is performed).  If the
       timeout argument is NULL, then semtimedop() behaves exactly like semop().

       Note that if semtimedop() is interrupted by a signal, causing the call to fail with the error EINTR, the contents of timeout are left unchanged.

RETURN VALUE
       If successful, semop() and semtimedop() return 0; otherwise they return -1 with errno indicating the error.

ERRORS
       On failure, errno is set to one of the following:

       E2BIG  The argument nsops is greater than SEMOPM, the maximum number of operations allowed per system call.

       EACCES The calling process does not have the permissions required to perform the specified semaphore operations, and does not have the CAP_IPC_OWNER capability in  the  user  namespace  that
              governs its IPC namespace.

       EAGAIN An operation could not proceed immediately and either IPC_NOWAIT was specified in sem_flg or the time limit specified in timeout expired.

       EFAULT An address specified in either the sops or the timeout argument isn't accessible.

       EFBIG  For some operation the value of sem_num is less than 0 or greater than or equal to the number of semaphores in the set.

       EIDRM  The semaphore set was removed.

       EINTR  While blocked in this system call, the thread caught a signal; see signal(7).

       EINVAL The semaphore set doesn't exist, or semid is less than zero, or nsops has a nonpositive value.

       ENOMEM The sem_flg of some operation specified SEM_UNDO and the system does not have enough memory to allocate the undo structure.

       ERANGE For some operation sem_op+semval is greater than SEMVMX, the implementation dependent maximum value for semval.

VERSIONS
       semtimedop() first appeared in Linux 2.5.52, and was subsequently backported into kernel 2.4.22.  Glibc support for semtimedop() first appeared in version 2.3.3.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4.

NOTES
       The  inclusion  of  <sys/types.h> and <sys/ipc.h> isn't required on Linux or by any version of POSIX.  However, some old implementations required the inclusion of these header files, and the
       SVID also documented their inclusion.  Applications intended to be portable to such old systems may need to include these header files.

       The sem_undo structures of a process aren't inherited by the child produced by fork(2), but they are inherited across an execve(2) system call.

       semop() is never automatically restarted after being interrupted by a signal handler, regardless of the setting of the SA_RESTART flag when establishing a signal handler.

       A semaphore adjustment (semadj) value is a per-process, per-semaphore integer that is the negated sum of all operations performed on a semaphore specifying the SEM_UNDO flag.   Each  process
       has  a  list of semadj values—one value for each semaphore on which it has operated using SEM_UNDO.  When a process terminates, each of its per-semaphore semadj values is added to the corre‐
       sponding semaphore, thus undoing the effect of that process's operations on the semaphore (but see BUGS below).  When a semaphore's value is directly set using the SETVAL or  SETALL  request
       to semctl(2), the corresponding semadj values in all processes are cleared.  The clone(2) CLONE_SYSVSEM flag allows more than one process to share a semadj list; see clone(2) for details.

       The semval, sempid, semzcnt, and semnct values for a semaphore can all be retrieved using appropriate semctl(2) calls.

   Semaphore limits
       The following limits on semaphore set resources affect the semop() call:

       SEMOPM Maximum  number  of  operations allowed for one semop() call.  Before Linux 3.19, the default value for this limit was 32.  Since Linux 3.19, the default value is 500.  On Linux, this
              limit can be read and modified via the third field of /proc/sys/kernel/sem.  Note: this limit should not be raised above 1000, because of the risk of that semop() fails due to  kernel
              memory fragmentation when allocating memory to copy the sops array.

       SEMVMX Maximum allowable value for semval: implementation dependent (32767).

       The  implementation  has  no  intrinsic limits for the adjust on exit maximum value (SEMAEM), the system wide maximum number of undo structures (SEMMNU) and the per-process maximum number of
       undo entries system parameters.

BUGS
       When a process terminates, its set of associated semadj structures is used to undo the effect of all of the semaphore operations it performed with the SEM_UNDO flag.  This  raises  a  diffi‐
       culty:  if  one (or more) of these semaphore adjustments would result in an attempt to decrease a semaphore's value below zero, what should an implementation do?  One possible approach would
       be to block until all the semaphore adjustments could be performed.  This is however undesirable since it could force process termination to block for arbitrarily long periods.  Another pos‐
       sibility  is  that  such semaphore adjustments could be ignored altogether (somewhat analogously to failing when IPC_NOWAIT is specified for a semaphore operation).  Linux adopts a third ap‐
       proach: decreasing the semaphore value as far as possible (i.e., to zero) and allowing process termination to proceed immediately.

       In kernels 2.6.x, x <= 10, there is a bug that in some circumstances prevents a thread that is waiting for a semaphore value to become zero from being woken up when the value  does  actually
       become zero.  This bug is fixed in kernel 2.6.11.

EXAMPLE
       The following code segment uses semop() to atomically wait for the value of semaphore 0 to become zero, and then increment the semaphore value by one.

           struct sembuf sops[2];
           int semid;

           /* Code to set semid omitted */

           sops[0].sem_num = 0;        /* Operate on semaphore 0 */
           sops[0].sem_op = 0;         /* Wait for value to equal 0 */
           sops[0].sem_flg = 0;

           sops[1].sem_num = 0;        /* Operate on semaphore 0 */
           sops[1].sem_op = 1;         /* Increment value by one */
           sops[1].sem_flg = 0;

           if (semop(semid, sops, 2) == -1) {
               perror("semop");
               exit(EXIT_FAILURE);
           }

SEE ALSO
       clone(2), semctl(2), semget(2), sigaction(2), capabilities(7), sem_overview(7), sysvipc(7), time(7)

COLOPHON
       This  page  is  part  of  release  5.05  of the Linux man-pages project.  A description of the project, information about reporting bugs, and the latest version of this page, can be found at
       https://www.kernel.org/doc/man-pages/.

Linux                                                                                         2019-08-02                                                                                     SEMOP(2)
```