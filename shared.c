#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include <unistd.h>
#include "shared.h"
#define COMMAND_STRING_LENGTH 5

char* read_file_line(FILE* file) {
    int bufferSize = 256;
    int position = 0;
    char* buffer = malloc(bufferSize * sizeof(char));
    int c;
    //reads characters and strores them in string until end of line or file.
    while ((c = fgetc(file)) != '\n' && !feof(file)) {
        fflush(stdout);
        buffer[position] = c;
        if (++position == bufferSize) {
            buffer = realloc(buffer, (bufferSize *= 2) * sizeof(char));
        }
    }
    fflush(stdout);
    buffer = realloc(buffer, (position + 1) * sizeof(char));
    buffer[position] = '\0';
    return buffer;
}

bool valid_response(char* line) {
    char commandWord[COMMAND_STRING_LENGTH + 1];
    memcpy(commandWord, line, COMMAND_STRING_LENGTH);
    commandWord[COMMAND_STRING_LENGTH] = '\0';
    if ((strlen(line) == COMMAND_STRING_LENGTH && 
            (strcmp(line, "DONE:") == 0)) ||
            ((strcmp(line, "QUIT:") == 0) || 
            (strcmp(commandWord, "NAME:") == 0) || 
            (strcmp(commandWord, "CHAT:") == 0) || 
            (strcmp(commandWord, "KICK:") == 0))) {
            return true;
    }
    return false;      
} 

void check_argc(int argc, char* errorMessage) {
    if (argc != 2) {
        fprintf(stderr, errorMessage);
        exit(1);
    }
}

void check_file(FILE* file, char* errorMessage) {
    if (file == 0) {
        fprintf(stderr, errorMessage);
        exit(1);
    }
}

bool is_comment(char* configfileLine) {
    for (int i = 0; i < strlen(configfileLine); i++) {
        if (!isspace(configfileLine[i]) && configfileLine[i] == '#') {
            return true;
        } else if (!isspace(configfileLine[i])) {
                return false;
        }
    }
    return false;
}
