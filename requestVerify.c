/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#include "requestVerify.h"
#include <string.h>


//checks messages of type 0 for issues
unsigned int type0Ver(MessageType0 * message)
{
    //return value that determines which errors are encountered
    unsigned int retval = 0;
    //index of the last non null character in the string
    unsigned char endIndex = 0;
    //calculate endIndex
    while((message->distinguishedName)[0] != '\0' && endIndex < 33)
        endIndex++;
    //if the type 0 distinguished name length field is not the actual length of the distinguished name
    if(message->dnLength != endIndex)
        retval += TP0_LENMATCH;
    //if the distinguished name length field is too large
    if(message->dnLength > 32 )
        retval += TP0_DNLEN33P;
    //if the actual distinguished name string is too many characters or doesn't have a null terminator
    if(endIndex == 33)
        retval += TP0_NONULL;
    //if the message length is not what the header describes it as
    if(message->header.messageLength != sizeof(MessageType0) - sizeof(Header))
        retval += TP0_MESSLEN;
    return retval;
}

//checks messages of type 3 for issues
unsigned int type3Ver(MessageType3 * message)
{

}

//checks messages of type 6 for issues
unsigned int type6Ver(MessageType6 * message)
{

}