# getd

## Authors

### Grant Farnsworth

Grant created the skeleton of the program and set up the git repository as well as create all the header files. More specifically, Grant wrote most of the main function as well as verification for types 0 and 3 messages, processing of type 3 messages, and the construction of types 4 and 5 messages.

### Joshua Sziede

Joshua wrote the verification for type 6 messages, the processing of types 0 and 6 messages, and the construction of types 1 and 2 messages. Joshua also wrote the documentation of the code, including the comments and the readme file.

## Class

CS-5750-100 - Secure Software Development

## Assignment

Assignment 2 - getd

## Security

### State

In order to maintain information across multiple messages and functions, our implementation of getd uses a custom structure referred to as a State to store information such as which message type was last sent and received, the session id, and the name of the get user. This is important since it gets passed around from function to function to determine if the sender is who they say they are and if getd is receiving a message type that it is expecting, such as expecting to receive a type 6 message after sending a type 4 message.

### Header

Before any message gets processed, first its header is read and verified. If the header's message type is not in range of zero through six then the message is not processed and the user receives an error message. Once the header is verified and contains an acceptable message type, the switch statement from main will determine how to process the contents of the message itself.

### Message Verification

Each message type that can be received (0, 3, and 6) has its own function for verifying the message contents. The verification function for each message type checks the values of the message fields such as messages lengths and message text. The verification for message types 0 and 6 are very similar. They first check that the user name (for type 0) or session id (for type 6) contained in the message is not too long and that they have a null terminator. Furthermore, we check if length of the user name or session id is as long as the message says it is. If any of these checks fail, the message is rejected and the user is returned an error. Likewise, the verification for type 3 messages performs the same type of checks as the other two message types, except it performs the checks twice to verify both the session id and path name are valid.

### Message Processing

The only type of general security found across all the message processing functions is that we use the state variable to determine if the message type that is being processed is a message type that getd is expecting. For example, getd should only process a type 1 message if it last received a type 0 message from get. If an unexpected message type is received, getd will cancel processing the message and return an error message to get.