#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "shared.h"
#define COMMAND_STRING_LENGTH 5

/* a client which has been added to the server*/
struct Client {
    int pipeServer[2];
    int pipeClient[2];
    int errorPipe[2];
    FILE* writeServer;
    FILE* readClient;
    char* name;
    bool left;
};

/*
 * A function which returns true if all clients have quit the server. Returns
 * false otherwise.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
bool all_quit(struct Client** clients, int* clientCount);

/*
 * A function which: closes the read end of the pipe from the server to the 
 * client, opens a file to write to the child at the write end of the server 
 * to client pipe, closes the write end of the client to server pipe, 
 * and opens a file to read from the client to server pipe.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
void set_child_pipes(struct Client** clients, int* clientCount);

/*
 * A function which: closes the write end of the pipe from the server to the 
 * client, redirects the read end from the server to client pipe to stdin of 
 * the child, closes the read end of the client to server pipe,  and redirects
 * the write end of the server to client pipe to stdout.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
void set_parent_pipes(struct Client** clients, int* clientCount);

/*
 * A function which generates two sets of pipes for a given client. One for 
 * the server to the given client and one from the given client to the server.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
void create_pipes(struct Client** clients, int* clientCount);

/*
 * Function which checks if a given name is currently taken by anybody on the 
 * server. If it has then true is returned and if not false it returned.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 * char* name - name that is being checked if it is taken.

 */
bool name_valid(struct Client** clients, int* clientCount, char* name);

/*
 * Function that returns true if a string is of the format KICK:name and
 * false otherwise.
 * Paramters:
 * char* clientResposne - string to be checked.
 */
bool check_kick(char* clientResponse);

/*
 * Returns the position that a client entered the server given its name.
 * eg if clientbot entered the server 3rd then 3 would be returns.
 * -1 is returned if the name dosent match any client in the server.
 * Paramters:
 * char* name - name of client ot find.
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
int find_client(char* name, struct Client** clients, int* clientCount);

/*
 * A function which prints to standard out that a certan client has left, 
 * records that it has left, and then broadcasts a message to all other 
 * active clients in the server that the client has left.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 * int clientIdentifier - a number which represents the position a client 
 * entered the server.
 */
void broadcast_left(struct Client** clients, int* clientCount, 
        int clientIdentifier);

/*
 * A function which broadcasts to all other active clients (ie not themselves)
 * that a message has been sent to by the client.
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 * int clientIdentifier - a number which represents the position a client 
 * entered the server.

 */
void broadcast_message(struct Client** clients, int* clientCount, 
        char* message, int clientIdentifier);

/*
 * A function which will send a single round of YT:s to every client 
 * currently active on the server a print the given response to standard out. 
 *  Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 */
void server_client_communication_round(int* clientCount, 
        struct Client** clients);

/*
 * Function which conducts launches the the given client program from the 
 * configfile file in a child program and then completes name negotiation 
 * for a the client and saves its name to the clients array.
 * Paramters:
 * char* participating client - client that is to be added to the server
 * Paramters:
 * struct Client** clients - an array conaining all clients that have been
 * in the surver
 * int* clientCount - number of clients that have been in the server.
 * int* bufferSize - size of buffer for the client array.
 */
void initalise_client(char* participatingClient, int* clientCount,
        struct Client** clients, int* bufferSize);

/*
 * Function which returns true if a given string is of the format Name:name 
 * and false otherwise.
 * Parameters:
 * char* clientResposne - string to be checked.
 */
