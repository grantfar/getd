//
// Created by grant on 3/14/19.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "requestHandlers.h"
#include "requestVerify.h"
#include <string.h>

int MessageType0Handler(MessageType0 * messageType0, State * state, void * outMessage)
{

}

int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage)
{
    static FILE * sendFile;
    int ver = type3Ver(messageType3);
    if(!((state->lastRecieved == 0 || state->lastRecieved == 3) && (state->lastSent == 1 || state->lastSent == 4) && ver == 0))
    {
        MessageType2 * OutMessage = outMessage;
        OutMessage->header.messageType = 2;
        OutMessage->header.messageLength = sizeof(MessageType2) - sizeof(Header);
        OutMessage->errorMessage[0] = '\0';
        if(!((state->lastRecieved == 0 || state->lastRecieved == 3) && (state->lastSent == 1 || state->lastSent == 4)))
            strcpy(OutMessage->errorMessage,"invalid state");
    }

    if(state->lastRecieved != 3 || state->lastSent != 4)
        sendFile = fopen(messageType3->pathName);
    if(state->lastRecieved == 3 && state->lastSent == 4)
    {
        if(feof(sendFile)){
            fclose(sendFile);
        }
        else{
            MessageType4 * OutMessege = outMessage;
            OutMessege->header.messageType = 4;
            OutMessege->header.messageLength = sizeof(MessageType4) - sizeof(Header);
            OutMessege->contentLength = fread(OutMessege->contentBuffer,4096,1,sendFile);
        }
    }
}

int MessageType6Handler(MessageType6 * messageType6, State * state, void * outMessage)
{

}

int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage)
{

}