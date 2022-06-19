#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include "sharedClientFunctions.h"

void output_left() {
    char* clientName = strtok(NULL, ":");
    fprintf(stderr, "(%s has left the chat)\n", clientName);
}

char* output_message(char* clientName) {
    char* message = strtok(NULL, ":");
    if (message == NULL) {
        communications_error();
    }
    fprintf(stderr, "(%s) %s\n", clientName, message);
    return message;
}

void send_name(char* name, int iteration) {
    if (iteration == -1) {
        printf("NAME:%s\n", name);
        fflush(stdout);
    } else {
        printf("NAME:%s%d\n", name, iteration);
        fflush(stdout);
    }
}

void communications_error() {
    fprintf(stderr, "Communications error\n");
    exit(2);
}

void kicked() {
    fprintf(stderr, "Kicked\n");
    exit(3);
}

void check_eof() {
    if (feof(stdin)) {
        communications_error();
    }
}

