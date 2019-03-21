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
int MessageType0Handler(MessageType0 * messageType0, State * state)
{
    //check contents of message to ensure it is secure
    int ver = type0Ver(messageType0);
    //the length of the unique session id string
    int sessionIdLength = 128;
    //if the message is in proper form
    if (ver == 0)
    {
        //store the username so we can later check if the requested file belongs to the user
        state->userName = messageType0->distinguishedName;
        state->sessionId = malloc(sizeof(char) * 129);
        //generates random string using the chars found in possibleChars and assigns the string to sessionId
        char possibleChars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        while (sessionIdLength > 0)
        {
            size_t index = (double) rand() / RAND_MAX * (sizeof possibleChars - 1);
            state->sessionId[sessionIdLength] = possibleChars[index];
            sessionIdLength = sessionIdLength - 1;
        }
        state->sessionId[129] = '\0';
        return (sizeof(int) + (sizeof(char) * 129));
    }
    //could return different negative value based on ver 
    else
    {
        return -1;
    }
    
}



//request from get to receive a file from getd
int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage)
{
    //check if message is in proper form
    int ver = type3Ver(messageType3);
    //if getd was not expecting a message of type 3
    if(state->lastRecieved == 0  || state->lastSent == 1 || ver == 0)
    {
        //send error message
        /*
        //Note:
        //It might be better to have a separate function for creating
        //Type 2 messages rather than remaking it for every message
        //function. It can be called from main() whenever a handler
        //function returns a bad value.
        */
        MessageType2 * OutMessage = outMessage;
        OutMessage->header.messageType = 2;
        OutMessage->header.messageLength = sizeof(MessageType2) - sizeof(Header);
        OutMessage->errorMessage[0] = '\0';
        if(!((state->lastRecieved == 0 || state->lastRecieved == 3) && (state->lastSent == 1 || state->lastSent == 4)))
            strcpy(OutMessage->errorMessage,"invalid state");

    }

        sendFile = fopen(messageType3->pathName, "R");
        if(sendFile == NULL) {
            char errorMsg[257];
            sprintf(errorMsg,"Failed to open file: %s",strerror(errno));
            MessageType2 errorM = MessageType2Builder(errorMsg);
            memcpy(outMessage,&errorM, sizeof(MessageType2));
            return sizeof(MessageType2);
        }
        struct stat * openStat = malloc(sizeof(struct stat));

        fstat(fileno(sendFile),openStat);
        //Checks for if file is regular file
        if(!S_ISREG(openStat->st_mode))
        {
            MessageType2 errorM = MessageType2Builder("Invalid File Type");
            memcpy(outMessage,&errorM, sizeof(MessageType2));
            free(openStat);
            return sizeof(MessageType2);
        }
        free(openStat);
    }

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

//ackowledgement from get that it received a type 4 message from getd
int MessageType6Handler(MessageType6 * messageType6, State * state, void * outMessage)
{
    int ver = type6Ver(messageType6, state);
    //type 6 messages are acks of types 4 and 5 messages
    if(ver != 0){
        MessageType2 errorM = MessageType2Builder("Corrupt Type 2 Messege");
        memcpy(outMessage,&errorM, sizeof(MessageType2));
        return sizeof(MessageType2);
    }
    if(state->lastSent == 4){

    }
    else if(state->lastSent == 5){

    }

    else{

    }
    
}

//unrecognized message types
int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage)
{
    char error [257];
    sprintf(error,"Invalid Message Type: \"%u\"",(unsigned int)type);
    MessageType2 errorM = MessageType2Builder(error);
    memcpy(outMessage,&errorM, sizeof(MessageType2));
    return sizeof(MessageType2);
}


void MessegeType4Builder(MessageType4 * out){
    out->header.messageType = 4;
    out->header.messageLength = sizeof(MessageType4) - sizeof(Header);
    out->contentLength = fread(out->contentBuffer,4096,1,sendFile);
}

//builds a type 2 error message given a string containing the error message
MessageType2 MessageType2Builder(char *errorMsg)
{
    //gets the length of the error message
    int errorMsgLen = strlen(errorMsg);
    //message header
    Header t2Head;
    t2Head.messageLength = errorMsgLen + sizeof(int);
    t2Head.messageType = '2';
    MessageType2 t2;
    t2.header = t2Head;
    //i am unsure if message length includes the null terminator or not
    t2.msgLength = errorMsgLen;
    strncpy(t2.errorMessage, errorMsg, errorMsgLen);
    t2.errorMessage[errorMsgLen + 1] = '\0';
    return t2;
}
