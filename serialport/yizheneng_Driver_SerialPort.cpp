#include "yizheneng_Driver_SerialPort.h"
#include <mutex>
#include "serial.h"
using namespace std; 

class SerialPort
{
public:
    SerialPort();
    ~SerialPort();

    bool open(std::string portName, int baud);

    int sendData(uint8_t* data, int16_t length);

    int readData(uint8_t* dataBuf, int16_t bufSize);

    void close();

    bool isOpened();

    std::string getError() {
        std::lock_guard<std::mutex> lk(serialPortErrorStringMutex);
        return errorString;
    }

private:
    void setError(std::string msg) {
        std::lock_guard<std::mutex> lk(serialPortErrorStringMutex);
        errorString = msg;
    }

    serial::Serial serial;
    std::string errorString;

    std::mutex serialPortMutex;
    std::mutex serialPortErrorStringMutex;
};

SerialPort::SerialPort(/* args */)
{
}

SerialPort::~SerialPort()
{
}

bool SerialPort::open(std::string portName, int baud)
{
  //LOG_INFO("Serial port:%s", portName.c_str());

  std::lock_guard<std::mutex> lk(serialPortMutex);

  if(serial.isOpen()) {
    serial.close();
  }

  try {
      serial.setPort(portName.c_str());
      serial.setBaudrate(baud);
      serial::Timeout to = serial::Timeout::simpleTimeout(5);
      serial.setTimeout(to);
      serial.open();
  } catch(serial::IOException& e) {
	  //LOG_ERROR("Open serial port exception:%s", e.what());
	  setError(e.what());
      return false;
  }

  if(!serial.isOpen()) {
	  //LOG_ERROR("Open serial port error!!");
      return false;
  } else {
	  //LOG_INFO("Open serial port succeed!!");
  }

  return true;
}

int SerialPort::sendData(uint8_t* data, int16_t length)
{
    serialPortMutex.lock();
    try {
        int len = serial.write((uint8_t*)data, length);
        serialPortMutex.unlock();
        return len;
    } catch(serial::PortNotOpenedException& e) {
		//LOG_ERROR("Send data error, serial not opened!");
		setError(e.what());
    } catch(serial::IOException& e) {
        //LOG_ERROR("Send data error, serial port closed!");
        setError(e.what());
    } catch (serial::SerialException& e) {
        //LOG_ERROR("Send data error, serial port disconnected, %s", e.what());
        setError(e.what());
    }
    serialPortMutex.unlock();
    close();
    return 0;
}

int SerialPort::readData(uint8_t* dataBuf, int16_t bufSize)
{
    serialPortMutex.lock();
    try {
        int len = serial.read(dataBuf, bufSize);
        serialPortMutex.unlock();
        return len;
    } catch(serial::IOException& e) {
        //LOG_ERROR("Receive data error, serial port closed, %s", e.what());
        setError(e.what());
    } catch (serial::SerialException& e) {
        //LOG_ERROR("Receive data error, serial port disconnected, %s", e.what());
        setError(e.what());
        return 0;
    }
    serialPortMutex.unlock();
    close();
    return 0;
}

void SerialPort::close()
{
    std::lock_guard<std::mutex> lk(serialPortMutex);
    serial.close();
}

bool SerialPort::isOpened()
{
    std::lock_guard<std::mutex> lk(serialPortMutex);
    return serial.isOpen();
}

char* listSerialPorts()
{
    char* buf = (char*)malloc(500);
    std::vector<serial::PortInfo> portsInfo = serial::list_ports();
    string temp;
    for(int i = 0; i < portsInfo.size(); i++) {
        if(i) {
            temp = temp + "||";
        }
        temp = temp + portsInfo[i].port;
    }

    if(temp.size() > 500) {
        buf[0] = '\0';
        return buf;
    }

    strcpy(buf, temp.c_str());
    return buf;
}

uint8_t openSerialPort(char* portName, int baud, void* pointer)
{
    return ((SerialPort*)pointer)->open(string(portName), baud);
}

uint8_t isSerialPortOpened(void* pointer)
{
    return ((SerialPort*)pointer)->isOpened();
}

void closeSerialPort(void* pointer)
{
    ((SerialPort*)pointer)->close();
}

int readDataFromSerialPort(void *dataBuf, int bufSize, void* pointer)
{
    return ((SerialPort*)pointer)->readData((uint8_t*)dataBuf, bufSize);
}

void sendToSerialPort(void *dataBuf, int length, void* pointer)
{
    ((SerialPort*)pointer)->sendData((uint8_t*)dataBuf, length);
} 

char* getSerialPortError(void* pointer)
{
    std::string error = ((SerialPort*)pointer)->getError();
    char* p = (char*)malloc(error.size());
    strcpy(p, error.c_str());
    return p;
}

void* newSerialPort()
{
    return new SerialPort();
}

void deleteSerialPort(void* pointer)
{
    delete ((SerialPort*)pointer);
}