bool check_name(char* clientResponse);

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    check_argc(argc, "Usage: server configfile\n");
    int configfileDiscriptor = open(argv[1], O_RDONLY);
    FILE* configfile = fdopen(configfileDiscriptor, "r");
    check_file(configfile, "Usage: server configfile\n");
    char* participatingClient;
    int* bufferSize = malloc(sizeof(int));
    int* clientCount = malloc(sizeof(int));
    struct Client** clients = malloc(*bufferSize * sizeof(*clients));
    bool allQuit;
    *clientCount = 0;
    *bufferSize = 256;
    do { //iterate through all clients in configFile and add to server
        participatingClient = read_file_line(configfile);  
        if (participatingClient[0] != '\0' && !is_comment(participatingClient) 
                && strchr(participatingClient, ':') != NULL) {
            initalise_client(participatingClient, clientCount, clients, 
                    bufferSize);
            if (++*clientCount == *bufferSize) {
                clients = realloc(clients, (*bufferSize *= 2) * 
                        sizeof(*clients));
            } 
        } 
        fflush(stdout);
    } while (!feof(configfile));
    // emit YT:'s in round robin fashion to clients until all have 
    // left the server
    while ((allQuit = all_quit(clients, clientCount)) == false) {
        server_client_communication_round(clientCount,clients);
    }
    exit(0);
}

void initalise_client(char* participatingClient, int* clientCount, 
        struct Client** clients, int* bufferSize) {
    bool nameValid = false;
    char* name;
    int pid;
    char* program = strtok(participatingClient, ":");
    char* chatscript = strtok(NULL, ":");
    clients[*clientCount] = malloc(sizeof(struct Client));
    create_pipes(clients, clientCount);
    if ((pid = fork())) { // parent
        set_parent_pipes(clients, clientCount);
        // continue asking who until unqiue name is returned
        while(nameValid == false) {
            fprintf(clients[*clientCount]->writeServer, "WHO:\n");
            fflush(clients[*clientCount]->writeServer);
            fflush(clients[*clientCount]->readClient);
            char* clientResponse = 
                read_file_line(clients[*clientCount]->readClient);
            // client leaves before name negotiation after bad response
            if(check_name(clientResponse) == false) {
                clients[*clientCount]->left = true;
                break;
            }
            strtok(clientResponse, ":"); 
            name = strtok(NULL, ":");
            nameValid = name_valid(clients, clientCount, name);
            if (nameValid == true) {
                printf("(%s has entered the chat)\n", name);
                fflush(clients[*clientCount]->readClient);
                clients[*clientCount]->name= name;
                clients[*clientCount]->left = false;
            } else {
                fprintf(clients[*clientCount]->writeServer, "NAME_TAKEN:\n");
            }
        }
    } else { // child
        set_child_pipes(clients, clientCount);
        execlp(program, program, chatscript, NULL); // run client program
    }
}

bool check_name(char* clientResponse) {
    char commandWord[COMMAND_STRING_LENGTH + 1];
    memcpy(commandWord, clientResponse, COMMAND_STRING_LENGTH);
    commandWord[COMMAND_STRING_LENGTH] = '\0';
    if (strcmp(commandWord, "NAME:") == 0) {
        return true;
    }
    return false;
}

void server_client_communication_round(int* clientCount, 
        struct Client** clients) {
    char* clientResponse, *commandWord;
    // send YT to each client in the server that has not left
    for (int i = 0; i < *clientCount; i++) {
        if (clients[i]->left == false) {
            fprintf(clients[i]->writeServer, "YT:\n");
            fflush(clients[i]->writeServer);
            do { //continue reading from client until is exits or recieve DONE:
                clientResponse = read_file_line(clients[i]->readClient);
                // exit beacuse of invalid client response
                if (feof(clients[i]->readClient) ||
                        valid_response(clientResponse) == false) {
                    broadcast_left(clients, clientCount, i);
                    break;
                }
                commandWord = strtok(clientResponse, ":");
                char* name = strtok(NULL, ":");
                if (strcmp(commandWord, "KICK") == 0) {
                    int clientIdentifier = find_client(name, clients, 
                            clientCount);
                    // only attempt to kick client if valid name has been given
                    if (clientIdentifier != -1) {
                        fprintf(clients[clientIdentifier]->writeServer, 
                                "KICK:\n");
                        broadcast_left(clients, clientCount, clientIdentifier);
                    } else {
                        broadcast_left(clients, clientCount, i);
                        break;
                    }
                } else if (strcmp(commandWord, "QUIT") == 0) {
                    broadcast_left(clients, clientCount, i);
                    break;
                } else if (strcmp(commandWord, "DONE") == 0) {
                    break;
                } else {
                    printf("(%s) %s\n", clients[i]->name, name);
                    broadcast_message(clients, clientCount, name, i);
                }
            } while (1);
            fflush(stdout);
        }
    }
}

