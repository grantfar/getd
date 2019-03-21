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

static int outMessage;
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
    outMessage = 0;

    //listen for messages
    while (-1) {
        //stores the bytes received from a message
        bytesRecieved = nn_recv(socket,(void *)buffer,4000,0);
        //copies the first bytes of the message into header
        memcpy(&header, buffer, sizeof(Header));
        //determines which protocol to run based on the header
        switch(header.messageType){
            //request session id
            case '0':
                outSize = MessageType0Handler((MessageType0 *)buffer, &state);
                //if outSize is a good value, create Type 1 message using the state data and send to client
                if (outSize >= 0)
                {
                    MessageType1 t1 = MessageType1Builder(outSize);

                    //gets the size of the type 1 message including its header
                    outSize += sizeof(char) + sizeof(int);
 
                    //send message
                    nn_send(socket, (void *)t1, outSize, 0);
                    state.lastSent = '1';
                }
                else
                {
                    MessageType2 t2;
                    //create type 2 message with error
                    if (outSize == -1)
                    {
                        t2 = MessageType2Builder("Error: Message type 0 not formatted properly.");
                    }
                    else
                    {
                        t2 = MessageType2Builder("Error: Message type 0 received but connection already established.");
                    }
                    //header size + message size
                    int t2MessageLength = sizeof(char) + sizeof(int) + t2.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t2, t2MessageLength, 0);
                    state.lastSent = '2';
                }
                break;
            //request contents of a file
            case '3':
                //keep checking for file fragments until whole file is received
                outSize = MessageType3Handler((MessageType3 *)buffer, &state);
                
                //if type 3 message is ok, send type 4
                if (outSize >= 0)
                {
                    MessageType4 t4 = MessageType4Builder(state);
                    //header size + message size
                    int t4MessageLength = sizeof(char) + sizeof(int) + t4.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t4, t4MessageLength, 0);
                    state.lastSent = '4';
                }
                //else there is an error with the type 3 message
                else
                {
                    MessageType2 t2 = MessageType2Builder("Error: Unexpected type 3 message or invalid type 3 format.");
                    //header size + message size
                    int t2MessageLength = sizeof(char) + sizeof(int) + t2.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t2, t2MessageLength, 0);
                    state.lastSent = '2';
                }
                break;
            //acknowledge receipt of type 4 message
            case '6':
                outSize = MessageType6Handler((MessageType6 *)buffer, &state);
                if (outSize == 4)
                {
                    MessageType4 t4 = MessageType4Builder(state);
                    //header size + message size
                    int t4MessageLength = sizeof(char) + sizeof(int) + t4.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t4, t4MessageLength, 0);
                    state.lastSent = '4';
                }
                else if (outSize == 5)
                {
                    //build message 5 and close connection
                }
                else
                {
                    MessageType2 t2 = MessageType2Builder("Error: Unexpected type 6 message or invalid type 6 format.");
                    //header size + message size
                    int t2MessageLength = sizeof(char) + sizeof(int) + t2.header.messageLength;
                    //send message
                    nn_send(socket, (void *)t2, t2MessageLength, 0);
                    state.lastSent = '2';
                }
                break;
            //unrecognized message type
            //maybe send a type 2 message?
            default:
                MessageType2 t2 = MessageType2Builder("Error: Unknown message type received.");
                //header size + message size
                int t2MessageLength = sizeof(char) + sizeof(int) + t2.header.messageLength;
                //send message
                nn_send(socket, (void *)t2, t2MessageLength, 0);
                state.lastSent = '2';
                break;
        }
    }
}