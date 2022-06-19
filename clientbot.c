#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include <ctype.h>
#include "shared.h"
#include "sharedClientFunctions.h"

/*
 * Function which converts a string to all lower case letters and returns.
 * Paramters:
 * char* phrase - string to be converted.
 */
char* phrase_to_lower(char* phrase);

/*
 * Function which will print the responses to all the stimulus that has 
 * currenlty been matched by the clientbot. After doing so the list of 
 * mathched stimulus is reset and DONE: is printed to STDOUT.
 * Paramaters:
 * char** matchedStimulus - array contianing all the stimulus which as been 
 * matched.
 * int* count - number of stimulus which have been mathced.
 * int* bufferSize - size of buffer for matchedStimulus array.
 */
void output_stimulus_responses(char** matchedStimulus, int* count, 
        int* bufferSize);

/*
 * Function which checks if a given message contians a given simtulus from the 
 * repsonse file. If it does then this stimulus is stored in an array and
 * true is returned. Otherwise false is returned.
 * Paramters:
 * char** matchedStimulus - array contianing all the stimulus which as been 
 * matched.
 * int* count - number of stimulus which have been mathced.
 * char* message - string that is to be checked for stimulus.
 * char* responsefileLine - line from responsefile with given stimulus.
 * char* clientName - name of client sending original message.
 * int iteration - number of times name_taken was asked.
 */
bool check_stimulus_match(char** matchedStimulus, int* count, 
        char* message, char* responsefileLine, char* clientName, 
        int iteration);

int main(int argc, char** argv) {
    check_argc(argc, "Usage: clientbot responsefile\n");
    int responsefileDiscriptor = open(argv[1], O_RDONLY);
    FILE* responsefile = fdopen(responsefileDiscriptor, "r");
    check_file(responsefile, "Usage: clientbot responsefile\n");
    int iteration = -1;
    char* serverMessage, *commandWord, *responsefileLine;
    int* bufferSize = malloc(sizeof(int));
    int* count = malloc(sizeof(int));
    char** matchedStimulus = (char**) malloc(sizeof(char*) * *bufferSize);
    *count = 0, *bufferSize = 1;
    while (1) {  // conitnues reading from STDINN until exit.
        serverMessage = read_file_line(stdin);
        if (serverMessage[0] == '\0') {
            communications_error();
        }
        if (strcmp(serverMessage, "WHO:") == 0) {
            send_name("clientbot", iteration);
        } else if (strcmp(serverMessage, "NAME_TAKEN:") == 0) { 
            iteration++;
        } else if (strcmp(serverMessage, "YT:") == 0) {
            output_stimulus_responses(matchedStimulus, count, bufferSize);
        } else if (strcmp(serverMessage, "KICK:") == 0) {
            kicked();
        } else {
            commandWord = strtok(serverMessage, ":");
            if (strcmp(commandWord, "MSG") == 0) {
                char* name = strtok(NULL, ":");
                char* message = output_message(name);
                do { // compares message to all stimulus in responsefile
                    responsefileLine = read_file_line(responsefile);
                    bool stimulusMatch = check_stimulus_match(matchedStimulus, 
                            count, message, responsefileLine, name, iteration);
                    if (stimulusMatch == true && ++(*count) == *bufferSize) {
                        matchedStimulus = realloc(matchedStimulus, 
                                (sizeof(char*) * (*bufferSize *= 2)));
                    }   
                } while (!feof(responsefile));
                fclose(responsefile);
                responsefileDiscriptor = open(argv[1], O_RDONLY);
                responsefile = fdopen(responsefileDiscriptor, "r");
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

bool check_stimulus_match(char** matchedStimulus, int* count, char* message, 
        char* responsefileLine, char* clientName, int iteration) {
    // checks wether line of responsefile is valid
    char* name = malloc(sizeof(char*));
    if (iteration == -1) {
        sprintf(name, "clientbot");  
    } else {
        sprintf(name, "clientbot%d", iteration);  
    }
    // only check for match if valid file line and it is not the clienbots
    // message
    if (responsefileLine[0] != '\0' && !is_comment(responsefileLine) &&
            strcmp(name, clientName) != 0) {
        char* stimulus, *response, *lowerCaseMessage;
        stimulus = strtok(responsefileLine, ":");
        response = strtok(NULL, ":");
        if (response == NULL) {
            return false;
        }
        stimulus = phrase_to_lower(stimulus);
        lowerCaseMessage = phrase_to_lower(message);
        if (strstr(lowerCaseMessage, stimulus) != NULL) {
            matchedStimulus[*count] = (char*) malloc(sizeof(response));
            matchedStimulus[*count] = response;
            return true;
        }     
    }
    return false;  
}

void output_stimulus_responses(char** matchedStimulus, int* count, 
        int* bufferSize) {
    // prints all responses
    for (int i = 0; i < *count; i++) {
        fprintf(stdout, "CHAT:%s\n", matchedStimulus[i]);
    }
    free(matchedStimulus);
    matchedStimulus = (char**)  malloc(sizeof(char*) * *bufferSize); 
    fprintf(stdout, "DONE:\n");
    *count = 0;
}

char* phrase_to_lower(char* phrase) {
    for (int i = 0; i < strlen(phrase); i++) {
        phrase[i] = tolower(phrase[i]);
    }
    return phrase;
}
