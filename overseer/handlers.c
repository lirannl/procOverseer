#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int get_memory_usage(pid_t pid) {
    int fd, data, stack;
    char buf[4096], status_child[1000];
    char *vm;

    sprintf(status_child, "/proc/%d/status", pid);
    if ((fd = open(status_child, O_RDONLY)) < 0)
        return -1;

    read(fd, buf, 4095);
    buf[4095] = '\0';
    close(fd);

    data = stack = 0;

    vm = strstr(buf, "VmData:");
    if (vm) {
        sscanf(vm, "%*s %d", &data);
    }
    vm = strstr(buf, "VmStk:");

    if (vm) {
        sscanf(vm, "%*s %d", &stack);
    }

    return data + stack;
}

char  *memHandler(pid_t *pidChild) {
     return  print_entry(memOverseer, fileno(stdout));
}

void memkill_handler(char **args, pid_t *pidChild, int num_threads) {
    struct sysinfo info;
    double percent;
    char *ptr;

    pid_t pid_child_cleaned[num_threads];
    percent = strtod(args[1], &ptr);

    if (sysinfo(&info) != 0) {
        perror("sysinfo: error reading system statistics");
    }
    double total_memory = info.totalram;

    for (int i = 0; i < num_threads; i++) {
        int memory_used = get_memory_usage(pidChild[i]);
        // Kill process if it uses too much memory
        if (memory_used / total_memory * 100.0 > percent) {
            kill(pidChild[i], SIGKILL);
        } else {
            pid_child_cleaned[i] = pidChild[i];
        }
    }
    pidChild = pid_child_cleaned;
}

