/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "requests.h"
#include "requestHandlers.h"
#include "requestVerify.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>


//the requested file to be sent from getd
static FILE * sendFile;

//establish session between get and getd with a unique session id.
int MessageType0Handler(MessageType0 * messageType0, State * state, void * outMessage)
{
    //check contents of message to ensure it is secure
    int ver = type0Ver(messageType0);
    //the length of the unique session id string
    int sessionIdLength = 127;

    //if the sender has a different session id than what was established
    if (state->lastSent != 5 && state->lastSent != 2)
    {
        return MessageType2Builder(outMessage,"1 session at a time");
    }

    //if message is in proper form
    if (ver != 0){
        state->lastSent = 2;
        return MessageType2Builder(outMessage,"Corrupt Type 0 Message");
    }

    else
    {
        //store the username so we can later check if the requested file belongs to the user
        strcpy(state->userName,messageType0->distinguishedName);
        state->sessionId[0] = '\0';
        //generates random string using the chars found in possibleChars and assigns the string to sessionId
        char possibleChars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        while (sessionIdLength >= 0)
        {
            size_t index =  (size_t)(((double)rand()) / RAND_MAX * (sizeof possibleChars - 1));
            state->sessionId[sessionIdLength] = possibleChars[index];
            sessionIdLength = sessionIdLength - 1;
        }
        state->sessionId[128] = '\0';
        
    }
    return MessageType1Builder(outMessage,state);
}



//request from get to receive a file from getd
int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage)
{
    //check if message is in proper form
    if(type3Ver(messageType3)) {
        state->lastSent = 2;
        return MessageType2Builder(outMessage, "Corrupt Type 3 Message");
    }

    //if the sender has a different session id than what was established
    if(strncmp(messageType3->sessionId,state->sessionId,128)!=0) {
        state->lastSent = 2;
        return MessageType2Builder(outMessage, "One session at a time");
    }

    //if getd was not expecting a message of type 3
    if(state->lastSent != 1)
    {
        state->lastSent = 2;
        return MessageType2Builder(outMessage,"Invalid State");
    }

    //attempts to open the file, otherwise returns error
    sendFile = fopen(messageType3->pathName, "r");
    if (sendFile == NULL)
    {
        char error[100];
        state->lastSent = 2;
        sprintf(error,"Error: opening file %s",strerror(errno));
        return MessageType2Builder(outMessage,"Error Opening File");
    }

    //checks if file is regular file and not symlink, dir, etc.
    struct stat * openStat = malloc(sizeof(struct stat));
    fstat(fileno(sendFile),openStat);
    if (!S_ISREG(openStat->st_mode))
    {
        free(openStat);
        state->lastSent = 2;
        return MessageType2Builder(outMessage,"Invalid File Type");
    }

    free(openStat);

    return MessegeType4Builder(outMessage,state);
}


//ackowledgement from get that it received a type 4 message from getd
int MessageType6Handler(MessageType6 * messageType6, State * state, void * outMessage)
{
    if(strcmp(messageType6->sessionId,state->sessionId)!=0)
    {
        return MessageType2Builder(outMessage,"One session at a time");
    }

    state->lastRecieved = 6;

    int ver = type6Ver(messageType6, state);


    if (ver != 0)
    {
        state->lastSent = 2;
        return MessageType2Builder(outMessage,"Corrupt Type 6 Messege");
    }

    //if get is still expecting file data
    if (state->lastSent == 4)
    {
        if (feof(sendFile))
            return MessegeType5Builder(outMessage,state);
        else
            return MessegeType4Builder(outMessage,state);
    }
    //if get has received the whole file contents
    else if ( state->lastSent != 5)
    {
        state->lastSent = 2;
        return MessageType2Builder(outMessage,"Invalid State");
    }
    return 0;
}

//unrecognized message types
int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage)
{
    char error [257];
    sprintf(error,"Invalid Message Type: \"%u\"",(unsigned int)type);
    return MessageType2Builder(outMessage,error);
}

//builds a type 4 message, which copies data from a file and send that data to get
int MessegeType4Builder(MessageType4 * out,State * state){
    state->lastSent = 4;
    out->header.messageType = 4;

    //copies data from the input file
    out->contentLength = fread(out->contentBuffer,1,4096,sendFile);
    //limit the input buffer to 4096 bytes
    if (out->contentLength != 4096)
    {
        getc(sendFile);
    }

    //populates the type 4 message with its other needed data unrelated to the file data
    strcpy(out->sessionId,state->sessionId);
    out->sidLength = strlen(out->sessionId);
    out->header.messageLength = sizeof(MessageType4);

    return out->header.messageLength;
}

//type 5 message terminates the connection to get
int MessegeType5Builder(MessageType5 * out,State * state)
{
    //closes the file since it has been successfully copied
    fclose(sendFile);

    state->lastSent = 5;

    //populate the message data
    out->header.messageType = 5;
    strcpy(out->sessionId,state->sessionId);
    out->sidLength = strlen(out->sessionId);
    out->header.messageLength = sizeof(MessageType5);

    return out->header.messageLength;
}

//builds a type 2 error message given a string containing the error message
int MessageType2Builder(MessageType2 * messageType2,char *errorMsg)
{
    //gets the length of the error message
    int errorMsgLen = strlen(errorMsg);

    //message header
    messageType2->header.messageLength = sizeof(MessageType2);
    messageType2->header.messageType = 2;

    //message data
    messageType2->msgLength = errorMsgLen;
    strncpy(messageType2->errorMessage, errorMsg, errorMsgLen);
    messageType2->errorMessage[errorMsgLen + 1] = '\0';

    return messageType2->header.messageLength;
}

//type 1 message establishes a session id between get and getd
int MessageType1Builder(MessageType1 * out, State * state)
{
    out->header.messageType = 1;
    strcpy(out->sessionId, state->sessionId);
    out->sidLength = strlen(state->sessionId);
    out->header.messageLength = sizeof(MessageType1);
    state->lastSent = 1;
    return out->header.messageLength;
}