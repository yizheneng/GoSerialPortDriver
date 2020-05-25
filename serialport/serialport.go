package serialport

/* #cgo CXXFLAGS: -std=c++11
#include <stdlib.h>
#include "yizheneng_Driver_SerialPort.h"
*/
import "C"

import (
	"runtime"
	"strings"
	"unsafe"
)

type Serial struct {
	cPointer unsafe.Pointer
	portName string
	buad     C.int
}

func serialGoUnpackPorts(str string) []string {
	return strings.Split(str, "||")
}

// ListPorts is get ports
func ListPorts() []string {
	portString := C.listSerialPorts()
	C.free(unsafe.Pointer(portString))
	return serialGoUnpackPorts(C.GoString(portString))
}

func NewSerialPort(portName string, buad int) *Serial {
	p := new(Serial)
	p.cPointer = C.newSerialPort()
	p.buad = C.int(buad)
	p.portName = portName

	runtime.SetFinalizer(p, p.deleteCPointer)
	return p
}

func (p *Serial) Open() bool {
	cPortName := C.CString(p.portName)
	defer C.free(unsafe.Pointer(cPortName))
	return C.openSerialPort(cPortName, p.buad, p.cPointer) >= 0
}

func (p *Serial) IsOpened() bool {
	return C.isSerialPortOpened(p.cPointer) > 0
}

func (p *Serial) Close() {
	C.closeSerialPort(p.cPointer)
}

func (p *Serial) Read() []byte {
	var buf [500]byte
	size := C.readDataFromSerialPort(unsafe.Pointer(&buf[0]), C.int(len(buf)), p.cPointer)
	return []byte(buf[:size])
}

func (p *Serial) ReadString() string {
	return string(p.Read())
}

func (p *Serial) Send(data []byte) {
	C.sendToSerialPort(unsafe.Pointer(&data[0]), C.int(len(data)), p.cPointer)
}

func (p *Serial) SendString(data string) {
	p.Send([]byte(data))
}

func (p *Serial) GetError() string {
	errorString := C.getSerialPortError(p.cPointer)
	C.free(unsafe.Pointer(errorString))
	return C.GoString(errorString)
}

func (p *Serial) deleteCPointer(pointer *Serial) {
	C.deleteSerialPort(p.cPointer)
}
