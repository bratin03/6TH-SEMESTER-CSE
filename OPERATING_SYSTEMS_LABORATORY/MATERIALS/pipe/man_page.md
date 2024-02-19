### PIPE

```c
PIPE(2)                         Linux Programmer's Manual                        PIPE(2)

NAME
       pipe, pipe2 - create pipe

SYNOPSIS
       #include <unistd.h>

       /* On Alpha, IA-64, MIPS, SuperH, and SPARC/SPARC64; see NOTES */
       struct fd_pair {
           long fd[2];
       };
       struct fd_pair pipe();

       /* On all other architectures */
       int pipe(int pipefd[2]);

       #define _GNU_SOURCE             /* See feature_test_macros(7) */
       #include <fcntl.h>              /* Obtain O_* constant definitions */
       #include <unistd.h>

       int pipe2(int pipefd[2], int flags);

DESCRIPTION
       pipe()  creates a pipe, a unidirectional data channel that can be used for inter‐
       process communication.  The array pipefd is used to return two  file  descriptors
       referring to the ends of the pipe.  pipefd[0] refers to the read end of the pipe.
       pipefd[1] refers to the write end of the pipe.  Data written to the write end  of
       the  pipe  is  buffered  by  the kernel until it is read from the read end of the
       pipe.  For further details, see pipe(7).

       If flags is 0, then pipe2() is the same as pipe().  The following values  can  be
       bitwise ORed in flags to obtain different behavior:

       O_CLOEXEC
              Set  the  close-on-exec (FD_CLOEXEC) flag on the two new file descriptors.
              See the description of the same flag in open(2) for reasons why  this  may
              be useful.

       O_DIRECT (since Linux 3.4)
              Create  a  pipe  that performs I/O in "packet" mode.  Each write(2) to the
              pipe is dealt with as a separate packet, and read(2)s from the  pipe  will
              read one packet at a time.  Note the following points:

              *  Writes  of greater than PIPE_BUF bytes (see pipe(7)) will be split into
                 multiple packets.  The constant PIPE_BUF is defined in <limits.h>.

              *  If a read(2) specifies a buffer size that  is  smaller  than  the  next
                 packet,  then  the  requested  number of bytes are read, and the excess
                 bytes in the  packet  are  discarded.   Specifying  a  buffer  size  of
                 PIPE_BUF  will  be sufficient to read the largest possible packets (see
                 the previous point).

              *  Zero-length packets are not supported.  (A  read(2)  that  specifies  a
                 buffer size of zero is a no-op, and returns 0.)

              Older kernels that do not support this flag will indicate this via an EIN‐
              VAL error.

              Since Linux 4.5, it is possible to change the O_DIRECT setting of  a  pipe
              file descriptor using fcntl(2).

       O_NONBLOCK
              Set the O_NONBLOCK file status flag on the open file descriptions referred
              to by the new file descriptors.  Using this flag saves extra calls to  fc‐
              ntl(2) to achieve the same result.

RETURN VALUE
       On  success,  zero is returned.  On error, -1 is returned, errno is set appropri‐
       ately, and pipefd is left unchanged.

       On Linux (and other systems), pipe() does not modify pipefd on  failure.   A  re‐
       quirement  standardizing this behavior was added in POSIX.1-2016.  The Linux-spe‐
       cific pipe2() system call likewise does not modify pipefd on failure.

ERRORS
       EFAULT pipefd is not valid.

       EINVAL (pipe2()) Invalid value in flags.

       EMFILE The per-process limit on the number of  open  file  descriptors  has  been
              reached.

       ENFILE The system-wide limit on the total number of open files has been reached.

       ENFILE The  user  hard  limit  on memory that can be allocated for pipes has been
              reached and the caller is not privileged; see pipe(7).

VERSIONS
       pipe2() was added to Linux in version 2.6.27; glibc support is available starting
       with version 2.9.

NOTES
       The  SystemV  ABI  on some architectures allows the use of more than one register
       for returning multiple values; several architectures (namely, Alpha, IA-64, MIPS,
       SuperH,  and SPARC/SPARC64) (ab)use this feature in order to implement the pipe()
       system call in a functional manner: the call doesn't take any arguments  and  re‐
       turns  a  pair  of  file  descriptors  as the return value on success.  The glibc
       pipe() wrapper function transparently deals with this.  See syscall(2) for infor‐
       mation regarding registers used for storing second file descriptor.

CONFORMING TO
       pipe(): POSIX.1-2001, POSIX.1-2008.

       pipe2() is Linux-specific.

EXAMPLE
       The  following  program  creates  a  pipe,  and  then  fork(2)s to create a child
       process; the child inherits a duplicate set of file descriptors that refer to the
       same  pipe.   After the fork(2), each process closes the file descriptors that it
       doesn't need for the pipe (see pipe(7)).  The parent then writes the string  con‐
       tained  in  the  program's command-line argument to the pipe, and the child reads
       this string a byte at a time from the pipe and echoes it on standard output.

   Program source
       #include <sys/types.h>
       #include <sys/wait.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>
       #include <string.h>

       int
       main(int argc, char *argv[])
       {
           int pipefd[2];
           pid_t cpid;
           char buf;

           if (argc != 2) {
               fprintf(stderr, "Usage: %s <string>\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           if (pipe(pipefd) == -1) {
               perror("pipe");
               exit(EXIT_FAILURE);
           }

           cpid = fork();
           if (cpid == -1) {
               perror("fork");
               exit(EXIT_FAILURE);
           }

           if (cpid == 0) {    /* Child reads from pipe */
               close(pipefd[1]);          /* Close unused write end */

               while (read(pipefd[0], &buf, 1) > 0)
                   write(STDOUT_FILENO, &buf, 1);

               write(STDOUT_FILENO, "\n", 1);
               close(pipefd[0]);
               _exit(EXIT_SUCCESS);

           } else {            /* Parent writes argv[1] to pipe */
               close(pipefd[0]);          /* Close unused read end */
               write(pipefd[1], argv[1], strlen(argv[1]));
               close(pipefd[1]);          /* Reader will see EOF */
               wait(NULL);                /* Wait for child */
               exit(EXIT_SUCCESS);
           }
       }

SEE ALSO
       fork(2),  read(2),  socketpair(2),  splice(2),  tee(2),  vmsplice(2),   write(2),
       popen(3), pipe(7)

COLOPHON
       This  page is part of release 5.05 of the Linux man-pages project.  A description
       of the project, information about reporting bugs, and the latest version of  this
       page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                  2019-03-06                                PIPE(2)

```

### OPEN

