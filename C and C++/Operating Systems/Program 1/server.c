/* CPSC/ECE 3220 server for client server
 *
 * Brandon Shumin
 * C18799120
 *
 * simple server that maintains a counter for each client
 *
 * request    meaning              reply
 * -------    -------              -----
 *   '0'      clear counter        counter value
 *   '+'      increment counter    counter value
 *   '-'      decrement counter    counter value
 *   '!'      stop                 <no reply>
 *
 * there is a timeout of 2 seconds for the call to select()
 */

//  select(int nfds, fd_set *readfds, fd_set *writefds,
//         fd_set *errorfds, struct timeval *timeout)

// fd_X[Y][0] = read_fd, fd_X[Y][0] = write_fd
// request: client->server, reply: server->client

#include "client_server.h"


int counters[MAX_CLIENTS];



/* helper functions can be defined here and used below */
/* NONE USED */


void server( int client_count, int req_fds[][2], int reply_fds[][2] ){

  fd_set read_fds;        /* file descriptor set for active clients      */
  fd_set query_fds;       /* file descriptor set passed to select()      */
  struct timeval timeout; /* time value structure for select() timeout   */
  int active_clients;     /* current count of clients; end server when 0 */
  int max_fds = 0;        /* must be one more than largest req_fd value  */
  int retval;             /* return value from select ()                 */
  int found;              /* logical flag that a req_fd was identified   */
  int reply=0;              /* counter value sent as a reply               */
  char request;           /* single-character command sent to server     */
  int i;


  /* initially clear the file descriptor set for active clients */
  FD_ZERO(&read_fds);
  FD_ZERO(&query_fds);

  /* initialize for each client:            */
  /* - close unused ends of  pipes          */
  for(i=0;i<client_count;i++){
    close(req_fds[i][WRITE_FD]);
    close(reply_fds[i][READ_FD]);
  }
  /* - set req_fd in the read_fds structure */
  for(i=0;i<client_count;i++){
    FD_SET(req_fds[i][READ_FD], &read_fds);
  }

  /* - update max_fds if necessary          */
  for(i=0;i<client_count;i++){
    // find highest value of req_fds by iterating through the array and add 1
    if(req_fds[i][READ_FD] > max_fds){
      max_fds = req_fds[i][READ_FD] + 1;
    }
  }

  /* - clear counter for client */
  for(i=0;i<client_count;i++){
      counters[MAX_CLIENTS] = 0;
  }

  /* set timeout value for 2 seconds */
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;

  /* display information about server and file descriptors */

  printf( "server with %d clients, which have file descriptors\n",
    client_count );
  for( i = 0; i < client_count; i++ ){
    printf( "  %d %d %d %d\n",
      req_fds[i][0], req_fds[i][1], reply_fds[i][0], reply_fds[i][1] );
  }

  /* start processing with all clients active and continue until 0; */
  /*   stop requests should decrement the count of active clients   */

  active_clients = client_count;
  while( active_clients ){
    // prepare for select() call
    // Reset timeout before each call of select()
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    // redeclare query_fds to read_fds just
    query_fds = read_fds;
    // reset reply and found to 0 on each loop to ensure that no erroneous
    // replies or found hits occur
    reply = 0;
    found = 0;

    // watch for request from a client process
    retval = select( max_fds, &query_fds, NULL, NULL, &timeout );

    // determine how to handle retval from select() call
    if (retval == -1){

      fprintf(stdout, "Error code returned from select()\n");
      active_clients = 0;

    } else if(retval == 0){

      fprintf(stdout,"Timeout returned from select()\n");
      active_clients = 0;

    } else {
      // locate, read, and process the request(s)
      // find the client that sent a request and handle the character sent
      for(i=0;i<client_count;i++){
        // locate the request
        found = FD_ISSET(req_fds[i][READ_FD], &query_fds);

        //if found read reply of size 1 byte
        if(found){
          // read the associated request pipe and save the found character
          read(req_fds[i][READ_FD], &request, 1);

          printf("Message from client %d to server is %c\n", i, request);

          // determine how to handle the request
          switch(request){
            // handle termination case
            case '!':
              active_clients--; // decrement clients upon termination
              // clear the read file descriptors for  the terminated client
              FD_CLR(req_fds[i][READ_FD], &read_fds);
              break;

            // handle addition case
            case '+':
              counters[i]++; // increment counter count
              reply = counters[i]; // save reply from the associated counter
              // write the counter reply to the reply pipe matching the
              // request pipe found using FD_ISSET
              write(reply_fds[i][WRITE_FD], (char*)&reply, 4);
              break;

            // handle subtraction case
            case '-':
              counters[i]--; // decrement counter count
              reply = counters[i]; // save reply from the associated counter
              // write the counter reply to the reply pipe matching the
              // request pipe found using FD_ISSET
              write(reply_fds[i][WRITE_FD], (char*)&reply, 4);
              break;

            // handle zero case
            // interpreted instructions as setting the counter to 0 and then
            // replying with 0
            case '0':
              counters[i] = 0;
              reply = counters[i]; // return counter of 0
              // write the counter reply to the reply pipe matching the
              // request pipe found using FD_ISSET
              write(reply_fds[i][WRITE_FD], (char*)&reply, 4);
              break;

            //default case in case a client errors and sends a bad signal
            default:
              // if bad signal, set active clients to zero and print error
              fprintf(stdout,"\n**Error: a bad character was sent.**\n\n");
              active_clients = 0;
          }
        }
      }
    }
  }
  /* close the other file descriptors */
  for(i=0;i<client_count;i++){
    // close the remaining pipes
    close(req_fds[i][READ_FD]);
    close(reply_fds[i][WRITE_FD]);
  }

  exit( 0 );
}
