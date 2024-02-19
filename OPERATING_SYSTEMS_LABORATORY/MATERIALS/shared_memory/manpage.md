### SHMGET

```c
SHMGET(2)                       Linux Programmer's Manual                      SHMGET(2)

NAME
       shmget - allocates a System V shared memory segment

SYNOPSIS
       #include <sys/ipc.h>
       #include <sys/shm.h>

       int shmget(key_t key, size_t size, int shmflg);

DESCRIPTION
       shmget()  returns the identifier of the System V shared memory segment associated
       with the value of the argument key.  It may be used either to obtain the  identi‐
       fier  of  a previously created shared memory segment (when shmflg is zero and key
       does not have the value IPC_PRIVATE), or to create a new set.

       A new shared memory segment, with size equal to the value of size rounded up to a
       multiple  of  PAGE_SIZE, is created if key has the value IPC_PRIVATE or key isn't
       IPC_PRIVATE, no shared memory segment corresponding to key exists, and  IPC_CREAT
       is specified in shmflg.

       If  shmflg  specifies both IPC_CREAT and IPC_EXCL and a shared memory segment al‐
       ready exists for key, then shmget() fails with errno set  to  EEXIST.   (This  is
       analogous to the effect of the combination O_CREAT | O_EXCL for open(2).)

       The value shmflg is composed of:

       IPC_CREAT   Create  a  new segment.  If this flag is not used, then shmget() will
                   find the segment associated with key and check to see if the user has
                   permission to access the segment.

       IPC_EXCL    This flag is used with IPC_CREAT to ensure that this call creates the
                   segment.  If the segment already exists, the call fails.

       SHM_HUGETLB (since Linux 2.6)
                   Allocate the segment using "huge pages."  See the Linux kernel source
                   file  Documentation/admin-guide/mm/hugetlbpage.rst for further infor‐
                   mation.

       SHM_HUGE_2MB, SHM_HUGE_1GB (since Linux 3.8)
                   Used in conjunction with SHM_HUGETLB to  select  alternative  hugetlb
                   page sizes (respectively, 2 MB and 1 GB) on systems that support mul‐
                   tiple hugetlb page sizes.

                   More generally, the desired huge page size can be configured  by  en‐
                   coding  the base-2 logarithm of the desired page size in the six bits
                   at the offset SHM_HUGE_SHIFT.  Thus, the above two constants are  de‐
                   fined as:

                       #define SHM_HUGE_2MB    (21 << SHM_HUGE_SHIFT)
                       #define SHM_HUGE_1GB    (30 << SHM_HUGE_SHIFT)

                   For  some  additional  details,  see  the discussion of the similarly
                   named constants in mmap(2).

       SHM_NORESERVE (since Linux 2.6.15)
                   This flag serves the same purpose as the mmap(2) MAP_NORESERVE  flag.
                   Do  not  reserve swap space for this segment.  When swap space is re‐
                   served, one has the guarantee that it is possible to modify the  seg‐
                   ment.   When  swap space is not reserved one might get SIGSEGV upon a
                   write if no physical memory is available.  See also the discussion of
                   the file /proc/sys/vm/overcommit_memory in proc(5).

       In  addition  to  the above flags, the least significant 9 bits of shmflg specify
       the permissions granted to the owner, group, and others.   These  bits  have  the
       same  format,  and the same meaning, as the mode argument of open(2).  Presently,
       execute permissions are not used by the system.

       When a new shared memory segment is created, its contents are initialized to zero
       values,  and its associated data structure, shmid_ds (see shmctl(2)), is initial‐
       ized as follows:

              shm_perm.cuid and shm_perm.uid are set to the effective  user  ID  of  the
              calling process.

              shm_perm.cgid  and  shm_perm.gid  are set to the effective group ID of the
              calling process.

              The least significant 9 bits of shm_perm.mode are set to the least signif‐
              icant 9 bit of shmflg.

              shm_segsz is set to the value of size.

              shm_lpid, shm_nattch, shm_atime, and shm_dtime are set to 0.

              shm_ctime is set to the current time.

       If  the shared memory segment already exists, the permissions are verified, and a
       check is made to see if it is marked for destruction.

RETURN VALUE
       On success, a valid shared memory identifier is returned.  On error,  -1  is  re‐
       turned, and errno is set to indicate the error.

ERRORS
       On failure, errno is set to one of the following:

       EACCES The user does not have permission to access the shared memory segment, and
              does not have the CAP_IPC_OWNER capability in the user namespace that gov‐
              erns its IPC namespace.

       EEXIST IPC_CREAT  and IPC_EXCL were specified in shmflg, but a shared memory seg‐
              ment already exists for key.

       EINVAL A new segment was to be created and size is less than  SHMMIN  or  greater
              than SHMMAX.

       EINVAL A  segment  for the given key exists, but size is greater than the size of
              that segment.

       ENFILE The system-wide limit on the total number of open files has been reached.

       ENOENT No segment exists for the given key, and IPC_CREAT was not specified.

       ENOMEM No memory could be allocated for segment overhead.

       ENOSPC All possible shared memory IDs have been taken (SHMMNI), or  allocating  a
              segment of the requested size would cause the system to exceed the system-
              wide limit on shared memory (SHMALL).

       EPERM  The SHM_HUGETLB flag was specified, but the caller was not privileged (did
              not have the CAP_IPC_LOCK capability).

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4.

       SHM_HUGETLB and SHM_NORESERVE are Linux extensions.

NOTES
       The  inclusion of <sys/types.h> and <sys/ipc.h> isn't required on Linux or by any
       version of POSIX.  However, some old implementations required  the  inclusion  of
       these  header  files, and the SVID also documented their inclusion.  Applications
       intended to be portable to such old systems may  need  to  include  these  header
       files.

       IPC_PRIVATE  isn't  a flag field but a key_t type.  If this special value is used
       for key, the system call ignores all but the least significant 9 bits  of  shmflg
       and creates a new shared memory segment.

   Shared memory limits
       The following limits on shared memory segment resources affect the shmget() call:

       SHMALL System-wide  limit on the total amount of shared memory, measured in units
              of the system page size.

              On Linux, this limit can be read and modified via /proc/sys/kernel/shmall.
              Since Linux 3.16, the default value for this limit is:

                  ULONG_MAX - 2^24

              The  effect  of  this  value (which is suitable for both 32-bit and 64-bit
              systems) is to impose no limitation on allocations.   This  value,  rather
              than ULONG_MAX, was chosen as the default to prevent some cases where his‐
              torical applications simply raised the existing limit without first check‐
              ing  its  current value.  Such applications would cause the value to over‐
              flow if the limit was set at ULONG_MAX.

              From Linux 2.4 up to Linux 3.15, the default value for this limit was:

                  SHMMAX / PAGE_SIZE * (SHMMNI / 16)

              If SHMMAX and SHMMNI were not modified, then  multiplying  the  result  of
              this formula by the page size (to get a value in bytes) yielded a value of
              8 GB as the limit on the total memory used by all shared memory segments.

       SHMMAX Maximum size in bytes for a shared memory segment.

              On Linux, this limit can be read and modified via /proc/sys/kernel/shmmax.
              Since Linux 3.16, the default value for this limit is:

                  ULONG_MAX - 2^24

              The  effect  of  this  value (which is suitable for both 32-bit and 64-bit
              systems) is to impose no limitation on allocations.  See  the  description
              of  SHMALL  for  a  discussion  of  why  this  default  value (rather than
              ULONG_MAX) is used.

              From Linux 2.2 up to Linux 3.15, the  default  value  of  this  limit  was
              0x2000000 (32 MB).

              Because  it  is  not possible to map just part of a shared memory segment,
              the amount of virtual memory places another limit on the maximum size of a
              usable  segment:  for  example,  on  i386 the largest segments that can be
              mapped have a size of around 2.8 GB, and on x86-64 the limit is around 127
              TB.

       SHMMIN Minimum  size  in bytes for a shared memory segment: implementation depen‐
              dent (currently 1 byte, though PAGE_SIZE is the effective minimum size).

       SHMMNI System-wide limit on the number of shared memory segments.  In Linux  2.2,
              the  default  value  for  this limit was 128; since Linux 2.4, the default
              value is 4096.

              On Linux, this limit can be read and modified via /proc/sys/kernel/shmmni.

       The implementation has no specific limits for the per-process maximum  number  of
       shared memory segments (SHMSEG).

   Linux notes
       Until  version 2.3.30, Linux would return EIDRM for a shmget() on a shared memory
       segment scheduled for deletion.

BUGS
       The name choice IPC_PRIVATE was perhaps unfortunate, IPC_NEW would  more  clearly
       show its function.

SEE ALSO
       memfd_create(2),   shmat(2),   shmctl(2),   shmdt(2),  ftok(3),  capabilities(7),
       shm_overview(7), sysvipc(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description
       of  the project, information about reporting bugs, and the latest version of this
       page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                  2019-08-02                              SHMGET(2)
```

