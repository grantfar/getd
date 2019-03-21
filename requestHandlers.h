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

int MessageType0Handler(MessageType0 * messageType0, State * state);
int MessageType3Handler(MessageType3 * messageType3, State * state);
int MessageType6Handler(MessageType6 * messageType6, State * state);
MessageType1 MessageType1Builder(int messageSize);
MessageType2 MessageType2Builder(char * errorMsg);
MessageType4 MessageType4Builder(State * state);
MessageType5 MessageType5Builder(State * state);
#endif //GETD_REQUESTHANDLERS_H
