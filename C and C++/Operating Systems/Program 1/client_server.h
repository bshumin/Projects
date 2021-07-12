/* CPSC/ECE 3220 client server using unnamed pipes
 *
 * header file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

#define CLIENT_TYPES 4
#define MAX_CLIENTS 10

#define READ_FD  0
#define WRITE_FD 1

void client_type_a( int, int[], int[] );
void client_type_b( int, int[], int[] );
void client_type_c( int, int[], int[] );
void client_type_d( int, int[], int[] );

void server( int, int[][2], int[][2] );
