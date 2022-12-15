#include "handlers_serv.h"
#include "../../../global/constants.h"
#include "../../../global/libs.h"

void hand_reveil(int sig){
    printf("[+]Response receipt from client noticed by the server\n");
}

void fin_serveur(int sig)
{
    printf("[+]Going to shut down the server\n[+]Going to delete FIFOs used for server and client communication\n");
    unlink(FIFO1);
    unlink(FIFO2);
    exit(0);   
}
