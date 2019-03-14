//
// Created by grant on 3/12/19.
//
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

void handleQuit(int sig){
    free(outMessage);
    nn_shutdown(socket,bind);
    exit(0);
}

int main(int argc, const char * argv[])
{
    signal(SIGINT ,handleQuit);
    signal(SIGQUIT ,handleQuit);
    signal(SIGTERM ,handleQuit);
    
    char buffer [4000];
    Header header;
    State state;
    state.lastRecieved=6;
    state.lastSent = 5;
    socket = nn_socket(AF_SP,NN_PAIR);
    bind = nn_bind(socket,"ipc:///tmp/getd.ipc");
    unsigned int bytesRecieved;
    unsigned int outSize;
    outMessage = malloc(10000);

    while (-1){
        bytesRecieved = nn_recv(socket,(void *)buffer,4000,0);
        memcpy(&header,buffer, sizeof(Header));
        switch(header.messageType){
            case 0:
                outSize = MessageType0Handler((MessageType0 *)buffer,&state,outMessage);
                break;
            case 3:
                outSize = MessageType3Handler((MessageType3 *)buffer,&state,outMessage);
                break;
            case 6:
                outSize = MessageType6Handler((MessageType6 *)buffer,&state,outMessage);
                break;
            default:
                outSize = MessageOtherHandler(buffer,header.messageType,&state,outMessage);
                break;
        }

        nn_send(socket,outMessage,outSize,0);
        state.lastSent = ((Header*)outMessage)->messageType;
    }
}