### SHMOP

```c
SHMOP(2)                                                                              Linux Programmer's Manual                                                                              SHMOP(2)

NAME
       shmat, shmdt - System V shared memory operations

SYNOPSIS
       #include <sys/types.h>
       #include <sys/shm.h>

       void *shmat(int shmid, const void *shmaddr, int shmflg);

       int shmdt(const void *shmaddr);

DESCRIPTION
   shmat()
       shmat()  attaches the System V shared memory segment identified by shmid to the address space of the calling process.  The attaching address is specified by shmaddr with one of the following
       criteria:

       *  If shmaddr is NULL, the system chooses a suitable (unused) page-aligned address to attach the segment.

       *  If shmaddr isn't NULL and SHM_RND is specified in shmflg, the attach occurs at the address equal to shmaddr rounded down to the nearest multiple of SHMLBA.

       *  Otherwise, shmaddr must be a page-aligned address at which the attach occurs.

       In addition to SHM_RND, the following flags may be specified in the shmflg bit-mask argument:

       SHM_EXEC (Linux-specific; since Linux 2.6.9)
              Allow the contents of the segment to be executed.  The caller must have execute permission on the segment.

       SHM_RDONLY
              Attach the segment for read-only access.  The process must have read permission for the segment.  If this flag is not specified, the segment is attached for read and write access, and
              the process must have read and write permission for the segment.  There is no notion of a write-only shared memory segment.

       SHM_REMAP (Linux-specific)
              This  flag  specifies  that  the mapping of the segment should replace any existing mapping in the range starting at shmaddr and continuing for the size of the segment.  (Normally, an
              EINVAL error would result if a mapping already exists in this address range.)  In this case, shmaddr must not be NULL.

       The brk(2) value of the calling process is not altered by the attach.  The segment will automatically be detached at process exit.  The same segment may be attached as a read and as a  read-
       write one, and more than once, in the process's address space.

       A successful shmat() call updates the members of the shmid_ds structure (see shmctl(2)) associated with the shared memory segment as follows:

              shm_atime is set to the current time.

              shm_lpid is set to the process-ID of the calling process.

              shm_nattch is incremented by one.

   shmdt()
       shmdt()  detaches  the shared memory segment located at the address specified by shmaddr from the address space of the calling process.  The to-be-detached segment must be currently attached
       with shmaddr equal to the value returned by the attaching shmat() call.

       On a successful shmdt() call, the system updates the members of the shmid_ds structure associated with the shared memory segment as follows:

              shm_dtime is set to the current time.

              shm_lpid is set to the process-ID of the calling process.

              shm_nattch is decremented by one.  If it becomes 0 and the segment is marked for deletion, the segment is deleted.

RETURN VALUE
       On success, shmat() returns the address of the attached shared memory segment; on error, (void *) -1 is returned, and errno is set to indicate the cause of the error.

       On success, shmdt() returns 0; on error -1 is returned, and errno is set to indicate the cause of the error.

ERRORS
       When shmat() fails, errno is set to one of the following:

       EACCES The calling process does not have the required permissions for the requested attach type, and does not have the CAP_IPC_OWNER capability in the user namespace  that  governs  its  IPC
              namespace.

       EIDRM  shmid points to a removed identifier.

       EINVAL Invalid  shmid  value,  unaligned  (i.e.,  not page-aligned and SHM_RND was not specified) or invalid shmaddr value, or can't attach segment at shmaddr, or SHM_REMAP was specified and
              shmaddr was NULL.

       ENOMEM Could not allocate memory for the descriptor or for the page tables.

       When shmdt() fails, errno is set as follows:

       EINVAL There is no shared memory segment attached at shmaddr; or, shmaddr is not aligned on a page boundary.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4.

       In SVID 3 (or perhaps earlier), the type of the shmaddr argument was changed from char * into const void *, and the returned type of shmat() from char * into void *.

NOTES
       After a fork(2), the child inherits the attached shared memory segments.

       After an execve(2), all attached shared memory segments are detached from the process.

       Upon _exit(2), all attached shared memory segments are detached from the process.

       Using shmat() with shmaddr equal to NULL is the preferred, portable way of attaching a shared memory segment.  Be aware that the shared memory segment attached in this way may be attached at
       different addresses in different processes.  Therefore, any pointers maintained within the shared memory must be made relative (typically to the starting address of the segment), rather than
       absolute.

       On Linux, it is possible to attach a shared memory segment even if it is already marked to be deleted.  However, POSIX.1 does not specify this behavior and many other implementations do  not
       support it.

       The following system parameter affects shmat():

       SHMLBA Segment low boundary address multiple.  When explicitly specifying an attach address in a call to shmat(), the caller should ensure that the address is a multiple of this value.  This
              is necessary on some architectures, in order either to ensure good CPU cache performance or to ensure that different attaches of the same segment have consistent views within the  CPU
              cache.  SHMLBA is normally some multiple of the system page size.  (On many Linux architectures, SHMLBA is the same as the system page size.)

       The implementation places no intrinsic per-process limit on the number of shared memory segments (SHMSEG).

SEE ALSO
       brk(2), mmap(2), shmctl(2), shmget(2), capabilities(7), shm_overview(7), sysvipc(7)

COLOPHON
       This  page  is  part  of  release  5.05  of the Linux man-pages project.  A description of the project, information about reporting bugs, and the latest version of this page, can be found at
       https://www.kernel.org/doc/man-pages/.

Linux                                                                                         2019-08-02                                                                                     SHMOP(2)
```

