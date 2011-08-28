#ifndef __SERIAL_H__
#define __SERIAL_H__

void SerialInit(void);
void SerialOutputByte(const char c);
int SerialInputByte(char *c);
int SerialIsReadyChar(void);
char SerialIsGetChar(void);

#endif 

