#ifndef _YIZHENNEG_DRIVER_SERIALPOR_
#define _YIZHENNEG_DRIVER_SERIALPOR_
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
char* listSerialPorts();

uint8_t openSerialPort(char* portName, int baud, void* pointer);

uint8_t isSerialPortOpened(void* pointer);

void closeSerialPort(void* pointer);

int readDataFromSerialPort(void *dataBuf, int bufSize, void* pointer);

void sendToSerialPort(void *dataBuf, int length, void* pointer);

char* getSerialPortError(void* pointer);

void* newSerialPort();

void deleteSerialPort(void* pointer);
#ifdef __cplusplus
}
#endif
#endif

