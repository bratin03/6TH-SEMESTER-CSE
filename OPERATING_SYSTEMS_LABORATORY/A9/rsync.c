/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <utime.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define VERBOSE
// #define DEBUG

void verbose(char c, char *dst)
{
#ifdef VERBOSE
    printf("[%c] %s\n", c, dst);
#endif
    return;
}

void report_error(char *msg)
{
    if (errno != 0)
    {
        fprintf(stderr, "rsync: %s: %s\n", msg, strerror(errno));
    }
    else
    {
        fprintf(stderr, "rsync: %s\n", msg);
    }
    exit(1);
}

void remove_directory(char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        char msg[PATH_MAX + BUFFER_SIZE];
        snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", path);
        report_error(msg);
    }
    struct dirent *entry;
    struct stat file_stat;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_FIFO || entry->d_type == DT_CHR || entry->d_type == DT_BLK || entry->d_type == DT_SOCK || entry->d_type == DT_LNK)
        {
            continue;
        }
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            char new_path[PATH_MAX];
            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            remove_directory(new_path);
        }
        else if (entry->d_type == DT_REG)
        {
            char new_path[PATH_MAX];
            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            if (unlink(new_path) == -1)
            {
                char msg[PATH_MAX + BUFFER_SIZE];
                snprintf(msg, PATH_MAX + BUFFER_SIZE, "unlink \"%s\" failed", new_path);
                report_error(msg);
            }
            verbose('-', new_path);
        }
    }
    if (rmdir(path) == -1)
    {
        char msg[PATH_MAX + BUFFER_SIZE];
        snprintf(msg, PATH_MAX + BUFFER_SIZE, "rmdir \"%s\" failed", path);
        report_error(msg);
    }
    verbose('-', path);
    closedir(dir);
}

void solve(char *src, char *dst)
{
#ifdef DEBUG
    printf("src: %s\n", src);
    printf("dst: %s\n", dst);
#endif
    DIR *dir_src = opendir(src);
    errno = 0;
    if (dir_src == NULL)
    {
        char msg[PATH_MAX + BUFFER_SIZE];
        snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", src);
        report_error(msg);
    }
    errno = 0;
    DIR *dir_dst = opendir(dst);
    if (dir_dst == NULL)
    {
        char msg[PATH_MAX + BUFFER_SIZE];
        snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", dst);
        report_error(msg);
    }
    struct dirent *entry, *entry_search;
    struct stat src_file_stat;
    struct stat dst_file_stat;
    while ((entry = readdir(dir_src)) != NULL)
    {
        if (entry->d_type == DT_FIFO || entry->d_type == DT_CHR || entry->d_type == DT_BLK || entry->d_type == DT_SOCK || entry->d_type == DT_LNK)
        {
            continue;
        }
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            rewinddir(dir_dst);
            int found = 0;
            while ((entry_search = readdir(dir_dst)) != NULL)
            {
                if (strcmp(entry_search->d_name, ".") == 0 || strcmp(entry_search->d_name, "..") == 0)
                {
                    continue;
                }
                if (strcmp(entry->d_name, entry_search->d_name) == 0 && entry->d_type == DT_DIR)
                {
                    found = 1;
                    break;
                }
            }
            if (found == 1)
            {
                char src_path[PATH_MAX];
                char dst_path[PATH_MAX];
                snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry_search->d_name);
#ifdef DEBUG
                printf("Directory: %s found in dst\n", entry->d_name);
#endif
                solve(src_path, dst_path);
            }
            else
            {
                char src_path[PATH_MAX];
                char dst_path[PATH_MAX];
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry->d_name);
                snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
                if (mkdir(dst_path, 0777) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "mkdir \"%s\" failed", dst_path);
                    report_error(msg);
                }
#ifdef DEBUG
                printf("Directory: %s not found in dst\n", entry->d_name);
