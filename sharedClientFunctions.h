#ifndef _SHARED_CLIENT_FUNC_H
#define _SHARED_CLIENT_FUNC_H

/*
 * Functions which causes the program to exit with 2 and print 
 * communcation error to STDERROR."
 */
void communications_error();

/*
 * Function which prints the name of a client to STDOUT given the amount of 
 * times NAME_TAKEN has been sent back.
 * Parameters: 
 * char* name - name of client that has sent the message
 * int iteration - number of times NAME_TAKEN has been sent back.
 */ 
void send_name(char* name, int iteration);

/*
 * Function which causes the program to ext with 3 and print kicked to STDERR
 */
void kicked();

/*
 * Function which prints to STDERR the message from a given client.
 * Parameters:
 * char* clientName - name of client who sent message
 */
char* output_message(char* clientName);

/*
 * Function which prints to STDERR that a given client has left.
 */
void output_left();

/*
 * Function which checks if STDINN has reach eof. If it has then a 
 * communcation error is called. If it hasent then nothing happens.
 */
void check_eof();

#endif