```c
OPEN(2)                         Linux Programmer's Manual                        OPEN(2)

NAME
       open, openat, creat - open and possibly create a file

SYNOPSIS
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);

       int creat(const char *pathname, mode_t mode);

       int openat(int dirfd, const char *pathname, int flags);
       int openat(int dirfd, const char *pathname, int flags, mode_t mode);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       openat():
           Since glibc 2.10:
               _POSIX_C_SOURCE >= 200809L
           Before glibc 2.10:
               _ATFILE_SOURCE

DESCRIPTION
       The  open()  system  call opens the file specified by pathname.  If the specified
       file does not exist, it may optionally (if O_CREAT is specified in flags) be cre‐
       ated by open().

       The  return  value  of  open() is a file descriptor, a small, nonnegative integer
       that is used in subsequent system calls (read(2), write(2),  lseek(2),  fcntl(2),
       etc.)  to  refer  to the open file.  The file descriptor returned by a successful
       call will be the lowest-numbered file  descriptor  not  currently  open  for  the
       process.

       By  default,  the  new  file descriptor is set to remain open across an execve(2)
       (i.e., the FD_CLOEXEC file descriptor flag described  in  fcntl(2)  is  initially
       disabled);  the  O_CLOEXEC  flag, described below, can be used to change this de‐
       fault.  The file offset is set to the beginning of the file (see lseek(2)).

       A call to open() creates a new open file description, an entry in the system-wide
       table  of  open files.  /*The open file description records the file offset and the
       file status flags*/ (see below).  A file descriptor is a reference to an open  file
       description;  this reference is unaffected if pathname is subsequently removed or
       modified to refer to a different file.  For further details on open file descrip‐
       tions, see NOTES.

       /*The  argument  flags  must  include  one of the following access modes: O_RDONLY,
       O_WRONLY, or O_RDWR.  These request opening the file  read-only,  write-only,  or
       read/write, respectively.*/

       In  addition,  zero or more file creation flags and file status flags can be bit‐
       wise-or'd in flags.  The file creation flags are O_CLOEXEC, O_CREAT, O_DIRECTORY,
       O_EXCL,  O_NOCTTY, O_NOFOLLOW, O_TMPFILE, and O_TRUNC.  The file status flags are
       all of the remaining flags listed  below.   /*The  distinction  between  these  two
       groups  of flags is that the file creation flags affect the semantics of the open
       operation itself, while the file status flags affect the semantics of  subsequent
       I/O operations.*/  The file status flags can be retrieved and (in some cases) modi‐
       fied; see fcntl(2) for details.

       The full list of file creation flags and file status flags is as follows:

       O_APPEND
              /*The file is opened in append mode.*/  Before each write(2), the file  offset
              is  positioned at the end of the file, as if with lseek(2).  The modifica‐
              tion of the file offset and the write operation are performed as a  single
              atomic step.

              O_APPEND  may  lead to corrupted files on NFS filesystems if more than one
              process appends data to a file at once.  This is because NFS does not sup‐
              port  appending  to a file, so the client kernel has to simulate it, which
              can't be done without a race condition.

       O_ASYNC
              Enable signal-driven I/O: generate a signal (SIGIO by  default,  but  this
              can be changed via fcntl(2)) when input or output becomes possible on this
              file descriptor.  This feature is available only for terminals, pseudoter‐
              minals,  sockets, and (since Linux 2.6) pipes and FIFOs.  See fcntl(2) for
              further details.  See also BUGS, below.

       O_CLOEXEC (since Linux 2.6.23)
              Enable the close-on-exec flag for the  new  file  descriptor.   Specifying
              this  flag  permits  a program to avoid additional fcntl(2) F_SETFD opera‐
              tions to set the FD_CLOEXEC flag.

              Note that the use of this flag is essential  in  some  multithreaded  pro‐
              grams,  because  using  a  separate  fcntl(2) F_SETFD operation to set the
              FD_CLOEXEC flag does not suffice to avoid race conditions where one thread
              opens  a  file descriptor and attempts to set its close-on-exec flag using
              fcntl(2) at the same time as another thread does a fork(2) plus execve(2).
              Depending  on  the  order  of execution, the race may lead to the file de‐
              scriptor returned by open() being unintentionally leaked  to  the  program
              executed  by  the child process created by fork(2).  (This kind of race is
              in principle possible for any system call that creates a  file  descriptor
              whose  close-on-exec  flag  should  be set, and various other Linux system
              calls provide an equivalent of the O_CLOEXEC flag to deal with this  prob‐
              lem.)

       O_CREAT
              /*If pathname does not exist, create it as a regular file.*/

              The owner (user ID) of the new file is set to the effective user ID of the
              process.

              The group ownership (group ID) of the new file is set either to the effec‐
              tive  group  ID  of the process (System V semantics) or to the group ID of
              the parent directory (BSD semantics).  On Linux, the behavior  depends  on
              whether  the set-group-ID mode bit is set on the parent directory: if that
              bit is set, then BSD semantics apply; otherwise, System V semantics apply.
              For some filesystems, the behavior also depends on the bsdgroups and sysv‐
              groups mount options described in mount(8)).

              The mode argument specifies the file mode bits be applied when a new  file
              is  created.   This argument must be supplied when O_CREAT or O_TMPFILE is
              specified in flags; if neither O_CREAT nor O_TMPFILE  is  specified,  then
              mode is ignored.  The effective mode is modified by the process's umask in
              the usual way: in the absence of a default ACL, the mode  of  the  created
              file  is  (mode & ~umask).  /*Note that this mode applies only to future ac‐
              cesses of the newly created file; the open() call that creates a read-only
              file may well return a read/write file descriptor.*/

              The following symbolic constants are provided for mode:

              S_IRWXU  00700 user (file owner) has read, write, and execute permission

              S_IRUSR  00400 user has read permission

              S_IWUSR  00200 user has write permission

              S_IXUSR  00100 user has execute permission

              S_IRWXG  00070 group has read, write, and execute permission

              S_IRGRP  00040 group has read permission

              S_IWGRP  00020 group has write permission

              S_IXGRP  00010 group has execute permission

              S_IRWXO  00007 others have read, write, and execute permission

              S_IROTH  00004 others have read permission

              S_IWOTH  00002 others have write permission

              S_IXOTH  00001 others have execute permission

              According to POSIX, the effect when other bits are set in mode is unspeci‐
              fied.  On Linux, the following bits are also honored in mode:

              S_ISUID  0004000 set-user-ID bit

              S_ISGID  0002000 set-group-ID bit (see inode(7)).

              S_ISVTX  0001000 sticky bit (see inode(7)).

       O_DIRECT (since Linux 2.4.10)
              Try to minimize cache effects of the I/O to and from this file.   In  gen‐
              eral  this  will  degrade  performance, but it is useful in special situa‐
              tions, such as when applications do their own caching.  File I/O  is  done
              directly  to/from  user-space buffers.  The O_DIRECT flag on its own makes
              an effort to transfer data synchronously, but does not give the guarantees
              of  the  O_SYNC flag that data and necessary metadata are transferred.  To
              guarantee synchronous I/O, O_SYNC must be used in  addition  to  O_DIRECT.
              See NOTES below for further discussion.

              A semantically similar (but deprecated) interface for block devices is de‐
              scribed in raw(8).

       O_DIRECTORY
              If pathname is not a directory, cause the open to  fail.   This  flag  was
              added  in  kernel  version 2.1.126, to avoid denial-of-service problems if
              opendir(3) is called on a FIFO or tape device.

       O_DSYNC
              Write operations on the file will complete according to  the  requirements
              of synchronized I/O data integrity completion.

              By the time write(2) (and similar) return, the output data has been trans‐
              ferred to the underlying hardware, along with any file metadata that would
              be  required to retrieve that data (i.e., as though each write(2) was fol‐
              lowed by a call to fdatasync(2)).  See NOTES below.

       O_EXCL Ensure that this call creates the file: if this flag is specified in  con‐
              junction with O_CREAT, and pathname already exists, then open() fails with
              the error EEXIST.

              When these two flags are specified, symbolic links are  not  followed:  if
              pathname  is  a  symbolic  link, then open() fails regardless of where the
              symbolic link points.

              In general, the behavior of O_EXCL is undefined  if  it  is  used  without
              O_CREAT.   There  is  one exception: on Linux 2.6 and later, O_EXCL can be
              used without O_CREAT if pathname refers to a block device.  If  the  block
              device  is in use by the system (e.g., mounted), open() fails with the er‐
              ror EBUSY.

              On NFS, O_EXCL is supported only when using NFSv3 or later on  kernel  2.6
              or  later.  In NFS environments where O_EXCL support is not provided, pro‐
              grams that rely on it for performing locking tasks  will  contain  a  race
              condition.  Portable programs that want to perform atomic file locking us‐
              ing a lockfile, and need to avoid reliance on NFS support for O_EXCL,  can
              create  a unique file on the same filesystem (e.g., incorporating hostname
              and PID), and use link(2) to make a link to the lockfile.  If link(2)  re‐
              turns  0,  the  lock  is successful.  Otherwise, use stat(2) on the unique
              file to check if its link count has increased to 2, in which case the lock
              is also successful.

*** IMP O_LARGEFILE
              (LFS)  Allow  files whose sizes cannot be represented in an off_t (but can
              be represented in an off64_t) to be opened.  The _LARGEFILE64_SOURCE macro
              must  be  defined  (before  including any header files) in order to obtain
              this definition.  Setting the _FILE_OFFSET_BITS feature test macro  to  64
              (rather than using O_LARGEFILE) is the preferred method of accessing large
              files on 32-bit systems (see feature_test_macros(7)).

       O_NOATIME (since Linux 2.6.8)
              Do not update the file last access time (st_atime in the inode)  when  the
              file is read(2).

              This flag can be employed only if one of the following conditions is true:

              *  The effective UID of the process matches the owner UID of the file.

              *  The calling process has the CAP_FOWNER capability in its user namespace
                 and the owner UID of the file has a mapping in the namespace.

              This flag is intended for use by indexing or backup  programs,  where  its
              use  can  significantly reduce the amount of disk activity.  This flag may
              not be effective on all filesystems.  One example is NFS, where the server
              maintains the access time.

       O_NOCTTY
              If  pathname refers to a terminal device—see tty(4)—it will not become the
              process's controlling terminal even if the process does not have one.

       O_NOFOLLOW
              If pathname is a symbolic link, then the open fails, with the error ELOOP.
              Symbolic  links  in  earlier components of the pathname will still be fol‐
              lowed.  (Note that the ELOOP error that can occur in this case  is  indis‐
              tinguishable  from the case where an open fails because there are too many
              symbolic links found while resolving components in the prefix part of  the
              pathname.)

              This  flag  is  a  FreeBSD  extension, which was added to Linux in version
              2.1.126, and has subsequently been standardized in POSIX.1-2008.

              See also O_PATH below.

***IMP O_NONBLOCK or O_NDELAY
              When possible, the file is opened in nonblocking mode.  Neither the open()
              nor any subsequent I/O operations on the file descriptor which is returned
              will cause the calling process to wait.

              Note that the setting of this flag has  no  effect  on  the  operation  of
              poll(2),  select(2),  epoll(7), and similar, since those interfaces merely
              inform the caller about whether a file descriptor is "ready", meaning that
              an I/O operation performed on the file descriptor with the O_NONBLOCK flag
              clear would not block.

              Note that this flag has no effect for regular  files  and  block  devices;
              that  is,  I/O operations will (briefly) block when device activity is re‐
              quired, regardless of whether O_NONBLOCK is set.  Since O_NONBLOCK  seman‐
              tics  might eventually be implemented, applications should not depend upon
              blocking behavior when specifying this flag for regular  files  and  block
              devices.

              For  the handling of FIFOs (named pipes), see also fifo(7).  For a discus‐
              sion of the effect of O_NONBLOCK in conjunction with mandatory file  locks
              and with file leases, see fcntl(2).

       O_PATH (since Linux 2.6.39)
              Obtain  a file descriptor that can be used for two purposes: to indicate a
              location in the filesystem tree and to perform operations that act  purely
              at  the  file  descriptor level.  The file itself is not opened, and other
              file operations (e.g., read(2),  write(2),  fchmod(2),  fchown(2),  fgetx‐
              attr(2), ioctl(2), mmap(2)) fail with the error EBADF.

              The  following  operations can be performed on the resulting file descrip‐
              tor:

              *  close(2).

              *  fchdir(2), if the file descriptor refers to a  directory  (since  Linux
                 3.5).

              *  fstat(2) (since Linux 3.6).

              *  fstatfs(2) (since Linux 3.12).

              *  Duplicating the file descriptor (dup(2), fcntl(2) F_DUPFD, etc.).

              *  Getting  and  setting  file  descriptor  flags  (fcntl(2)  F_GETFD  and
                 F_SETFD).

              *  Retrieving open file status flags using the fcntl(2) F_GETFL operation:
                 the returned flags will include the bit O_PATH.

              *  Passing  the  file descriptor as the dirfd argument of openat() and the
                 other "*at()" system calls.  This includes linkat(2) with AT_EMPTY_PATH
                 (or  via  procfs using AT_SYMLINK_FOLLOW) even if the file is not a di‐
                 rectory.

              *  Passing the file descriptor to another process via a UNIX domain socket
                 (see SCM_RIGHTS in unix(7)).

              When  O_PATH  is specified in flags, flag bits other than O_CLOEXEC, O_DI‐
              RECTORY, and O_NOFOLLOW are ignored.

              Opening a file or directory with the O_PATH flag requires  no  permissions
              on  the object itself (but does require execute permission on the directo‐
              ries in the path prefix).  Depending on the subsequent operation, a  check
              for  suitable  file permissions may be performed (e.g., fchdir(2) requires
              execute permission on the directory referred to by its file descriptor ar‐
              gument).   By  contrast,  obtaining  a reference to a filesystem object by
              opening it with the O_RDONLY flag requires that the caller have read  per‐
              mission   on  the  object,  even  when  the  subsequent  operation  (e.g.,
              fchdir(2), fstat(2)) does not require read permission on the object.

              If pathname is a symbolic link and the O_NOFOLLOW flag is also  specified,
              then  the  call  returns a file descriptor referring to the symbolic link.
              This file descriptor can be used as the dirfd argument in calls to  fchow‐
              nat(2), fstatat(2), linkat(2), and readlinkat(2) with an empty pathname to
              have the calls operate on the symbolic link.

              If pathname refers to an automount point that has not yet been  triggered,
              so  no other filesystem is mounted on it, then the call returns a file de‐
              scriptor referring to the automount directory without triggering a  mount.
              fstatfs(2) can then be used to determine if it is, in fact, an untriggered
              automount point (.f_type == AUTOFS_SUPER_MAGIC).

              One use of O_PATH for regular  files  is  to  provide  the  equivalent  of
              POSIX.1's  O_EXEC functionality.  This permits us to open a file for which
              we have execute permission but not read permission, and then execute  that
              file, with steps something like the following:

                  char buf[PATH_MAX];
                  fd = open("some_prog", O_PATH);
                  snprintf(buf, PATH_MAX, "/proc/self/fd/%d", fd);
                  execl(buf, "some_prog", (char *) NULL);

              An  O_PATH  file  descriptor  can  also  be passed as the argument of fex‐
              ecve(3).

       O_SYNC Write operations on the file will complete according to  the  requirements
              of  synchronized  I/O file integrity completion (by contrast with the syn‐
              chronized I/O data integrity completion provided by O_DSYNC.)

              By the time write(2) (or similar) returns, the output data and  associated
              file  metadata  have been transferred to the underlying hardware (i.e., as
              though each write(2) was followed by a call to fsync(2)).  See  NOTES  be‐
              low.

       O_TMPFILE (since Linux 3.11)
              Create an unnamed temporary regular file.  The pathname argument specifies
              a directory; an unnamed inode will be created in that directory's filesys‐
              tem.   Anything  written  to the resulting file will be lost when the last
              file descriptor is closed, unless the file is given a name.

              O_TMPFILE must be specified with one of O_RDWR or  O_WRONLY  and,  option‐
              ally,  O_EXCL.   If O_EXCL is not specified, then linkat(2) can be used to
              link the temporary file into the filesystem, making  it  permanent,  using
              code like the following:

                  char path[PATH_MAX];
                  fd = open("/path/to/dir", O_TMPFILE | O_RDWR,
                                          S_IRUSR | S_IWUSR);

                  /* File I/O on 'fd'... */

                  linkat(fd, NULL, AT_FDCWD, "/path/for/file", AT_EMPTY_PATH);

                  /* If the caller doesn't have the CAP_DAC_READ_SEARCH
                     capability (needed to use AT_EMPTY_PATH with linkat(2)),
                     and there is a proc(5) filesystem mounted, then the
                     linkat(2) call above can be replaced with:

                  snprintf(path, PATH_MAX,  "/proc/self/fd/%d", fd);
                  linkat(AT_FDCWD, path, AT_FDCWD, "/path/for/file",
                                          AT_SYMLINK_FOLLOW);
                  */

              In  this  case,  the  open()  mode argument determines the file permission
              mode, as with O_CREAT.

              Specifying O_EXCL in conjunction with O_TMPFILE prevents a temporary  file
              from being linked into the filesystem in the above manner.  (Note that the
              meaning of O_EXCL in this case is different from  the  meaning  of  O_EXCL
              otherwise.)

              There are two main use cases for O_TMPFILE:

              *  Improved  tmpfile(3)  functionality:  race-free  creation  of temporary
                 files that (1) are automatically deleted when closed; (2) can never  be
                 reached  via  any pathname; (3) are not subject to symlink attacks; and
                 (4) do not require the caller to devise unique names.

              *  Creating a file that is initially invisible, which  is  then  populated
                 with  data  and  adjusted  to  have  appropriate  filesystem attributes
                 (fchown(2), fchmod(2), fsetxattr(2),  etc.)   before  being  atomically
                 linked  into the filesystem in a fully formed state (using linkat(2) as
                 described above).

              O_TMPFILE requires support by the underlying filesystem; only a subset  of
              Linux  filesystems  provide  that support.  In the initial implementation,
              support was provided in the  ext2,  ext3,  ext4,  UDF,  Minix,  and  shmem
              filesystems.  Support for other filesystems has subsequently been added as
              follows: XFS (Linux 3.15); Btrfs (Linux  3.16);  F2FS  (Linux  3.16);  and
              ubifs (Linux 4.9)

       O_TRUNC
              If  the  file already exists and is a regular file and the access mode al‐
              lows writing (i.e., is O_RDWR or O_WRONLY) it will be truncated to  length
              0.  If the file is a FIFO or terminal device file, the O_TRUNC flag is ig‐
              nored.  Otherwise, the effect of O_TRUNC is unspecified.

   creat()
       A  call  to  creat()  is  equivalent  to  calling  open()  with  flags  equal  to
       O_CREAT|O_WRONLY|O_TRUNC.

   openat()
       The  openat()  system call operates in exactly the same way as open(), except for
       the differences described here.

       If the pathname given in pathname is relative, then it is interpreted relative to
       the  directory  referred to by the file descriptor dirfd (rather than relative to
       the current working directory of the calling process, as is done by open() for  a
       relative pathname).

       If pathname is relative and dirfd is the special value AT_FDCWD, then pathname is
       interpreted relative to the current working  directory  of  the  calling  process
       (like open()).

       If pathname is absolute, then dirfd is ignored.

RETURN VALUE
       open(),  openat(),  and creat() return the new file descriptor, or -1 if an error
       occurred (in which case, errno is set appropriately).

ERRORS
       open(), openat(), and creat() can fail with the following errors:

       EACCES The requested access to the file is not allowed, or search  permission  is
              denied  for  one of the directories in the path prefix of pathname, or the
              file did not exist yet and write access to the parent directory is not al‐
              lowed.  (See also path_resolution(7).)

       EDQUOT Where  O_CREAT is specified, the file does not exist, and the user's quota
              of disk blocks or inodes on the filesystem has been exhausted.

       EEXIST pathname already exists and O_CREAT and O_EXCL were used.

       EFAULT pathname points outside your accessible address space.

       EFBIG  See EOVERFLOW.

       EINTR  While blocked waiting to complete an open of a slow device (e.g., a  FIFO;
              see fifo(7)), the call was interrupted by a signal handler; see signal(7).

       EINVAL The filesystem does not support the O_DIRECT flag.  See NOTES for more in‐
              formation.

       EINVAL Invalid value in flags.

       EINVAL O_TMPFILE was specified in flags, but  neither  O_WRONLY  nor  O_RDWR  was
              specified.

       EINVAL O_CREAT was specified in flags and the final component ("basename") of the
              new file's pathname is invalid (e.g., it contains characters not permitted
              by the underlying filesystem).

       EISDIR pathname  refers  to a directory and the access requested involved writing
              (that is, O_WRONLY or O_RDWR is set).

       EISDIR pathname refers to an existing directory, O_TMPFILE and one of O_WRONLY or
              O_RDWR  were  specified in flags, but this kernel version does not provide
              the O_TMPFILE functionality.

       ELOOP  Too many symbolic links were encountered in resolving pathname.

       ELOOP  pathname was a symbolic link,  and  flags  specified  O_NOFOLLOW  but  not
              O_PATH.

       EMFILE The  per-process  limit  on  the  number of open file descriptors has been
              reached (see the description of RLIMIT_NOFILE in getrlimit(2)).

       ENAMETOOLONG
              pathname was too long.

       ENFILE The system-wide limit on the total number of open files has been reached.

       ENODEV pathname refers to a device special file and no corresponding  device  ex‐
              ists.   (This  is  a Linux kernel bug; in this situation ENXIO must be re‐
              turned.)

       ENOENT O_CREAT is not set and the named file does not exist.

       ENOENT A directory component in pathname does not exist or is a dangling symbolic
              link.

       ENOENT pathname  refers to a nonexistent directory, O_TMPFILE and one of O_WRONLY
              or O_RDWR were specified in flags, but this kernel version does  not  pro‐
              vide the O_TMPFILE functionality.

       ENOMEM The  named  file  is a FIFO, but memory for the FIFO buffer can't be allo‐
              cated because the per-user hard limit on memory allocation for  pipes  has
              been reached and the caller is not privileged; see pipe(7).

       ENOMEM Insufficient kernel memory was available.

       ENOSPC pathname  was to be created but the device containing pathname has no room
              for the new file.

       ENOTDIR
              A component used as a directory in pathname is not, in fact, a  directory,
              or O_DIRECTORY was specified and pathname was not a directory.

       ENXIO  O_NONBLOCK | O_WRONLY is set, the named file is a FIFO, and no process has
              the FIFO open for reading.

       ENXIO  The file is a device special file and no corresponding device exists.

       ENXIO  The file is a UNIX domain socket.

       EOPNOTSUPP
              The filesystem containing pathname does not support O_TMPFILE.

       EOVERFLOW
              pathname refers to a regular file that is too large  to  be  opened.   The
              usual  scenario  here is that an application compiled on a 32-bit platform
              without -D_FILE_OFFSET_BITS=64 tried to open a  file  whose  size  exceeds
              (1<<31)-1  bytes; see also O_LARGEFILE above.  This is the error specified
              by POSIX.1; in kernels before 2.6.24, Linux gave the error EFBIG for  this
              case.

       EPERM  The  O_NOATIME flag was specified, but the effective user ID of the caller
              did not match the owner of the file and the caller was not privileged.

       EPERM  The operation was prevented by a file seal; see fcntl(2).

       EROFS  pathname refers to a file on a read-only filesystem and write  access  was
              requested.

       ETXTBSY
              pathname  refers  to an executable image which is currently being executed
              and write access was requested.

       ETXTBSY
              pathname refers to a file that is currently in use as a swap file, and the
              O_TRUNC flag was specified.

       ETXTBSY
              pathname refers to a file that is currently being read by the kernel (e.g.
              for module/firmware loading), and write access was requested.

       EWOULDBLOCK
              The O_NONBLOCK flag was specified, and an incompatible lease was  held  on
              the file (see fcntl(2)).

       The following additional errors can occur for openat():

       EBADF  dirfd is not a valid file descriptor.

       ENOTDIR
              pathname  is  a relative pathname and dirfd is a file descriptor referring
              to a file other than a directory.

VERSIONS
       openat() was added to Linux in kernel 2.6.16; library support was added to  glibc
       in version 2.4.

CONFORMING TO
       open(), creat() SVr4, 4.3BSD, POSIX.1-2001, POSIX.1-2008.

       openat(): POSIX.1-2008.

       The  O_DIRECT,  O_NOATIME,  O_PATH,  and O_TMPFILE flags are Linux-specific.  One
       must define _GNU_SOURCE to obtain their definitions.

       The  O_CLOEXEC,  O_DIRECTORY,  and  O_NOFOLLOW  flags  are   not   specified   in
       POSIX.1-2001,  but  are specified in POSIX.1-2008.  Since glibc 2.12, one can ob‐
       tain their definitions by defining either _POSIX_C_SOURCE with  a  value  greater
       than  or  equal to 200809L or _XOPEN_SOURCE with a value greater than or equal to
       700.  In glibc  2.11  and  earlier,  one  obtains  the  definitions  by  defining
       _GNU_SOURCE.

       As  noted in feature_test_macros(7), feature test macros such as _POSIX_C_SOURCE,
       _XOPEN_SOURCE, and _GNU_SOURCE must be defined before including any header files.

NOTES
       Under Linux, the O_NONBLOCK flag is sometimes used in cases where  one  wants  to
       open  but does not necessarily have the intention to read or write.  For example,
       this may be used to open a device in order to get a file descriptor for use  with
       ioctl(2).

       The  (undefined)  effect  of O_RDONLY | O_TRUNC varies among implementations.  On
       many systems the file is actually truncated.

       Note that open() can open device special files, but creat() cannot  create  them;
       use mknod(2) instead.

       If  the  file  is newly created, its st_atime, st_ctime, st_mtime fields (respec‐
       tively, time of last access, time of last status change, and time of last modifi‐
       cation;  see  stat(2))  are  set to the current time, and so are the st_ctime and
       st_mtime fields of the parent directory.  Otherwise, if the file is modified  be‐
       cause  of  the O_TRUNC flag, its st_ctime and st_mtime fields are set to the cur‐
       rent time.

       The files in the /proc/[pid]/fd directory show the open file descriptors  of  the
       process  with  the  PID  pid.  The files in the /proc/[pid]/fdinfo directory show
       even more information about these file descriptors.  See proc(5) for further  de‐
       tails of both of these directories.

       The  Linux  header  file  <asm/fcntl.h> doesn't define O_ASYNC; the (BSD-derived)
       FASYNC synonym is defined instead.

   Open file descriptions
       The term open file description is the one used by POSIX to refer to  the  entries
       in  the system-wide table of open files.  In other contexts, this object is vari‐
       ously also called an "open file object", a "file handle", an "open file table en‐
       try", or—in kernel-developer parlance—a struct file.

       When  a  file  descriptor  is duplicated (using dup(2) or similar), the duplicate
       refers to the same open file description as the original file descriptor, and the
       two  file  descriptors  consequently share the file offset and file status flags.
       Such sharing can also occur  between  processes:  a  child  process  created  via
       fork(2)  inherits  duplicates  of its parent's file descriptors, and those dupli‐
       cates refer to the same open file descriptions.

       Each open() of a file creates a new open file description;  thus,  there  may  be
       multiple open file descriptions corresponding to a file inode.

       On  Linux,  one  can use the kcmp(2) KCMP_FILE operation to test whether two file
       descriptors (in the same process or in two different processes) refer to the same
       open file description.

   Synchronized I/O
       The  POSIX.1-2008  "synchronized I/O" option specifies different variants of syn‐
       chronized I/O, and specifies the open() flags O_SYNC, O_DSYNC,  and  O_RSYNC  for
       controlling  the behavior.  Regardless of whether an implementation supports this
       option, it must at least support the use of O_SYNC for regular files.

       Linux implements O_SYNC and O_DSYNC,  but  not  O_RSYNC.   Somewhat  incorrectly,
       glibc  defines  O_RSYNC to have the same value as O_SYNC.  (O_RSYNC is defined in
       the Linux header file <asm/fcntl.h> on HP PA-RISC, but it is not used.)

       O_SYNC provides synchronized I/O file integrity completion, meaning write  opera‐
       tions  will  flush  data  and all associated metadata to the underlying hardware.
       O_DSYNC provides synchronized I/O data integrity completion, meaning write opera‐
       tions  will  flush  data to the underlying hardware, but will only flush metadata
       updates that are required to allow a subsequent read operation to  complete  suc‐
       cessfully.   Data  integrity  completion can reduce the number of disk operations
       that are required for applications that don't need the guarantees of file  integ‐
       rity completion.

       To  understand  the  difference between the two types of completion, consider two
       pieces of file metadata: the file last modification timestamp (st_mtime) and  the
       file  length.   All write operations will update the last file modification time‐
       stamp, but only writes that add data to the end of the file will change the  file
       length.  The last modification timestamp is not needed to ensure that a read com‐
       pletes successfully, but the file length is.  Thus, O_DSYNC would only  guarantee
       to  flush  updates  to the file length metadata (whereas O_SYNC would also always
       flush the last modification timestamp metadata).

       Before Linux 2.6.33, Linux implemented only the O_SYNC flag for open().  However,
       when  that  flag was specified, most filesystems actually provided the equivalent
       of synchronized I/O data integrity completion (i.e., O_SYNC was  actually  imple‐
       mented as the equivalent of O_DSYNC).

       Since  Linux 2.6.33, proper O_SYNC support is provided.  However, to ensure back‐
       ward binary compatibility, O_DSYNC was defined with the same value as the histor‐
       ical  O_SYNC,  and O_SYNC was defined as a new (two-bit) flag value that includes
       the O_DSYNC flag value.  This ensures  that  applications  compiled  against  new
       headers get at least O_DSYNC semantics on pre-2.6.33 kernels.

   C library/kernel differences
       Since  version  2.26,  the glibc wrapper function for open() employs the openat()
       system call, rather than the kernel's open() system call.  For certain  architec‐
       tures, this is also true in glibc versions before 2.26.

   NFS
       There  are  many  infelicities  in the protocol underlying NFS, affecting amongst
       others O_SYNC and O_NDELAY.

       On NFS filesystems with UID mapping enabled, open() may return a file  descriptor
       but,  for  example, read(2) requests are denied with EACCES.  This is because the
       client performs open() by checking the permissions, but UID mapping is  performed
       by the server upon read and write requests.

   FIFOs
       Opening the read or write end of a FIFO blocks until the other end is also opened
       (by another process or thread).  See fifo(7) for further details.

   File access mode
       Unlike the other values that can be specified in flags, the  access  mode  values
       O_RDONLY,  O_WRONLY, and O_RDWR do not specify individual bits.  Rather, they de‐
       fine the low order two bits of flags, and are defined respectively as 0,  1,  and
       2.   In  other words, the combination O_RDONLY | O_WRONLY is a logical error, and
       certainly does not have the same meaning as O_RDWR.

       Linux reserves the special, nonstandard access mode 3 (binary  11)  in  flags  to
       mean:  check for read and write permission on the file and return a file descrip‐
       tor that can't be used for reading or writing.  This nonstandard access  mode  is
       used  by  some  Linux drivers to return a file descriptor that is to be used only
       for device-specific ioctl(2) operations.

   Rationale for openat() and other directory file descriptor APIs
       openat() and the other system calls and library functions that take  a  directory
       file descriptor argument (i.e., execveat(2), faccessat(2), fanotify_mark(2), fch‐
       modat(2), fchownat(2), fstatat(2),  futimesat(2),  linkat(2),  mkdirat(2),  mkno‐
       dat(2), name_to_handle_at(2), readlinkat(2), renameat(2), statx(2), symlinkat(2),
       unlinkat(2), utimensat(2), mkfifoat(3), and scandirat(3))  address  two  problems
       with  the older interfaces that preceded them.  Here, the explanation is in terms
       of the openat() call, but the rationale is analogous for the other interfaces.

       First, openat() allows an application to avoid race conditions that  could  occur
       when using open() to open files in directories other than the current working di‐
       rectory.  These race conditions result from the fact that some component  of  the
       directory  prefix  given  to open() could be changed in parallel with the call to
       open().  Suppose, for example, that we wish to create the file  dir1/dir2/xxx.dep
       if  the  file  dir1/dir2/xxx  exists.   The problem is that between the existence
       check and the file-creation step, dir1 or dir2 (which might  be  symbolic  links)
       could be modified to point to a different location.  Such races can be avoided by
       opening a file descriptor for the target directory, and then specifying that file
       descriptor  as  the  dirfd argument of (say) fstatat(2) and openat().  The use of
       the dirfd file descriptor also has other benefits:

       *  the file descriptor is a stable reference to the directory, even if the direc‐
          tory is renamed; and

       *  the  open  file  descriptor prevents the underlying filesystem from being dis‐
          mounted, just as when a process has a current working directory on a  filesys‐
          tem.

       Second,  openat()  allows the implementation of a per-thread "current working di‐
       rectory", via file descriptor(s) maintained by the application.  (This  function‐
       ality can also be obtained by tricks based on the use of /proc/self/fd/dirfd, but
       less efficiently.)

   O_DIRECT
       The O_DIRECT flag may impose alignment restrictions on the length and address  of
       user-space  buffers and the file offset of I/Os.  In Linux alignment restrictions
       vary by filesystem and kernel version and  might  be  absent  entirely.   However
       there is currently no filesystem-independent interface for an application to dis‐
       cover these restrictions for a given file or filesystem.  Some  filesystems  pro‐
       vide their own interfaces for doing so, for example the XFS_IOC_DIOINFO operation
       in xfsctl(3).

       Under Linux 2.4, transfer sizes, and the alignment of the  user  buffer  and  the
       file  offset  must  all be multiples of the logical block size of the filesystem.
       Since Linux 2.6.0, alignment to the logical block size of the underlying  storage
       (typically  512  bytes) suffices.  The logical block size can be determined using
       the ioctl(2) BLKSSZGET operation or from the shell using the command:

           blockdev --getss

       O_DIRECT I/Os should never be run concurrently with the fork(2) system  call,  if
       the  memory  buffer  is  a  private  mapping  (i.e., any mapping created with the
       mmap(2) MAP_PRIVATE flag; this includes memory allocated on the heap  and  stati‐
       cally  allocated  buffers).  Any such I/Os, whether submitted via an asynchronous
       I/O interface or from another thread in the process, should be  completed  before
       fork(2)  is called.  Failure to do so can result in data corruption and undefined
       behavior in parent and child processes.  This restriction does not apply when the
       memory  buffer  for  the O_DIRECT I/Os was created using shmat(2) or mmap(2) with
       the MAP_SHARED flag.  Nor does this restriction apply when the memory buffer  has
       been  advised  as  MADV_DONTFORK  with  madvise(2),  ensuring that it will not be
       available to the child after fork(2).

       The O_DIRECT flag was introduced in SGI IRIX, where it has alignment restrictions
       similar  to those of Linux 2.4.  IRIX has also a fcntl(2) call to query appropri‐
       ate alignments, and sizes.  FreeBSD 4.x introduced a flag of the same  name,  but
       without alignment restrictions.

       O_DIRECT  support  was  added  under Linux in kernel version 2.4.10.  Older Linux
       kernels simply ignore this flag.  Some filesystems may not implement the flag, in
       which case open() fails with the error EINVAL if it is used.

       Applications  should  avoid  mixing O_DIRECT and normal I/O to the same file, and
       especially to overlapping byte regions in the same file.  Even when the  filesys‐
       tem  correctly  handles  the  coherency  issues  in  this  situation, overall I/O
       throughput is likely to be slower than using either mode alone.  Likewise, appli‐
       cations should avoid mixing mmap(2) of files with direct I/O to the same files.

       The behavior of O_DIRECT with NFS will differ from local filesystems.  Older ker‐
       nels, or kernels configured in certain ways, may not  support  this  combination.
       The NFS protocol does not support passing the flag to the server, so O_DIRECT I/O
       will bypass the page cache only on the client; the server  may  still  cache  the
       I/O.  The client asks the server to make the I/O synchronous to preserve the syn‐
       chronous semantics of O_DIRECT.  Some servers will  perform  poorly  under  these
       circumstances,  especially  if  the  I/O size is small.  Some servers may also be
       configured to lie to clients about the I/O having reached  stable  storage;  this
       will avoid the performance penalty at some risk to data integrity in the event of
       server power failure.  The Linux NFS client places no alignment  restrictions  on
       O_DIRECT I/O.

       In summary, O_DIRECT is a potentially powerful tool that should be used with cau‐
       tion.  It is recommended that applications treat use of O_DIRECT as a performance
       option which is disabled by default.

BUGS
       Currently,  it  is not possible to enable signal-driven I/O by specifying O_ASYNC
       when calling open(); use fcntl(2) to enable this flag.

       One must check for two different error codes, EISDIR and ENOENT, when  trying  to
       determine whether the kernel supports O_TMPFILE functionality.

       When  both  O_CREAT and O_DIRECTORY are specified in flags and the file specified
       by pathname does not exist, open() will create a regular file (i.e.,  O_DIRECTORY
       is ignored).

SEE ALSO
       chmod(2),  chown(2),  close(2),  dup(2),  fcntl(2),  link(2), lseek(2), mknod(2),
       mmap(2), mount(2), open_by_handle_at(2), read(2), socket(2),  stat(2),  umask(2),
       unlink(2),  write(2),  fopen(3),  acl(5),  fifo(7), inode(7), path_resolution(7),
       symlink(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description
       of  the project, information about reporting bugs, and the latest version of this
       page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                  2020-02-09                                OPEN(2)
```


