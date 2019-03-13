#include "requestVerify.h"
#include <string.h>



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

unsigned int type3Ver(MessageType3 * message)
{

}

unsigned int type6Ver(MessageType6 * message)
{

}