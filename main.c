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

int main(int argc, const char * argv[])
{
    char buffer [4000];
    Header header;
    int socket = nn_socket(AF_SP,NN_PAIR);
    int bind = nn_bind(socket,"ipc:///tmp/getd.ipc");
    unsigned int bytesRecieved;
    unsigned int outSize;
    char sessionId[129];
    void * outMessage = malloc(10000);

    while (-1){
        bytesRecieved = nn_recv(socket,(void *)buffer,4000,0);
        memcpy(&header,buffer, sizeof(Header));
        switch(header.messageType){
            case 0:
                outSize = MessageType0Handler((MessageType0 *)buffer,sessionId,outMessage);
                break;
            case 3:
                outSize = MessageType3Handler((MessageType3 *)buffer,sessionId,outMessage);
                break;
            case 6:
                outSize = MessageType6Handler((MessageType6 *)buffer,sessionId,outMessage);
                break;
            default:
                outSize = MessageOtherHandler(buffer,header.messageType,sessionId,outMessage);
                break;
        }

        nn_send(socket,outMessage,outSize,0);
    }
}