### READ

```c
READ(2)                         Linux Programmer's Manual                        READ(2)

NAME
       read - read from a file descriptor

SYNOPSIS
       #include <unistd.h>

       ssize_t read(int fd, void *buf, size_t count);

DESCRIPTION
       read() attempts to read up to count bytes from file descriptor fd into the buffer
       starting at buf.

       On files that support seeking, the read operation commences at the  file  offset,
       and the file offset is incremented by the number of bytes read.  If the file off‐
       set is at or past the end of file, no bytes are read, and read() returns zero.

       If count is zero, read() may detect the errors described below.  In  the  absence
       of any errors, or if read() does not check for errors, a read() with a count of 0
       returns zero and has no other effects.

       According to POSIX.1, if count is greater than SSIZE_MAX, the result is implemen‐
       tation-defined; see NOTES for the upper limit on Linux.

RETURN VALUE
       On  success,  the  number of bytes read is returned (zero indicates end of file),
       and the file position is advanced by this number.  It is not  an  error  if  this
       number is smaller than the number of bytes requested; this may happen for example
       because fewer bytes are actually available right now (maybe because we were close
       to  end-of-file,  or  because we are reading from a pipe, or from a terminal), or
       because read() was interrupted by a signal.  See also NOTES.

       On error, -1 is returned, and errno is set appropriately.  In this  case,  it  is
       left unspecified whether the file position (if any) changes.

ERRORS
       EAGAIN The  file  descriptor fd refers to a file other than a socket and has been
              marked nonblocking (O_NONBLOCK), and the read would  block.   See  open(2)
              for further details on the O_NONBLOCK flag.

       EAGAIN or EWOULDBLOCK
              The  file descriptor fd refers to a socket and has been marked nonblocking
              (O_NONBLOCK), and the read would block.  POSIX.1-2001 allows either  error
              to be returned for this case, and does not require these constants to have
              the same value, so a portable application should check for both possibili‐
              ties.

       EBADF  fd is not a valid file descriptor or is not open for reading.

       EFAULT buf is outside your accessible address space.

       EINTR  The  call  was  interrupted by a signal before any data was read; see sig‐
              nal(7).

       EINVAL fd is attached to an object which is unsuitable for reading; or  the  file
              was  opened  with  the  O_DIRECT flag, and either the address specified in
              buf, the value specified in count, or the  file  offset  is  not  suitably
              aligned.

       EINVAL fd  was  created via a call to timerfd_create(2) and the wrong size buffer
              was given to read(); see timerfd_create(2) for further information.

       EIO    I/O error.  This will happen for example when the process is  in  a  back‐
              ground process group, tries to read from its controlling terminal, and ei‐
              ther it is ignoring or blocking SIGTTIN or its process group is  orphaned.
              It may also occur when there is a low-level I/O error while reading from a
              disk or tape.  A further possible cause of EIO on networked filesystems is
              when  an  advisory lock had been taken out on the file descriptor and this
              lock has been lost.  See the Lost locks section of  fcntl(2)  for  further
              details.

       EISDIR fd refers to a directory.

       Other errors may occur, depending on the object connected to fd.

CONFORMING TO
       SVr4, 4.3BSD, POSIX.1-2001.

NOTES
       The  types size_t and ssize_t are, respectively, unsigned and signed integer data
       types specified by POSIX.1.

       On Linux, read() (and similar system calls)  will  transfer  at  most  0x7ffff000
       (2,147,479,552) bytes, returning the number of bytes actually transferred.  (This
       is true on both 32-bit and 64-bit systems.)

       On NFS filesystems, reading small amounts of data will update the timestamp  only
       the  first  time,  subsequent calls may not do so.  This is caused by client side
       attribute caching, because most if not all NFS clients leave st_atime (last  file
       access  time)  updates  to  the  server, and client side reads satisfied from the
       client's cache will not cause st_atime updates on the  server  as  there  are  no
       server-side  reads.   UNIX semantics can be obtained by disabling client-side at‐
       tribute caching, but in most situations this will substantially  increase  server
       load and decrease performance.

BUGS
       According to POSIX.1-2008/SUSv4 Section XSI 2.9.7 ("Thread Interactions with Reg‐
       ular File Operations"):

           All of the following functions shall be atomic with respect to each other  in
           the  effects  specified in POSIX.1-2008 when they operate on regular files or
           symbolic links: ...

       Among the APIs subsequently listed are read() and readv(2).  And  among  the  ef‐
       fects  that  should  be  atomic across threads (and processes) are updates of the
       file offset.  However, on Linux before version 3.14, this was not  the  case:  if
       two  processes that share an open file description (see open(2)) perform a read()
       (or readv(2)) at the same time, then the I/O operations were not atomic with  re‐
       spect  updating  the  file offset, with the result that the reads in the two pro‐
       cesses might (incorrectly) overlap in the blocks  of  data  that  they  obtained.
       This problem was fixed in Linux 3.14.

SEE ALSO
       close(2),  fcntl(2),  ioctl(2),  lseek(2),  open(2),  pread(2), readdir(2), read‐
       link(2), readv(2), select(2), write(2), fread(3)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description
       of  the project, information about reporting bugs, and the latest version of this
       page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                  2018-02-02                                READ(2)
```

