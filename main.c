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
            case '0':
                outSize = MessageType0Handler((MessageType0 *)buffer,&state);
                //if outSize is a good value, create Type 1 message using the state data and send to client
                if (outSize >= 0)
                {
                    //build type 1 message to send to client
                    //message header
                    Header t1Header;
                    t1Header.messageType = '1';
                    t1Header.messageLength = outSize;
                    MessageType1 t1;
                    t1.header = t1Header;
                    //gets the length of the session id since outSize is sidLength + sizeof(int)
                    t1.sidLength = outSize - sizeof(int);
                    //copy session id generated from the type 0 handler into the type 1 message
                    strncpy(t1.sessionId, state.sessionId, sidLength);

                    //gets the size of the type 1 message including its header
                    outSize += sizeof(char) + sizeof(int);
 
                    //send message
                    nn_send(socket, (void *)t1, outSize, 0);
                }
                else
                {
                    /*
                    //build type 2 message with error based on what outSize returns
                    //if outSize == -1 then return this error
                    //else if outSize == -2 then return this error
                    //else if ...
                    */
                    
                    /*
                    //create type 2 message with error
                    MessageType2 *t2 = MessageType2Builder("error message here");
                    //header size + message size
                    int t2MessageLength = sizeof(char) + sizeof(int) + t2.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t2, t2MessageLength, 0);
                    */
                    handleQuit(0);
                }
                //else report error and close the connection
                break;
            //request contents of a file
            case '3':
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
            case '6':
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