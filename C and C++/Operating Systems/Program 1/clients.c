/* CPSC/ECE 3220 sample clients for client server
 *
 * four client types are defined in this file
 *  a - trivial client that immediately stops
 *  b - trivial client that sleeps for 3 seconds to test server time out
 *  c - client that requests two increments and stops, printing the reply
 *        after each request
 *  d - client that requests three decrements and stops, printing the
 *        final reply
 */

#include "client_server.h"


void client_type_a( int client_id, int req_fds[], int reply_fds[] ){
  char stop = '!';

  printf( "client %d, type a, with file descriptors %d %d %d %d\n",
    client_id, req_fds[0], req_fds[1], reply_fds[0], reply_fds[1] );

  /* close the ends of the two pipes that are unused by the client */
  close( req_fds[READ_FD] ); close( reply_fds[WRITE_FD] );

  write( req_fds[WRITE_FD], &stop, 1 );

  /* close the other ends of the pipes */
  close( req_fds[WRITE_FD] ); close( reply_fds[READ_FD] );

  exit( 0 );
}


void client_type_b( int client_id, int req_fds[], int reply_fds[] ){
  char stop = '!';

  printf( "client %d, type b, with file descriptors %d %d %d %d\n",
    client_id, req_fds[0], req_fds[1], reply_fds[0], reply_fds[1] );

  /* close the ends of the two pipes that are unused by the client */
  close( req_fds[READ_FD] ); close( reply_fds[WRITE_FD] );

  sleep( 3 );

  write( req_fds[WRITE_FD], &stop, 1 );

  /* close the other ends of the pipes */
  close( req_fds[WRITE_FD] ); close( reply_fds[READ_FD] );

  exit( 0 );
}


void client_type_c( int client_id, int req_fds[], int reply_fds[] ){
  char plus = '+';
  char stop = '!';
  int reply;

  printf( "client %d, type c, with file descriptors %d %d %d %d\n",
    client_id, req_fds[0], req_fds[1], reply_fds[0], reply_fds[1] );

  sleep( 1 );

  /* close the ends of the two pipes that are unused by the client */
  close( req_fds[READ_FD] ); close( reply_fds[WRITE_FD] );

  write( req_fds[WRITE_FD], &plus, 1 );
  read( reply_fds[READ_FD], (char *) &reply, 4 );
  printf( "reply from server to client %d is %d\n", client_id, reply );

  write( req_fds[WRITE_FD], &plus, 1 );
  read( reply_fds[READ_FD], (char *) &reply, 4 );
  printf( "reply from server to client %d is %d\n", client_id, reply );

  write( req_fds[WRITE_FD], &stop, 1 );

  /* close the other ends of the pipes */
  close( req_fds[WRITE_FD] ); close( reply_fds[READ_FD] );

  exit( 0 );
}

void client_type_d( int client_id, int req_fds[], int reply_fds[] ){
  char subtract = '-';
  char stop = '!';
  int reply;

  printf( "client %d, type d, with file descriptors %d %d %d %d\n",
    client_id, req_fds[0], req_fds[1], reply_fds[0], reply_fds[1] );

  sleep( 1 );

  /* close the ends of the two pipes that are unused by the client */
  close( req_fds[READ_FD] ); close( reply_fds[WRITE_FD] );

  write( req_fds[WRITE_FD], &subtract, 1 );
  read( reply_fds[READ_FD], (char *) &reply, 4 );
  write( req_fds[WRITE_FD], &subtract, 1 );
  read( reply_fds[READ_FD], (char *) &reply, 4 );
  write( req_fds[WRITE_FD], &subtract, 1 );
  read( reply_fds[READ_FD], (char *) &reply, 4 );

  printf( "final reply from server to client %d is %d\n", client_id, reply );

  write( req_fds[WRITE_FD], &stop, 1 );

  /* close the other ends of the pipes */
  close( req_fds[WRITE_FD] ); close( reply_fds[READ_FD] );

  exit( 0 );
}
