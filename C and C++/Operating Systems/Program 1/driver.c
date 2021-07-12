/* CPSC/ECE 3220 test driver for client server
 *
 * example usage ./a.out abb
 *
 * the string is traversed to create client processes of specified types
 *   and a server process; "abb" causes the driver to create one client
 *   process of type a and two client processes of type b
 *
 * the length of the command-line string gives the number of clients,
 *   unless the length exceds the MAX_CLIENTS defined value
 *
 * if a character in the string is not a defined client type, then type
 *   a is used as the default (thus "xbb" has the same effect as "abb")
 *
 * each client must have be passed a pair of pipes
 * - a client sends requests to the server using the fd_request pipe
 * - a client receives replies from the server using the fd_reply pipe
 */

#include "client_server.h"

int main( int argc, char *argv[] ){

  /* client types are represented in the choices array as integers */

  int client_type_choices[MAX_CLIENTS];

  /* the choices array entries are used to index into */
  /*   an array of pointers to client functions       */

  void ( *pfn[CLIENT_TYPES] )( int, int[], int[] )
    = { client_type_a, client_type_b, client_type_c, client_type_d };


  int pid[MAX_CLIENTS+1];  /* process ids */


  int fd_request[MAX_CLIENTS][2], fd_reply[MAX_CLIENTS][2];  /* pipe fds */

  int clients;  /* client count based on command-line string */
  int status;   /* exit status from child process */
  int i;        /* index variable */


  if( argc == 1 ){
    printf( "usage: command followed by a string of letters\n" );
    printf( "         that specify your client type choices\n" );
    exit( 0 );
  }

  clients = strlen( argv[1] );
  if( clients >= MAX_CLIENTS ) clients = MAX_CLIENTS;
  printf( "client/server test driver with %d client processes\n", clients );

  for( i = 0; i < clients; i++ ){
    switch( argv[1][i] ){
      case 'a': client_type_choices[i] = 0;
                printf( "client type a selected\n" );
                break;
      case 'b': client_type_choices[i] = 1;
                printf( "client type b selected\n" );
                break;
      case 'c': client_type_choices[i] = 2;
                printf( "client type c selected\n" );
                break;
      case 'd': client_type_choices[i] = 3;
                printf( "client type d selected\n" );
                break;
      default:  client_type_choices[i] = 0;
                printf( "unknown client type specified; " );
                printf( "client type a selected\n" );
    }
  }


  /* fork each client */

  for( i = 0; i < clients; i++ ){
    if( pipe( fd_request[i] ) == -1 ){
      printf( "*** unable to obtain request pipe for client %d\n", i );
      exit( 1 );
    }
    if( pipe( fd_reply[i] ) == -1 ){
      printf( "*** unable to obtain reply pipe for client %d\n", i );
      exit( 1 );
    }
    pid[i] = fork();
    if( pid[i] == -1 ){
      printf( "*** unable to fork client %d\n", i );
      exit( 1 );
    }
    if( pid[i] == 0 ){
      ( *pfn[client_type_choices[i]] )( i, fd_request[i], fd_reply[i] );
    }
  }


  /* fork the server */

  pid[clients] = fork();
  if( pid[clients] == -1 ){
    printf( "*** unable to fork server\n" );
    exit( 1 );
  }
  if( pid[clients] == 0 ) server( clients, fd_request, fd_reply );


  /* wait until all children end (clients and server) */

  printf( "parent waits\n" );
  for( i = 0; i <= clients; i++ ){
    waitpid( pid[i], &status, 0 );
  }
  printf( "parent done\n" );

  return( 0 );
}