### WRITE

```c
WRITE(2)                          Linux Programmer's Manual                         WRITE(2)

NAME
       write - write to a file descriptor

SYNOPSIS
       #include <unistd.h>

       ssize_t write(int fd, const void *buf, size_t count);

DESCRIPTION
       write() writes up to count bytes from the buffer starting at buf to the file referred
       to by the file descriptor fd.

       The number of bytes written may be less than count if, for example, there is insuffi‐
       cient  space on the underlying physical medium, or the RLIMIT_FSIZE resource limit is
       encountered (see setrlimit(2)), or the call was interrupted by a signal handler after
       having written less than count bytes.  (See also pipe(7).)

       For a seekable file (i.e., one to which lseek(2) may be applied, for example, a regu‐
       lar file) writing takes place at the file offset, and the file offset is  incremented
       by  the  number  of bytes actually written.  If the file was open(2)ed with O_APPEND,
       the file offset is first set to the end of the file before writing.   The  adjustment
       of the file offset and the write operation are performed as an atomic step.

       POSIX  requires  that  a  read(2) that can be proved to occur after a write() has re‐
       turned will return the new data.  Note that not all filesystems are POSIX conforming.

       According to POSIX.1, if count is greater than SSIZE_MAX, the result  is  implementa‐
       tion-defined; see NOTES for the upper limit on Linux.

RETURN VALUE
       On  success,  the number of bytes written is returned.  On error, -1 is returned, and
       errno is set to indicate the cause of the error.

       Note that a successful write() may transfer fewer than  count  bytes.   Such  partial
       writes  can  occur  for  various reasons; for example, because there was insufficient
       space on the disk device to write all of the requested bytes, or  because  a  blocked
       write()  to  a  socket, pipe, or similar was interrupted by a signal handler after it
       had transferred some, but before it had transferred all of the requested  bytes.   In
       the  event  of  a partial write, the caller can make another write() call to transfer
       the remaining bytes.  The subsequent call will either transfer further bytes  or  may
       result in an error (e.g., if the disk is now full).

       If  count  is zero and fd refers to a regular file, then write() may return a failure
       status if one of the errors below is detected.  If no errors are detected,  or  error
       detection  is not performed, 0 will be returned without causing any other effect.  If
       count is zero and fd refers to a file other than a regular file, the results are  not
       specified.

ERRORS
       EAGAIN The  file  descriptor  fd  refers  to  a file other than a socket and has been
              marked nonblocking (O_NONBLOCK), and the write would block.  See  open(2)  for
              further details on the O_NONBLOCK flag.

       EAGAIN or EWOULDBLOCK
              The  file  descriptor  fd  refers  to a socket and has been marked nonblocking
              (O_NONBLOCK), and the write would block.  POSIX.1-2001 allows either error  to
              be  returned  for  this case, and does not require these constants to have the
              same value, so a portable application should check for both possibilities.

       EBADF  fd is not a valid file descriptor or is not open for writing.

       EDESTADDRREQ
              fd refers to a datagram socket for which a peer address has not been set using
              connect(2).

       EDQUOT The user's quota of disk blocks on the filesystem containing the file referred
              to by fd has been exhausted.

       EFAULT buf is outside your accessible address space.

       EFBIG  An attempt was made to write a file that  exceeds  the  implementation-defined
              maximum  file size or the process's file size limit, or to write at a position
              past the maximum allowed offset.

       EINTR  The call was interrupted by a signal before any data  was  written;  see  sig‐
              nal(7).

       EINVAL fd  is  attached to an object which is unsuitable for writing; or the file was
              opened with the O_DIRECT flag, and either the address specified  in  buf,  the
              value specified in count, or the file offset is not suitably aligned.

       EIO    A  low-level I/O error occurred while modifying the inode.  This error may re‐
              late to the write-back of data written by an earlier write(), which  may  have
              been  issued  to  a  different  file descriptor on the same file.  Since Linux
              4.13, errors from write-back come with a promise that they may be reported  by
              subsequent.   write()  requests, and will be reported by a subsequent fsync(2)
              (whether or not they were also reported by write()).  An  alternate  cause  of
              EIO  on  networked  filesystems is when an advisory lock had been taken out on
              the file descriptor and this lock has been lost.  See the Lost  locks  section
              of fcntl(2) for further details.

       ENOSPC The device containing the file referred to by fd has no room for the data.

       EPERM  The operation was prevented by a file seal; see fcntl(2).

       EPIPE  fd  is  connected  to a pipe or socket whose reading end is closed.  When this
              happens the writing process will also receive a SIGPIPE  signal.   (Thus,  the
              write return value is seen only if the program catches, blocks or ignores this
              signal.)

       Other errors may occur, depending on the object connected to fd.

CONFORMING TO
       SVr4, 4.3BSD, POSIX.1-2001.

       Under SVr4 a write may be interrupted and return EINTR at any point, not just  before
       any data is written.

NOTES
       The  types  size_t  and  ssize_t  are, respectively, unsigned and signed integer data
       types specified by POSIX.1.

       A successful return from write() does not make any guarantee that data has been  com‐
       mitted  to disk.  On some filesystems, including NFS, it does not even guarantee that
       space has successfully been reserved for the data.  In this case, some  errors  might
       be  delayed  until  a future write(), fsync(2), or even close(2).  The only way to be
       sure is to call fsync(2) after you are done writing all your data.

       If a write() is interrupted by a signal handler before any bytes  are  written,  then
       the call fails with the error EINTR; if it is interrupted after at least one byte has
       been written, the call succeeds, and returns the number of bytes written.

       On Linux, write() (and  similar  system  calls)  will  transfer  at  most  0x7ffff000
       (2,147,479,552)  bytes, returning the number of bytes actually transferred.  (This is
       true on both 32-bit and 64-bit systems.)

       An error return value while performing write() using direct I/O does not mean the en‐
       tire write has failed. Partial data may be written and the data at the file offset on
       which the write() was attempted should be considered inconsistent.

BUGS
       According to POSIX.1-2008/SUSv4 Section XSI 2.9.7 ("Thread Interactions with  Regular
       File Operations"):

           All  of the following functions shall be atomic with respect to each other in the
           effects specified in POSIX.1-2008 when they operate on regular files or  symbolic
           links: ...

       Among  the APIs subsequently listed are write() and writev(2).  And among the effects
       that should be atomic across threads (and processes) are updates of the file  offset.
       However,  on  Linux before version 3.14, this was not the case: if two processes that
       share an open file description (see open(2)) perform a write() (or writev(2)) at  the
       same  time,  then  the  I/O operations were not atomic with respect updating the file
       offset, with the result that the blocks of data output by  the  two  processes  might
       (incorrectly) overlap.  This problem was fixed in Linux 3.14.

SEE ALSO
       close(2),  fcntl(2),  fsync(2),  ioctl(2), lseek(2), open(2), pwrite(2), read(2), se‐
       lect(2), writev(2), fwrite(3)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description  of
       the  project,  information about reporting bugs, and the latest version of this page,
       can be found at https://www.kernel.org/doc/man-pages/.

Linux                                    2019-10-10                                 WRITE(2)
```

