#ifndef REQUESTS
#define REQUESTS

typedef struct _header {
    unsigned char messageType;
    unsigned int messageLength;
} Header;

typedef struct _type0 {
    Header header;
    unsigned int dnLength;
    char distinguishedName[33];
} MessageType0;

typedef struct _type1 {
    Header header;
    unsigned int sidLength;
    char sessionId[129];
} MessageType1;

typedef struct _type2 {
    Header header;
    unsigned int msgLength;
    char errorMessage[257];
} MessageType2;

typedef struct _type3 {
    Header header;
    unsigned int sidLength;
    unsigned int pathLength;
    char sessionId[129];
    char pathName[4097];
} MessageType3;

typedef struct _type4 {
    Header header;
    unsigned int sidLength;
    unsigned int contentLength;
    char sessionId[129];
    char contentBuffer[4096];
} MessageType4;

typedef struct _type5 {
    Header header;
    unsigned int sidLength;
    char sessionId[129];
} MessageType5;

typedef struct _type6 {
    Header header;
    unsigned int sidLength;
    char sessionId[129];
} MessageType6;

#endif