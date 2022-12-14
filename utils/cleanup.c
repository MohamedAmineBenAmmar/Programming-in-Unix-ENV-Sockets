#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv){
    execlp("cleanup", "rm", "./data", NULL);
    return 0;
}