### SHMCTL

```c
SHMCTL(2)                                                                    Linux Programmer's Manual                                                                   SHMCTL(2)

NAME
       shmctl - System V shared memory control

SYNOPSIS
       #include <sys/ipc.h>
       #include <sys/shm.h>

       int shmctl(int shmid, int cmd, struct shmid_ds *buf);

DESCRIPTION
       shmctl() performs the control operation specified by cmd on the System V shared memory segment whose identifier is given in shmid.

       The buf argument is a pointer to a shmid_ds structure, defined in <sys/shm.h> as follows:

           struct shmid_ds {
               struct ipc_perm shm_perm;    /* Ownership and permissions */
               size_t          shm_segsz;   /* Size of segment (bytes) */
               time_t          shm_atime;   /* Last attach time */
               time_t          shm_dtime;   /* Last detach time */
               time_t          shm_ctime;   /* Last change time */
               pid_t           shm_cpid;    /* PID of creator */
               pid_t           shm_lpid;    /* PID of last shmat(2)/shmdt(2) */
               shmatt_t        shm_nattch;  /* No. of current attaches */
               ...
           };

       The ipc_perm structure is defined as follows (the highlighted fields are settable using IPC_SET):

           struct ipc_perm {
               key_t          __key;    /* Key supplied to shmget(2) */
               uid_t          uid;      /* Effective UID of owner */
               gid_t          gid;      /* Effective GID of owner */
               uid_t          cuid;     /* Effective UID of creator */
               gid_t          cgid;     /* Effective GID of creator */
               unsigned short mode;     /* Permissions + SHM_DEST and
                                           SHM_LOCKED flags */
               unsigned short __seq;    /* Sequence number */
           };

       Valid values for cmd are:

       IPC_STAT  Copy information from the kernel data structure associated with shmid into the shmid_ds structure pointed to by buf.  The caller must have read permission on the
                 shared memory segment.

       IPC_SET   Write the values of some members of the shmid_ds structure pointed to by buf to the kernel data structure associated with this shared  memory  segment,  updating
                 also  its shm_ctime member.  The following fields can be changed: shm_perm.uid, shm_perm.gid, and (the least significant 9 bits of) shm_perm.mode.  The effective
                 UID of the calling process must match the owner (shm_perm.uid) or creator (shm_perm.cuid) of the shared memory segment, or the caller must be privileged.

       IPC_RMID  Mark the segment to be destroyed.  The segment will actually be destroyed only after the last process detaches it (i.e., when the shm_nattch member of the  asso‐
                 ciated structure shmid_ds is zero).  The caller must be the owner or creator of the segment, or be privileged.  The buf argument is ignored.

                 If  a  segment  has  been  marked  for destruction, then the (nonstandard) SHM_DEST flag of the shm_perm.mode field in the associated data structure retrieved by
                 IPC_STAT will be set.

                 The caller must ensure that a segment is eventually destroyed; otherwise its pages that were faulted in will remain in memory or swap.

                 See also the description of /proc/sys/kernel/shm_rmid_forced in proc(5).

       IPC_INFO (Linux-specific)
                 Return information about system-wide shared memory limits and parameters in the structure pointed to by buf.  This structure is of type shminfo (thus, a cast  is
                 required), defined in <sys/shm.h> if the _GNU_SOURCE feature test macro is defined:

                     struct shminfo {
                         unsigned long shmmax; /* Maximum segment size */
                         unsigned long shmmin; /* Minimum segment size;
                                                  always 1 */
                         unsigned long shmmni; /* Maximum number of segments */
                         unsigned long shmseg; /* Maximum number of segments
                                                  that a process can attach;
                                                  unused within kernel */
                         unsigned long shmall; /* Maximum number of pages of
                                                  shared memory, system-wide */
                     };

                 The shmmni, shmmax, and shmall settings can be changed via /proc files of the same name; see proc(5) for details.

       SHM_INFO (Linux-specific)
                 Return  a  shm_info structure whose fields contain information about system resources consumed by shared memory.  This structure is defined in <sys/shm.h> if the
                 _GNU_SOURCE feature test macro is defined:

                     struct shm_info {
                         int           used_ids; /* # of currently existing
                                                    segments */
                         unsigned long shm_tot;  /* Total number of shared
                                                    memory pages */
                         unsigned long shm_rss;  /* # of resident shared
                                                    memory pages */
                         unsigned long shm_swp;  /* # of swapped shared
                                                    memory pages */
                         unsigned long swap_attempts;
                                                 /* Unused since Linux 2.4 */
                         unsigned long swap_successes;
                                                 /* Unused since Linux 2.4 */
                     };

       SHM_STAT (Linux-specific)
                 Return a shmid_ds structure as for IPC_STAT.  However, the shmid argument is not a segment identifier, but instead an index into the kernel's internal array that
                 maintains information about all shared memory segments on the system.

       SHM_STAT_ANY (Linux-specific, since Linux 4.17)
                 Return  a  shmid_ds  structure as for SHM_STAT.  However, shm_perm.mode is not checked for read access for shmid, meaning that any user can employ this operation
                 (just as any user may read /proc/sysvipc/shm to obtain the same information).

       The caller can prevent or allow swapping of a shared memory segment with the following cmd values:

       SHM_LOCK (Linux-specific)
                 Prevent swapping of the shared memory segment.  The caller must fault in any pages that are required to be present after locking is enabled.  If  a  segment  has
                 been locked, then the (nonstandard) SHM_LOCKED flag of the shm_perm.mode field in the associated data structure retrieved by IPC_STAT will be set.

       SHM_UNLOCK (Linux-specific)
                 Unlock the segment, allowing it to be swapped out.

       In  kernels before 2.6.10, only a privileged process could employ SHM_LOCK and SHM_UNLOCK.  Since kernel 2.6.10, an unprivileged process can employ these operations if its
       effective UID matches the owner or creator UID of the segment, and (for SHM_LOCK) the amount of memory to be locked falls within the  RLIMIT_MEMLOCK  resource  limit  (see
       setrlimit(2)).

RETURN VALUE
       A  successful  IPC_INFO or SHM_INFO operation returns the index of the highest used entry in the kernel's internal array recording information about all shared memory seg‐
       ments.  (This information can be used with repeated SHM_STAT or SHM_STAT_ANY operations to obtain information about all shared memory segments on the system.)  A  success‐
       ful SHM_STAT operation returns the identifier of the shared memory segment whose index was given in shmid.  Other operations return 0 on success.

       On error, -1 is returned, and errno is set appropriately.

ERRORS
       EACCES IPC_STAT  or SHM_STAT is requested and shm_perm.mode does not allow read access for shmid, and the calling process does not have the CAP_IPC_OWNER capability in the
              user namespace that governs its IPC namespace.

       EFAULT The argument cmd has value IPC_SET or IPC_STAT but the address pointed to by buf isn't accessible.

       EIDRM  shmid points to a removed identifier.

       EINVAL shmid is not a valid identifier, or cmd is not a valid command.  Or: for a SHM_STAT or SHM_STAT_ANY operation, the index value specified in shmid referred to an ar‐
              ray slot that is currently unused.

       ENOMEM (In kernels since 2.6.9), SHM_LOCK was specified and the size of the to-be-locked segment would mean that the total bytes in locked shared memory segments would ex‐
              ceed the limit for the real user ID of the calling process.  This limit is defined by the RLIMIT_MEMLOCK soft resource limit (see setrlimit(2)).

       EOVERFLOW
              IPC_STAT is attempted, and the GID or UID value is too large to be stored in the structure pointed to by buf.

       EPERM  IPC_SET or IPC_RMID is attempted, and the effective user ID of the calling process is not that of the creator (found in  shm_perm.cuid),  or  the  owner  (found  in
              shm_perm.uid), and the process was not privileged (Linux: did not have the CAP_SYS_ADMIN capability).

              Or  (in  kernels  before 2.6.9), SHM_LOCK or SHM_UNLOCK was specified, but the process was not privileged (Linux: did not have the CAP_IPC_LOCK capability).  (Since
              Linux 2.6.9, this error can also occur if the RLIMIT_MEMLOCK is 0 and the caller is not privileged.)

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4.

NOTES
       The inclusion of <sys/types.h> and <sys/ipc.h> isn't required on Linux or by any version of POSIX.  However, some old  implementations  required  the  inclusion  of  these
       header files, and the SVID also documented their inclusion.  Applications intended to be portable to such old systems may need to include these header files.

       The  IPC_INFO,  SHM_STAT and SHM_INFO operations are used by the ipcs(1) program to provide information on allocated resources.  In the future, these may modified or moved
       to a /proc filesystem interface.

       Linux permits a process to attach (shmat(2)) a shared memory segment that has already been marked for deletion using shmctl(IPC_RMID).  This feature is  not  available  on
       other UNIX implementations; portable applications should avoid relying on it.

       Various  fields  in  a  struct  shmid_ds  were  typed  as  short  under  Linux  2.2 and have become long under Linux 2.4.  To take advantage of this, a recompilation under
       glibc-2.1.91 or later should suffice.  (The kernel distinguishes old and new calls by an IPC_64 flag in cmd.)

SEE ALSO
       mlock(2), setrlimit(2), shmget(2), shmop(2), capabilities(7), sysvipc(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A description of the project, information about reporting bugs, and the latest version of this page, can
       be found at https://www.kernel.org/doc/man-pages/.

Linux                                                                               2019-08-02                                                                           SHMCTL(2)
```