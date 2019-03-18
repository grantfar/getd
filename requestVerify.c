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
    unsigned int retval = 0;
    unsigned char endIndex = 0;
    while((message->distinguishedName)[0] != '\0' && endIndex < 33)
        endIndex++;
    if(message->dnLength != endIndex)
        retval += TP0_LENMATCH;
    if(message->dnLength > 32 )
        retval += TP0_DNLEN33P;
    if(endIndex == 33)
        retval += TP0_NONULL;
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