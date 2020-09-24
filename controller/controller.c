#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){
    if (!strcmp(argv[1], "--help"))
        {
            printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
            return 0;
        }
    return 1;
}