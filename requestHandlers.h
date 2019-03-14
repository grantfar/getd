//
// Created by grant on 3/14/19.
//

#ifndef GETD_REQUESTHANDLERS_H
#define GETD_REQUESTHANDLERS_H

#include "requests.h"

int MessageType0Handler(MessageType0 * messageType0, char * sessionId, void * outMessage);
int MessageType3Handler(MessageType1 * messageType1, char * sessionID, void * outMessage);
int MessageType6Handler(MessageType1 * messageType1, char * sessionID, void * outMessage);
int MessageOtherHandler(char * message, unsigned char type ,char * sessionID, void * outMessage);
#endif //GETD_REQUESTHANDLERS_H
