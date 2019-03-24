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
#include <stdio.h>

#define asHeader(x) (*((Header*)(x)))

//stores the message that getd sends to get
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

    //stores the data from the received message
    char buffer [4000];

    //maintains information about the current session
    State state;
    state.lastRecieved = 6;
    state.lastSent = 5;

    //create socket
    socket = nn_socket(AF_SP,NN_PAIR);

    //add endpoint to socket
    bind = nn_bind(socket,"ipc:///tmp/getd.ipc");

    //stores the number of bytes in the message that getd will send to get
    unsigned int outSize;
    outMessage = malloc(10000);

    //listen for messages
    while (-1) {
        //stores the bytes received from a message
        nn_recv (socket, buffer, 4000, 0);

        //gets the type of message based on the header
        char mType = asHeader(buffer).messageType;
        printf("Received Type %i Message\n", mType);

        switch(mType){
            //request session id
            case 0:
                outSize = MessageType0Handler((MessageType0 *)buffer,&state,outMessage);
                nn_send(socket, outMessage, outSize, 0);
                break;
            //request contents of a file
            case 3:
                outSize = MessageType3Handler((MessageType3 *) buffer, &state, outMessage);
                nn_send(socket,outMessage,outSize,0);
                break;
            //acknowledge receipt of type 4 message
            case 6:
                outSize = MessageType6Handler((MessageType6 *)buffer,&state,outMessage);
                if(outSize != 0)
                    nn_send(socket,outMessage,outSize,0);
                break;
            //unrecognized message type
            default:
                outSize = MessageOtherHandler(buffer,asHeader(buffer).messageType,&state,outMessage);
                nn_send(socket,outMessage,outSize,0);
                break;
        }

        //print which message type getd has sent to get
        mType = asHeader(outMessage).messageType;
        printf("Sent Type %i Message\n", mType);
    }
}