### CLOSE

```c
CLOSE(2)                          Linux Programmer's Manual                         CLOSE(2)

NAME
       close - close a file descriptor

SYNOPSIS
       #include <unistd.h>

       int close(int fd);

DESCRIPTION
       close()  closes a file descriptor, so that it no longer refers to any file and may be
       reused.  Any record locks (see fcntl(2)) held on the file it was associated with, and
       owned by the process, are removed (regardless of the file descriptor that was used to
       obtain the lock).

       If fd is the last file descriptor referring to the underlying open  file  description
       (see  open(2)), the resources associated with the open file description are freed; if
       the file descriptor was the last reference to a file which has been removed using un‐
       link(2), the file is deleted.

RETURN VALUE
       close()  returns  zero on success.  On error, -1 is returned, and errno is set appro‐
       priately.

ERRORS
       EBADF  fd isn't a valid open file descriptor.

       EINTR  The close() call was interrupted by a signal; see signal(7).

       EIO    An I/O error occurred.

       ENOSPC, EDQUOT
              On NFS, these errors are not normally reported against the first  write  which
              exceeds  the  available  storage  space,  but  instead  against  a  subsequent
              write(2), fsync(2), or close().

       See NOTES for a discussion of why close() should not be retried after an error.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.

NOTES
       A successful close does not guarantee that the data has been  successfully  saved  to
       disk, as the kernel uses the buffer cache to defer writes.  Typically, filesystems do
       not flush buffers when a file is closed.  If you need to be sure  that  the  data  is
       physically  stored on the underlying disk, use fsync(2).  (It will depend on the disk
       hardware at this point.)

       The close-on-exec file descriptor flag can be used to ensure that a  file  descriptor
       is automatically closed upon a successful execve(2); see fcntl(2) for details.

       It  is  probably  unwise to close file descriptors while they may be in use by system
       calls in other threads in the same process.  Since a file descriptor may  be  reused,
       there are some obscure race conditions that may cause unintended side effects.

   Dealing with error returns from close()
       A careful programmer will check the return value of close(), since it is quite possi‐
       ble that errors on a previous write(2) operation  are  reported  only  on  the  final
       close()  that  releases the open file description.  Failing to check the return value
       when closing a file may lead to silent loss of data.  This can especially be observed
       with NFS and with disk quota.

       Note,  however,  that  a  failure  return should be used only for diagnostic purposes
       (i.e., a warning to the application that there may still be I/O pending or there  may
       have  been failed I/O) or remedial purposes (e.g., writing the file once more or cre‐
       ating a backup).

       Retrying the close() after a failure return is the wrong thing to do, since this  may
       cause  a reused file descriptor from another thread to be closed.  This can occur be‐
       cause the Linux kernel always releases the file descriptor early in the close  opera‐
       tion, freeing it for reuse; the steps that may return an error, such as flushing data
       to the filesystem or device, occur only later in the close operation.

       Many other implementations similarly always close the file descriptor (except in  the
       case  of  EBADF,  meaning  that  the file descriptor was invalid) even if they subse‐
       quently report an error on return from close().  POSIX.1 is currently silent on  this
       point,  but there are plans to mandate this behavior in the next major release of the
       standard.

       A careful programmer who wants to know about I/O errors may precede  close()  with  a
       call to fsync(2).

       The  EINTR error is a somewhat special case.  Regarding the EINTR error, POSIX.1-2013
       says:

              If close() is interrupted by a signal that is to be caught, it shall return -1
              with errno set to EINTR and the state of fildes is unspecified.

       This permits the behavior that occurs on Linux and many other implementations, where,
       as with other errors that may be reported by close(), the file descriptor is  guaran‐
       teed  to be closed.  However, it also permits another possibility: that the implemen‐
       tation returns an EINTR error and keeps the file descriptor open.  (According to  its
       documentation,  HP-UX's  close()  does  this.)   The  caller  must then once more use
       close() to close the file descriptor, to avoid file descriptor  leaks.   This  diver‐
       gence  in  implementation behaviors provides a difficult hurdle for portable applica‐
       tions, since on many implementations, close() must not be called again after an EINTR
       error, and on at least one, close() must be called again.  There are plans to address
       this conundrum for the next major release of the POSIX.1 standard.

SEE ALSO
       fcntl(2), fsync(2), open(2), shutdown(2), unlink(2), fclose(3)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description  of
       the  project,  information about reporting bugs, and the latest version of this page,
       can be found at https://www.kernel.org/doc/man-pages/.

Linux                                    2019-10-10                                 CLOSE(2)

```

