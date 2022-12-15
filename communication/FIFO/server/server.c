#include "../../../global/types.h"
#include "../../../global/libs.h"
#include "../../../global/constants.h"
#include "handlers_serv.h"

FILE *fptr;

void close_communication_file()
{
    fptr = fopen(COMMUNICATION, "a");
    fputs("\n}", fptr);
    fclose(fptr);
    kill(getpid(), SIGKILL);
}

int main()
{
    /* Declare the needed variables */
    Request req;
    Response res;
    int req_flag;
    int res_flag;

    int nbr = 0;
    FILE *cfptr;

    /* Creation of named pipes */
    if (mkfifo(FIFO1, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("[-]Could not create the FIFO1 file\n");
            exit(1);
        }
    }

    if (mkfifo(FIFO2, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("[-]Could not create the FIFO2 file\n");
            exit(1);
        }
    }

    /* Init random number generator */
    srand(getpid());

    /* Install handlers */
    signal(SIGUSR1, hand_reveil);

    /* Config all the signals to be aable to stop the server */
    for (int sig = 1; sig < NSIG; sig++)
    {
        if (sig != SIGUSR1 && sig != SIGINT)
        {
            signal(sig, fin_serveur);
        }
    }

    /* Initialize the signal handler so that the serve know when to close the communication file */
    signal(SIGINT, close_communication_file);

    /* Open the communication file */
    fptr = fopen(COMMUNICATION, "w");
    fprintf(fptr, "%s", "{\n\"server\":{\"PID\":");
    fprintf(fptr, "%d", getpid());
    fprintf(fptr, "%s", ", \"port\":");
    fprintf(fptr, "%s", "\"NO_PORT\"");
    fprintf(fptr, "%s", ", \"address\": \"127.0.0.1\"");
    fprintf(fptr, "%s", "}");
    fclose(fptr);

    printf("[+]Server is listening ...\n");
    
    /* Core server code */
    while (1)
    {
        // Opening the named pipes
        int fifo1 = open(FIFO1, O_RDONLY);
        if (fifo1 == -1)
        {
            printf("[-]Could not open the FIFO1 in mode read only from server\n");
            exit(1);
        }
        printf("[+]FIFO1 is created successfully in mode read only from server\n");

        int fifo2 = open(FIFO2, O_WRONLY);
        if (fifo2 == -1)
        {
            printf("[-]Could not open the FIFO2 in mode write only from server\n");
            exit(1);
        }
        printf("[+]FIFO2 is created successfully in mode write only from server\n");

        /* Writing the basic clients data to the communication.json file */
        fptr = fopen(COMMUNICATION, "a");
        nbr++;
        fprintf(fptr, "%s", ",\n\"client_");
        fprintf(fptr, "%d", nbr);
        fprintf(fptr, "%s", "\": {");

        fprintf(fptr, "%s", "\"address\":\"");
        fprintf(fptr, "%s", "127.0.0.1");
        fprintf(fptr, "%s", "\", ");
        fprintf(fptr, "%s", "\"port\": \"");
        fprintf(fptr, "%s", "NO_PORT");
        fprintf(fptr, "%s", "\"}");
        fclose(fptr);

        char client_name[20];
        int length = snprintf(NULL, 0, "%d", nbr);
        char *str_nbr = malloc(length + 1);
        snprintf(str_nbr, length + 1, "%d", nbr);

        strcpy(client_name, "client_");
        strcat(client_name, str_nbr);

        char client_filepath[50];
        strcpy(client_filepath, DATA);
        strcat(client_filepath, client_name);
        strcat(client_filepath, ".json");

        /* Creating a tmp client file that holds all the client communication with the client*/
        cfptr = fopen(client_filepath, "w");
        fclose(cfptr);

        /* Manage the client request */
        /* Read client request */
        req_flag = read(fifo1, &req, sizeof(Request));
        close(fifo1);
        if (req_flag == -1)
        {
            printf("[-]Error occured while receiving request from the client\n");
            exit(2);
        }

        /* Opening the specific client file that will hold his communication with the server */
        cfptr = fopen(client_filepath, "a");
        fprintf(cfptr, "%s", "{");
        fprintf(cfptr, "%s", "\"input\":");
        fprintf(cfptr, "%d", req.data);
        fprintf(cfptr, "%s", ", \"output\": [");

        /* Building response */
        res.size = req.data;
        for (int i = 0; i < res.size; i++)
        {
            int gen_rand_nbr = (rand() % (UPPER - LOWER + 1)) + LOWER;
            fprintf(cfptr, "%d", gen_rand_nbr);
            if (i + 1 < res.size)
            {
                fprintf(cfptr, "%c", ',');
            }
            res.data[i] = gen_rand_nbr;
        }
        fprintf(cfptr, "%s", "] ");
        fprintf(cfptr, "%s", ", ");
        fprintf(cfptr, "%s", "\"server_pid\":");
        fprintf(cfptr, "%d", getpid());
        fprintf(cfptr, "%s", ", ");
        fprintf(cfptr, "%s", "\"pid\":");
        fprintf(cfptr, "%d", req.client_pid);
        fprintf(fptr, "%s", "}");
        fclose(cfptr);

        res.server_pid = getpid();

        /* Send response to the client */
        res_flag = write(fifo2, &res, sizeof(Response));
        close(fifo2);
        if (res_flag == -1)
        {
            printf("[-]Error occured while creating response for the client\n");
            exit(2);
        }

        /* send SIGUSR1 to the user to be notified about the response construction */
        kill(req.client_pid, SIGUSR1);
    }

    return 0;
}
