/*
//CS-5750-100 - Secure Software Development
//Program 2: getd
//Authors:
//  Grant Farnsworth
//  Joshua Sziede
*/
#ifndef REQUESTVER
#define REQUESTVER
#include "requests.h"

#define TP0_LENMATCH 1
#define TP0_DNLEN33P 2
#define TP0_NONULL 4
#define TP0_MESSLEN 8

#define SID_MISMATCH 16

unsigned int type0Ver(MessageType0 * message);
unsigned int type3Ver(MessageType3 * message);
unsigned int type6Ver(MessageType6 * message, State * state);

#endif
