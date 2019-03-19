/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#include <stdlib.h>
#include <nanomsg/nn.h>
#include <nanomsg/pair.h>
#include <nanomsg/ipc.h>
#include <string.h>
#include "requests.h"
#include "requestHandlers.h"
#include <signal.h>
#include <unistd.h>

static void * outMessage;
static int socket;
static int bind;

//ends the connection and exits the program, likely in case of an error
void handleQuit(int sig)
{
    free(outMessage);
    nn_shutdown(socket,bind);
    exit(0);
}

//opens socket connection and checks for messages to determine protocol
int main(int argc, const char * argv[])
{
    signal(SIGINT, handleQuit);
    signal(SIGQUIT, handleQuit);
    signal(SIGTERM, handleQuit);

    char buffer [4000];
    //contains the message type and length of message
    Header header;
    //maintains information about the current session
    State state;
    state.lastRecieved = 6;
    state.lastSent = 5;
    //create socket
    socket = nn_socket(AF_SP,NN_PAIR);
    //add endpoint to socket
    bind = nn_bind(socket,"ipc:///tmp/getd.ipc");
    //stores the number of bytes in the received message
    unsigned int bytesRecieved;
    //stoes the number of bytes in the sent message
    unsigned int outSize;
    outMessage = malloc(10000);

    //listen for messages
    while (-1) {
        //stores the bytes received from a message
        bytesRecieved = nn_recv(socket,(void *)buffer,4000,0);
        //copies the first bytes of the message into header
        memcpy(&header,buffer, sizeof(Header));
        //determines which protocol to run based on the header
        switch(header.messageType){
            //request session id
            case 0:
                outSize = MessageType0Handler((MessageType0 *)buffer,&state,outMessage);
                //if outSize is a good value, create Type 1 message using the state data and send to client
                if (outSize == 0)
                {
                    
                }
                else
                {

                }
                //else report error and close the connection
                break;
            //request contents of a file
            case 3:
                //keep checking for file fragments until whole file is received
                do
                {
                    outSize = MessageType3Handler((MessageType3 *) buffer, &state, outMessage);
                    nn_send(socket,outMessage,outSize,0);
                    state.lastSent = ((Header*)outMessage)->messageType;
                }
                while (state.lastSent == 4);

                break;
            //acknowledge receipt of type 4 message
            case 6:
                outSize = MessageType6Handler((MessageType6 *)buffer,&state,outMessage);
                break;
            //unrecognized message type
            default:
                outSize = MessageOtherHandler(buffer,header.messageType,&state,outMessage);
                nn_send(socket,outMessage,outSize,0);
                state.lastSent = ((Header*)outMessage)->messageType;
                break;
        }
    }
}