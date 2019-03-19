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

//establish session between get and getd with a unique session id.
int MessageType0Handler(MessageType0 * messageType0, State * state, void * outMessage)
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
        while (sessionIdLength > 0) {
            size_t index = (double) rand() / RAND_MAX * (sizeof possibleChars - 1);
            state->sessionId[sessionIdLength] = possibleChars[index];
            sessionIdLength = sessionIdLength - 1;
        }
        state->sessionId[129] = '\0';
    }
    return ver;
}

//request from get to receive a file from getd
int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage)
{
    //the requested file to be sent from getd
    static FILE * sendFile;
    //check if message is in proper form
    int ver = type3Ver(messageType3);
    //if getd was not expecting a message of type 3
    if(!((state->lastRecieved == 0 || state->lastRecieved == 3) && (state->lastSent == 1 || state->lastSent == 4) && ver == 0))
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

    //if get makes initial request to receive a file
    if(state->lastRecieved != 3 || state->lastSent != 4)
        sendFile = fopen(messageType3->pathName);
    //if get is expecting a file fragment?
    /*
    //Note:
    //I think this is expecting get to continuously send a type 3 message if it
    //receives a type 4 message from getd, but get should be sending a type 6
    //message in this case rather than type 3 and handled in that function instead.
    //I think it might be worth putting the file I/O in its own function outside of
    //the type 4 handler and maybe using a global variable to handle the file state
    //(such as the file pointer and its seek position) across multiple messages
    //since this program isn't expected to be asynchronous.
    */
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

}

//unrecognized message types
int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage)
{

}