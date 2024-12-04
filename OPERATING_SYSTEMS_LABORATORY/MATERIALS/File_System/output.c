#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *current_working_directory = getcwd(NULL, 0);
    printf("Current working directory: %s\n", current_working_directory);
}

/* Output of the C code:
Current working directory: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char buffer[1024];
    char *current_working_directory = getcwd(buffer, sizeof(buffer));
    printf("Current working directory: %s\n", current_working_directory);
    printf("Buffer: %s\n", buffer);
}
/* Output of the C code:
Current working directory: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System
Buffer: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

int main(int argc, char *argv[])
{
    char buffer[PATH_MAX];
    char *current_working_directory = getwd(buffer);
    printf("Current working directory: %s\n", current_working_directory);
}
/* Output of the C code:
Current working directory: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

int main(int argc, char *argv[])
{
    char *current_working_directory = get_current_dir_name();
    if (current_working_directory == NULL)
    {
        perror("get_current_dir_name");
        exit(EXIT_FAILURE);
    }
    printf("Current working directory: %s\n", current_working_directory);
}

// Segmentation fault (core dumped)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

#define TARGET_DIR "/home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS"

int main(int argc, char *argv[])
{
    char *buffer = (char *)malloc(sizeof(char) * strlen(TARGET_DIR));
    int ret = chdir(TARGET_DIR);
    if (ret == -1)
    {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    char *current_working_directory = getcwd(NULL, 0);
    printf("Current working directory: %s\n", current_working_directory);
}
/* Output of the C code:
Current working directory: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/limits.h>

#define TARGET_FILE "/home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System"

int main(int argc, char *argv[])
{
    int fd = open(TARGET_FILE, O_RDONLY);
    int ret = fchdir(fd);
    if (ret == -1)
    {
        perror("fchdir");
        exit(EXIT_FAILURE);
    }
    char *current_working_directory = getcwd(NULL, 0);
    if (current_working_directory == NULL)
    {
        perror("get_current_dir_name");
        exit(EXIT_FAILURE);
    }
    printf("Current working directory: %s\n", current_working_directory);
    free(current_working_directory);
}
/* Output of the C code:
Current working directory: /home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS/File_System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>

#define TARGET_DIR "/home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS"

int main(int argc, char *argv[])
{
    DIR *dir = opendir(TARGET_DIR);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    int fd = dirfd(dir);
    if (fd == -1)
    {
        perror("dirfd");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    errno = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
        printf("inode number: %ld\n", entry->d_ino);
        printf("file type: %s\n", (entry->d_type == DT_REG) ? "regular file" : "directory");
        printf("offset: %ld\n", entry->d_off);
        printf("reclen: %d\n", entry->d_reclen);
        printf("\n");
    }
    if (errno != 0)
    {
        perror("readdir");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
fork_wait_exec
inode number: 6758610
file type: directory
offset: 716564984028209469
reclen: 40

signal
inode number: 6758988
file type: directory
offset: 1365639168520691452
reclen: 32

clone
inode number: 8271215
file type: directory
offset: 2467244234154767055
reclen: 32

shared_memory
inode number: 6783252
file type: directory
offset: 2691325509346160338
reclen: 40

.
inode number: 6758927
file type: directory
offset: 2954419325159455962
reclen: 24

semaphore
inode number: 6783285
file type: directory
offset: 3597085960879157901
reclen: 32

..
inode number: 6758591
file type: directory
offset: 4121416458249650593
reclen: 24

thread
inode number: 7483095
file type: directory
offset: 5009929851864891065
reclen: 32

File_System
inode number: 8162321
file type: directory
offset: 5080249967330303866
reclen: 32

LT1
inode number: 8273818
file type: directory
offset: 6292196210399208136
reclen: 24

beej.pdf
inode number: 6739231
file type: regular file
offset: 8422380753960005121
reclen: 32

pipe
inode number: 6759118
file type: directory
offset: 9223372036854775807
reclen: 24
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>

#define TARGET_DIR "/home/bratin/6TH-SEMESTER-CSE/OPERATING_SYSTEMS_LABORATORY/MATERIALS"

int main(int argc, char *argv[])
{
    DIR *dir = opendir(TARGET_DIR);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    int fd = dirfd(dir);
    if (fd == -1)
    {
        perror("dirfd");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry = (struct dirent *)malloc(sizeof(struct dirent));
    if (entry == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    while ((readdir_r(dir, entry, &entry)) == 0)
    {
        if (entry == NULL)
        {
            break;
        }
        printf("Name: %s\n", entry->d_name);
        printf("inode: %d\n", entry->d_ino);
        printf("offset: %d\n", entry->d_off);
        printf("reclen: %d\n", entry->d_reclen);
        printf("Type %s\n", (entry->d_type == DT_DIR) ? "Directory" : "File");
        printf("\n");
    }
    if (errno != 0)
    {
        perror("readdir");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
Name: fork_wait_exec
inode: 6758610
offset: 1937514813
reclen: 40
Type Directory

Name: signal
inode: 6758988
offset: -1693901060
reclen: 32
Type Directory

Name: clone
inode: 8271215
offset: 384627407
reclen: 32
Type Directory

Name: shared_memory
inode: 6783252
offset: -1618579758
reclen: 40
Type Directory

Name: .
inode: 6758927
offset: 430816474
reclen: 24
Type Directory

Name: semaphore
inode: 6783285
offset: -1933626739
reclen: 32
Type Directory

Name: ..
inode: 6758591
offset: -1117044319
reclen: 24
Type Directory

Name: thread
inode: 7483095
offset: 302530233
reclen: 32
Type Directory

Name: File_System
inode: 8162321
offset: -789995654
reclen: 32
Type Directory

Name: LT1
inode: 8273818
offset: -1444453688
reclen: 24
Type Directory

Name: beej.pdf
inode: 6739231
offset: 182414849
reclen: 32
Type File

Name: pipe
inode: 6759118
offset: -1
reclen: 24
Type Directory
*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(void)
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir("./");
    if (dp != NULL)
    {
        while (ep = readdir(dp))
            puts(ep->d_name);
        (void)closedir(dp);
    }
    else
        perror("Couldn't open the directory");

    return 0;
}

/* Output of the C code:
.
..
bin.c
a.out
bin.out
output.c
*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(void)
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir("/home/bratin/6TH-SEMESTER-CSE");
    if (dp != NULL)
    {
        while (ep = readdir(dp))
            puts(ep->d_name);
        (void)closedir(dp);
    }
    else
        perror("Couldn't open the directory");

    return 0;
}

/* Output of the C code:
template.tex
readme.md
NETWORKS_LAB
schedule.png
.
..
OPERATING_SYSTEMS
COMPUTER_NETWORKS
OPERATING_SYSTEMS_LABORATORY
iit_kgp_logo.png
.gitignore
schedule.pdf
schedule.ics
.git
DATABASE_MANAGEMENT_SYSTEMS_LABORATORY
st_info.txt
DATABASE_MANAGEMENT_SYSTEMS
MACHINE_LEARNING
*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(void)
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir(".");
    if (dp != NULL)
    {
        while (ep = readdir(dp))
            puts(ep->d_name);
        rewinddir(dp);
        while (ep = readdir(dp))
            puts(ep->d_name);
        (void)closedir(dp);
    }
    else
        perror("Couldn't open the directory");

    return 0;
}
/* Output of the C code:
.
..
bin.c
a.out
bin.out
output.c
.
..
bin.c
a.out
bin.out
output.c
*/

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#define TARGET_FILE "bin.c"

int main(void)
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir(".");
    if (dp != NULL)
    {
        long int pos;
        while (1)
        {
            long int curpos = telldir(dp);
            ep = readdir(dp);
            if (strcmp(ep->d_name, TARGET_FILE) == 0)
            {
                pos = curpos;
                break;
            }
        }
        seekdir(dp, pos);
        ep = readdir(dp);
        puts(ep->d_name);
    }

    else
        perror("Couldn't open the directory");

    return 0;
}
/* Output of the C code:
bin.c
*/
