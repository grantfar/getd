/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#include "requestVerify.h"
#include <string.h>
#include <stdio.h>

//checks messages of type 0 for issues
unsigned int type0Ver(MessageType0 * message)
{
    //return value that determines which errors are encountered
    unsigned int retval = 0;
    //calculate endIndex
    size_t reallen = strnlen(message->distinguishedName,32);
    if(reallen != (message->dnLength))
        retval += TP0_LENMATCH;
    //if the distinguished name length field is too large
    if(message->dnLength > 32 )
        retval += TP0_DNLEN33P;
    //if the message length is not what the header describes it as
    if(message->header.messageLength != sizeof(MessageType0))
        retval += TP0_MESSLEN;
    return retval;
}

//checks messages of type 3 for issues
unsigned int type3Ver(MessageType3 * message)
{
    //if header's message length is inconsistent with actual message length
    if (message->header.messageLength != sizeof(MessageType3))
        return 1;
    //if session id length is not actual length
    if (strnlen(message->sessionId,128) != message->sidLength)
        return 1;
    //if path length is not actual length
    if (strnlen(message->pathName,4096) != message->pathLength)
        return 1;
    return 0;
}

//checks messages of type 6 for issues
//note the type 0 constants can apply to type 6 messages as well
unsigned int type6Ver(MessageType6 * message, State * state)
{
    //return value that determines which errors are encountered
    unsigned int retval = 0;
    //index of the last non null character in the string
    unsigned char endIndex = 0;

    //if session id length is not actual length
    if(strnlen(message->sessionId,128) != message->sidLength)
        retval+= TP0_DNLEN33P;

    //if the session id from type 6 is not the same as the session id stored in the state
    if (strncmp(state->sessionId, message->sessionId, message->sidLength) != 0) {
        retval += SID_MISMATCH;
    }

    //calculate endIndex
    while((message->sessionId)[endIndex] != '\0' && endIndex < 129)
        endIndex++;
    //if the type 6 session id length field is not the actual length of the session id
    if(message->sidLength != endIndex)
        retval += TP0_LENMATCH;
    //if the session id length field is too large
    if(message->sidLength > 128 )
        retval += TP0_DNLEN33P;
    //if the actual session id string is too many characters or doesn't have a null terminator
    if(endIndex == 129)
        retval += TP0_NONULL;
    //if the message length is not what the header describes it as
    if(message->header.messageLength != sizeof(MessageType6))
        retval += TP0_MESSLEN;
    return retval;
}