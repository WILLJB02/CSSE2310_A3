#ifndef READ_H
#define READ_H
#include<stdio.h>
#include<stdbool.h>

/*
 * Function which reads characters from the file until a /n character is 
 * reached or EOF is reaced. Returns all the read characters in a string.
 * Paramters:
 * FILE* file - file to be read from.
 */
char* read_file_line(FILE* file);

/*
 * Function which checks if the response from a client is of the correct 
 * structure as per table 3 in the spec sheet. Returns true if correct and 
 * false otherwise.
 * Paramters:
 * char* resposne - response string from client
 */
bool valid_response(char* response);

/*
 * Function which checks if the correct number of arguments has been suplied 
 * when running a program. If an incorrect number has been provided the 
 * function causes the program to exit with 0 and print and error message.
 * Paramters:
 * int argc - number of arguments supplied to the program.
 * char* errorMerssage - error Message to be printed.
 */
void check_argc(int argc, char* errorMessage);

/*
 * Function which checks if the given filename exsists. If it doesnt the 
 * function causes the program to exit with 0 and print and error message.
 * Paramters:
 * FILE* file - file to be checked.
 * char* errorMerssage - error Message to be printed.
 */
void check_file(FILE* file, char* errorMessage);

/*
 * Function which checks if a line the a file is a comment 
 * (leading non whitespace character is #). Returns true if a comment and 
 * false otherwise.
 * Paramters:
 * char* configfileLine - string to check weather it is a comment.
 */
bool is_comment(char* configfileLine);

#endif