#endif
                verbose('+', dst_path);
                solve(src_path, dst_path);
            }
        }
        else if (entry->d_type == DT_REG)
        {
#ifdef DEBUG
            printf("File: %s\n", entry->d_name);
#endif
            rewinddir(dir_dst);
            int found = 0;
            while ((entry_search = readdir(dir_dst)) != NULL)
            {
                if (strcmp(entry_search->d_name, ".") == 0 || strcmp(entry_search->d_name, "..") == 0)
                {
                    continue;
                }
                if (strcmp(entry->d_name, entry_search->d_name) == 0 && entry->d_type == DT_REG)
                {
                    found = 1;
#ifdef DEBUG
                    printf("Found: %s\n", entry->d_name);
#endif
                    break;
                }
            }
            if (found == 1)
            {
                char src_path[PATH_MAX];
                char dst_path[PATH_MAX];
                snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry_search->d_name);
                if (lstat(src_path, &src_file_stat) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "link_stat \"%s\" failed", src_path);
                    report_error(msg);
                }
                if (lstat(dst_path, &dst_file_stat) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "link_stat \"%s\" failed", dst_path);
                    report_error(msg);
                }

                if (src_file_stat.st_mode != dst_file_stat.st_mode)
                {
                    if (chmod(dst_path, src_file_stat.st_mode) == -1)
                    {
                        char msg[PATH_MAX + BUFFER_SIZE];
                        snprintf(msg, PATH_MAX + BUFFER_SIZE, "chmod \"%s\" failed", dst_path);
                        report_error(msg);
                    }
                    verbose('p', dst_path);
                }

                // Check if the files are the same
                if (src_file_stat.st_mtime == dst_file_stat.st_mtime && src_file_stat.st_size == dst_file_stat.st_size)
                {
#ifdef DEBUG
                    printf("Files are the same\n");
#endif
                    continue;
                }
                else
                {
#ifdef DEBUG
                    printf("Files are different\n");
#endif
#ifdef DEBUG
                    printf("src_file_stat.st_mode: %d\n", src_file_stat.st_mode);
                    printf("dst_file_stat.st_mode: %d\n", dst_file_stat.st_mode);
                    printf("src_file_stat.st_mtime: %ld\n", src_file_stat.st_mtime);
                    printf("dst_file_stat.st_mtime: %ld\n", dst_file_stat.st_mtime);
                    printf("src_file_stat.st_size: %ld\n", src_file_stat.st_size);
                    printf("dst_file_stat.st_size: %ld\n", dst_file_stat.st_size);
#endif
                    int src_fd = open(src_path, O_RDONLY);
                    if (src_fd == -1)
                    {
                        char msg[PATH_MAX + BUFFER_SIZE];
                        snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", src_path);
                        report_error(msg);
                    }
                    int dst_fd = open(dst_path, O_WRONLY | O_TRUNC);
                    if (dst_fd == -1)
                    {
                        char msg[PATH_MAX + BUFFER_SIZE];
                        snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", dst_path);
                        report_error(msg);
                    }
                    char buffer[BUFFER_SIZE];
                    ssize_t bytes_read;
                    ssize_t bytes_written;
                    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0)
                    {
                        bytes_written = write(dst_fd, buffer, bytes_read);
                        if (bytes_written != bytes_read)
                        {
                            char msg[PATH_MAX + BUFFER_SIZE];
                            snprintf(msg, PATH_MAX + BUFFER_SIZE, "write \"%s\" failed", dst_path);
                            report_error(msg);
                        }
                    }
                    close(src_fd);
                    close(dst_fd);
                    verbose('o', dst_path);
                    if (src_file_stat.st_mtime != dst_file_stat.st_mtime)
                    {
                        struct utimbuf times;
                        times.actime = src_file_stat.st_atime;
                        times.modtime = src_file_stat.st_mtime;
                        if (utime(dst_path, &times) == -1)
                        {
                            char msg[PATH_MAX + BUFFER_SIZE];
                            snprintf(msg, PATH_MAX + BUFFER_SIZE, "utime \"%s\" failed", dst_path);
                            report_error(msg);
                        }
                        verbose('t', dst_path);
                    }
                }
            }
            else
            {
                char src_path[PATH_MAX];
                char dst_path[PATH_MAX];
                snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry->d_name);
                if (lstat(src_path, &src_file_stat) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "link_stat \"%s\" failed", src_path);
                    report_error(msg);
                }
                int src_fd = open(src_path, O_RDONLY);
                if (src_fd == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", src_path);
                    report_error(msg);
                }
                int dst_fd = open(dst_path, O_WRONLY | O_TRUNC | O_CREAT, src_file_stat.st_mode);
                if (dst_fd == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "open \"%s\" failed", dst_path);
                    report_error(msg);
                }
                char buffer[BUFFER_SIZE];
                ssize_t bytes_read;
                ssize_t bytes_written;
                while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0)
                {
                    bytes_written = write(dst_fd, buffer, bytes_read);
                    if (bytes_written != bytes_read)
                    {
                        char msg[PATH_MAX + BUFFER_SIZE];
                        snprintf(msg, PATH_MAX + BUFFER_SIZE, "write \"%s\" failed", dst_path);
                        report_error(msg);
                    }
                }
                close(src_fd);
                close(dst_fd);
                struct utimbuf times;
                times.actime = src_file_stat.st_atime;
                times.modtime = src_file_stat.st_mtime;
                if (utime(dst_path, &times) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "utime \"%s\" failed", dst_path);
                    report_error(msg);
                }
                verbose('+', dst_path);
            }
        }
    }
    rewinddir(dir_dst);
    while ((entry = readdir(dir_dst)) != NULL)
    {
        if (entry->d_type == DT_FIFO || entry->d_type == DT_CHR || entry->d_type == DT_BLK || entry->d_type == DT_SOCK || entry->d_type == DT_LNK)
        {
            continue;
        }
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            rewinddir(dir_src);
            int found = 0;
            while ((entry_search = readdir(dir_src)) != NULL)
            {
                if (strcmp(entry_search->d_name, ".") == 0 || strcmp(entry_search->d_name, "..") == 0)
                {
                    continue;
                }
                if (strcmp(entry->d_name, entry_search->d_name) == 0 && entry_search->d_type == DT_DIR)
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
#ifdef DEBUG
                printf("Directory: %s in dst not in src\n", entry->d_name);
#endif
                char dst_path[PATH_MAX];
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry->d_name);
                remove_directory(dst_path); 
            }
        }
        else if (entry->d_type == DT_REG)
        {
            rewinddir(dir_src);
            int found = 0;
            while ((entry_search = readdir(dir_src)) != NULL)
            {
                if (strcmp(entry_search->d_name, ".") == 0 || strcmp(entry_search->d_name, "..") == 0)
                {
                    continue;
                }
                if (strcmp(entry->d_name, entry_search->d_name) == 0 && entry_search->d_type == DT_REG)
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
#ifdef DEBUG
                printf("File: %s in dst not in src\n", entry->d_name);
#endif
                char dst_path[PATH_MAX];
                snprintf(dst_path, PATH_MAX, "%s/%s", dst, entry->d_name);
                if (lstat(dst_path, &dst_file_stat) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "link_stat \"%s\" failed", dst_path);
                    report_error(msg);
                }
                if (unlink(dst_path) == -1)
                {
                    char msg[PATH_MAX + BUFFER_SIZE];
                    snprintf(msg, PATH_MAX + BUFFER_SIZE, "unlink \"%s\" failed", dst_path);
                    report_error(msg);
                }
                verbose('-', dst_path);
            }
        }
    }

    // Close dir_srcectory
    closedir(dir_src);
    closedir(dir_dst);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <src> <dst>\n", argv[0]);
        exit(1);
    }
    solve(argv[1], argv[2]);
}