void broadcast_message(struct Client** clients, int* clientCount, 
        char* message, int clientIdentifier) {
    for (int j = 0; j < *clientCount;j++) {
        // only broadcast message to other clients that havent left.
        if (clients[j]->left == false && j != clientIdentifier) {
            fprintf(clients[j]->writeServer, "MSG:%s:%s\n", 
                    clients[clientIdentifier]->name, message);
        }
    }
}

void broadcast_left(struct Client** clients, int* clientCount, 
        int clientIdentifier) {
    printf("(%s has left the chat)\n", clients[clientIdentifier]->name);
    clients[clientIdentifier]->left = true;
    for (int j = 0; j < *clientCount;j++) {
        if (clients[j]->left == false) {
            fprintf(clients[j]->writeServer, "LEFT:%s\n", 
                    clients[clientIdentifier]->name);
        }
    }
}

bool all_quit(struct Client** clients, int* clientCount) {
    for (int i = 0; i < *clientCount; i++) {
        if (clients[i]->left == false) {
            return false;
        }
    }
    return true;
}

bool check_kick(char* clientResponse) {
    char commandWord[COMMAND_STRING_LENGTH + 1];
    memcpy(commandWord, clientResponse, COMMAND_STRING_LENGTH);
    commandWord[COMMAND_STRING_LENGTH] = '\0';
    if (strcmp(commandWord, "KICK:") == 0) {
        return true;
    }
    return false;
}

int find_client(char* name, struct Client** clients, int* clientCount) {
    for (int i = 0; i < *clientCount; i++) {
        if (clients[i]->left == false && strcmp(clients[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_child_pipes(struct Client** clients, int* clientCount) {
    // close write end of server -> client
    close(clients[*clientCount]->pipeServer[1]);
    // dupe read end of server -> client on to standard in.
    dup2(clients[*clientCount]->pipeServer[0], 0);
    // close read end server -> client
    close(clients[*clientCount]->pipeServer[0]);
    // close read end client -> server
    close(clients[*clientCount]->pipeClient[0]);
    // dup write end of client -> server onto standar out
    dup2(clients[*clientCount]->pipeClient[1], 1);
    // close write end client -> server
    close(clients[*clientCount]->pipeClient[1]);
    // close read end client -> server error pipe
    close(clients[*clientCount]->errorPipe[0]);
    // dup write end of client -> server error pipe onto STDERR
    dup2(clients[*clientCount]->errorPipe[1], 2);
    // close write end of client -> server error pipe
    close(clients[*clientCount]->errorPipe[1]);
}

void set_parent_pipes(struct Client** clients, int* clientCount) {
    //close read end server -> client
    close(clients[*clientCount]->pipeServer[0]);     
    //close write end of client -> server
    close(clients[*clientCount]->pipeClient[1]); 
    //file to write from the server to Client
    clients[*clientCount]->writeServer = 
            fdopen(clients[*clientCount]->pipeServer[1], "w");
    //file to read from client
    clients[*clientCount]->readClient = 
            fdopen(clients[*clientCount]->pipeClient[0], "r");
}

void create_pipes(struct Client** clients, int* clientCount) {
    pipe(clients[*clientCount]->pipeServer);
    pipe(clients[*clientCount]->pipeClient);
    pipe(clients[*clientCount]->errorPipe);
}

bool name_valid(struct Client** clients, int* clientCount, char* name) {
    for (int j = 0; j < *clientCount; j++) {
        if (clients[j]->left == false && strcmp(clients[j]->name, name) == 0) {
            return false;
        }
    }
    return true;
}


