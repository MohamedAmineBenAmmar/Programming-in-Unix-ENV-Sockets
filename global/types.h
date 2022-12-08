#ifndef TYPES_HEADER_FILE
#define TYPES_HEADER_FILE

#include "constants.h"
#include "libs.h"

typedef struct Request
{
    int data;
    pid_t client_pid;
} Request;

typedef struct Response
{
    int data[NMAX];
    int size;
    pid_t server_pid;
    pid_t server_child_pid;
} Response;

typedef struct Ack
{
    int confirmation;
    pid_t client_pid;
} Ack;

#endif
