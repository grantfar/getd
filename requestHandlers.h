//
// Created by grant on 3/14/19.
//

#ifndef GETD_REQUESTHANDLERS_H
#define GETD_REQUESTHANDLERS_H

#include "requests.h"

int MessageType0Handler(MessageType0 * messageType0, State * state, void * outMessage);
int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage);
int MessageType6Handler(MessageType6 * messageType6, State * state, void * outMessage);
int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage);
#endif //GETD_REQUESTHANDLERS_H