### DUP

```c
DUP(2)                            Linux Programmer's Manual                           DUP(2)

NAME
       dup, dup2, dup3 - duplicate a file descriptor

SYNOPSIS
       #include <unistd.h>

       int dup(int oldfd);
       int dup2(int oldfd, int newfd);

       #define _GNU_SOURCE             /* See feature_test_macros(7) */
       #include <fcntl.h>              /* Obtain O_* constant definitions */
       #include <unistd.h>

       int dup3(int oldfd, int newfd, int flags);

DESCRIPTION
       The  dup() system call creates a copy of the file descriptor oldfd, using the lowest-
       numbered unused file descriptor for the new descriptor.

       After a successful return, the old and new file descriptors may be used  interchange‐
       ably.  They refer to the same open file description (see open(2)) and thus share file
       offset and file status flags; for example, if the file offset is  modified  by  using
       lseek(2) on one of the file descriptors, the offset is also changed for the other.

       The two file descriptors do not share file descriptor flags (the close-on-exec flag).
       The close-on-exec flag (FD_CLOEXEC; see fcntl(2)) for  the  duplicate  descriptor  is
       off.

   dup2()
       The dup2() system call performs the same task as dup(), but instead of using the low‐
       est-numbered unused file descriptor, it uses the file descriptor number specified  in
       newfd.   If  the file descriptor newfd was previously open, it is silently closed be‐
       fore being reused.

       The steps of closing and reusing the file descriptor newfd are performed  atomically.
       This  is  important,  because  trying  to  implement  equivalent  functionality using
       close(2) and dup() would be subject to race conditions, whereby newfd might be reused
       between  the  two  steps.  Such reuse could happen because the main program is inter‐
       rupted by a signal handler that allocates a file descriptor, or  because  a  parallel
       thread allocates a file descriptor.

       Note the following points:

       *  If  oldfd  is  not  a valid file descriptor, then the call fails, and newfd is not
          closed.

       *  If oldfd is a valid file descriptor, and newfd has the same value as  oldfd,  then
          dup2() does nothing, and returns newfd.

   dup3()
       dup3() is the same as dup2(), except that:

       *  The  caller can force the close-on-exec flag to be set for the new file descriptor
          by specifying O_CLOEXEC in flags.  See the description of the same flag in open(2)
          for reasons why this may be useful.

       *  If oldfd equals newfd, then dup3() fails with the error EINVAL.

RETURN VALUE
       On  success,  these system calls return the new file descriptor.  On error, -1 is re‐
       turned, and errno is set appropriately.

ERRORS
       EBADF  oldfd isn't an open file descriptor.

       EBADF  newfd is out of the allowed range for file descriptors (see the discussion  of
              RLIMIT_NOFILE in getrlimit(2)).

       EBUSY  (Linux  only) This may be returned by dup2() or dup3() during a race condition
              with open(2) and dup().

       EINTR  The dup2() or dup3() call was interrupted by a signal; see signal(7).

       EINVAL (dup3()) flags contain an invalid value.

       EINVAL (dup3()) oldfd was equal to newfd.

       EMFILE The per-process limit on the number of open file descriptors has been  reached
              (see the discussion of RLIMIT_NOFILE in getrlimit(2)).

VERSIONS
       dup3() was added to Linux in version 2.6.27; glibc support is available starting with
       version 2.9.

CONFORMING TO
       dup(), dup2(): POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.

       dup3() is Linux-specific.

NOTES
       The error returned by dup2() is different from that returned by  fcntl(...,  F_DUPFD,
       ...)   when  newfd  is  out of range.  On some systems, dup2() also sometimes returns
       EINVAL like F_DUPFD.

       If newfd was open, any errors that would have been  reported  at  close(2)  time  are
       lost.  If this is of concern, then—unless the program is single-threaded and does not
       allocate file descriptors in signal handlers—the correct approach  is  not  to  close
       newfd before calling dup2(), because of the race condition described above.  Instead,
       code something like the following could be used:

           /* Obtain a duplicate of 'newfd' that can subsequently
              be used to check for close() errors; an EBADF error
              means that 'newfd' was not open. */

           tmpfd = dup(newfd);
           if (tmpfd == -1 && errno != EBADF) {
               /* Handle unexpected dup() error */
           }

           /* Atomically duplicate 'oldfd' on 'newfd' */

           if (dup2(oldfd, newfd) == -1) {
               /* Handle dup2() error */
           }

           /* Now check for close() errors on the file originally
              referred to by 'newfd' */

           if (tmpfd != -1) {
               if (close(tmpfd) == -1) {
                   /* Handle errors from close */
               }
           }

SEE ALSO
       close(2), fcntl(2), open(2)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.  A  description  of
       the  project,  information about reporting bugs, and the latest version of this page,
       can be found at https://www.kernel.org/doc/man-pages/.

Linux                                    2017-09-15                                   DUP(2)
```

