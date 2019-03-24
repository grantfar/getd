/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#ifndef GETD_REQUESTHANDLERS_H
#define GETD_REQUESTHANDLERS_H

#include "requests.h"

int MessageType0Handler(MessageType0 * messageType0, State * state, void * outMessage);
int MessageType2Handler(MessageType2 * messageType2);
int MessageType3Handler(MessageType3 * messageType3, State * state, void * outMessage);
int MessageType6Handler(MessageType6 * messageType6, State * state, void * outMessage);
int MessageOtherHandler(char * message, unsigned char type , State * state, void * outMessage);
int MessageType2Builder(MessageType2 * messageType2,char *errorMsg);
int MessegeType4Builder(MessageType4 * out,State * state);
int MessegeType5Builder(MessageType5 * out,State * state);
int MessageType1Builder(MessageType1 * out, State * state);
#endif //GETD_REQUESTHANDLERS_H
