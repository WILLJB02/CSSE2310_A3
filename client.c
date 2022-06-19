#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include "shared.h"
#include "sharedClientFunctions.h"

void respond_on_turn(FILE* chatscript);

int main(int argc, char** argv) {
    check_argc(argc, "Usage: client chatscript\n");
    int chatscriptDiscriptor = open(argv[1], O_RDONLY);
    FILE* chatscript = fdopen(chatscriptDiscriptor, "r");
    check_file(chatscript, "Usage: client chatscript\n");
    int iteration = -1;
    //continues to ask for input from STDINN indefinetly until the program 
    //exits.
    while (1) {
        char* serverMessage = read_file_line(stdin);
        if (serverMessage[0] == '\0') {
            communications_error();
        } else if (strcmp(serverMessage, "WHO:") == 0) {
            send_name("client", iteration);
        } else if (strcmp(serverMessage, "NAME_TAKEN:") == 0) { 
            iteration++;
        } else if (strcmp(serverMessage, "YT:") == 0) {
            respond_on_turn(chatscript);
        } else if (strcmp(serverMessage, "KICK:") == 0) {
            kicked();
        } else {
            char* commandWord = strtok(serverMessage, ":");
            if (strcmp(commandWord, "MSG") == 0) {
                char* clientName = strtok(NULL, ":");
                output_message(clientName);
            } else if (strcmp(commandWord, "LEFT") == 0) {
                output_left();
            } else {
                communications_error();
            }
        }
        fflush(stdout);
        check_eof();
    }
}

/*
 * After a YT: function is sent by the server this function will read the 
 * chatscript and output the result to STDOUT if the repsonse in the 
 * chatscript is of a valid format. If the format is invalid then that
 * line is ignored. The function will continue reading line by line from the
 * chatscript until DONE:/QUIT: or eof are reached. If EOF is reached program 
 * exits with value of 0.
 * Parameters:
 * FILE* chatscrip - chatscript associated with client. 
 */
void respond_on_turn(FILE* chatscript) {
    char* response;
    // continues to read lines from chatscript until program exits or a DONE:
    do {
        response = read_file_line(chatscript);
        if (valid_response(response)) {
            fprintf(stdout, "%s\n", response);
        }    
        if (feof(chatscript) || strcmp(response, "QUIT:") == 0) {
            exit(0);        
        }  
    } while (strcmp(response, "DONE:") != 0 && strcmp(response, "QUIT:") != 0);
}