### POPEN

```c
POPEN(3)                          Linux Programmer's Manual                         POPEN(3)

NAME
       popen, pclose - pipe stream to or from a process

SYNOPSIS
       #include <stdio.h>

       FILE *popen(const char *command, const char *type);

       int pclose(FILE *stream);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       popen(), pclose():
           _POSIX_C_SOURCE >= 2
               || /* Glibc versions <= 2.19: */ _BSD_SOURCE || _SVID_SOURCE

DESCRIPTION
       The  popen()  function  opens a process by creating a pipe, forking, and invoking the
       shell.  Since a pipe is by definition unidirectional, the type argument  may  specify
       only  reading or writing, not both; the resulting stream is correspondingly read-only
       or write-only.

       The command argument is a pointer to a null-terminated string containing a shell com‐
       mand  line.   This command is passed to /bin/sh using the -c flag; interpretation, if
       any, is performed by the shell.

       The type argument is a pointer to a null-terminated string which must contain  either
       the  letter 'r' for reading or the letter 'w' for writing.  Since glibc 2.9, this ar‐
       gument can additionally include the letter 'e', which causes the  close-on-exec  flag
       (FD_CLOEXEC)  to be set on the underlying file descriptor; see the description of the
       O_CLOEXEC flag in open(2) for reasons why this may be useful.

       The return value from popen() is a normal standard I/O stream in  all  respects  save
       that it must be closed with pclose() rather than fclose(3).  Writing to such a stream
       writes to the standard input of the command; the command's  standard  output  is  the
       same  as  that of the process that called popen(), unless this is altered by the com‐
       mand itself.  Conversely, reading from the stream reads the command's  standard  out‐
       put,  and the command's standard input is the same as that of the process that called
       popen().

       Note that output popen() streams are block buffered by default.

       The pclose() function waits for the associated process to terminate and  returns  the
       exit status of the command as returned by wait4(2).

RETURN VALUE
       popen():  on success, returns a pointer to an open stream that can be used to read or
       write to the pipe; if the fork(2) or pipe(2) calls fail, or if  the  function  cannot
       allocate memory, NULL is returned.

       pclose():  on success, returns the exit status of the command; if wait4(2) returns an
       error, or some other error is detected, -1 is returned.

       Both functions set errno to an appropriate value in the case of an error.

ERRORS
       The popen() function does not set errno if memory allocation fails.  If the  underly‐
       ing  fork(2)  or  pipe(2) fails, errno is set appropriately.  If the type argument is
       invalid, and this condition is detected, errno is set to EINVAL.

       If pclose() cannot obtain the child status, errno is set to ECHILD.

ATTRIBUTES
       For an explanation of the terms used in this section, see attributes(7).

       ┌──────────────────┬───────────────┬─────────┐
       │Interface         │ Attribute     │ Value   │
       ├──────────────────┼───────────────┼─────────┤
       │popen(), pclose() │ Thread safety │ MT-Safe │
       └──────────────────┴───────────────┴─────────┘

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008.

       The 'e' value for type is a Linux extension.

NOTES
       Note: carefully read Caveats in system(3).

BUGS
       Since the standard input of a command opened for reading shares its seek offset  with
       the  process  that  called popen(), if the original process has done a buffered read,
       the command's input position may not be as expected.  Similarly, the  output  from  a
       command opened for writing may become intermingled with that of the original process.
       The latter can be avoided by calling fflush(3) before popen().

       Failure to execute the shell is indistinguishable from the shell's failure to execute
       command,  or  an  immediate  exit of the command.  The only hint is an exit status of
       127.

SEE ALSO
       sh(1), fork(2), pipe(2), wait4(2), fclose(3),  fflush(3),  fopen(3),  stdio(3),  sys‐
       tem(3)

COLOPHON
       This  page  is part of release 5.05 of the Linux man-pages project.  A description of
       the project, information about reporting bugs, and the latest version of  this  page,
       can be found at https://www.kernel.org/doc/man-pages/.

GNU                                      2017-09-15                                 POPEN(3)
```