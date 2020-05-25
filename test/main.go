package main

import (
	"log"
	"time"

	"github.com/yizheneng/GoSerialPortDriver/serialport"
)

func main() {
	ports := serialport.ListPorts()
	log.Printf("%v", ports)

	serial := serialport.NewSerialPort("/dev/ttyUSB0", 115200)
	if serial.Open() {
		log.Printf("Open succeed!")
	} else {
		log.Printf("Open error:%v", serial.GetError())
	}

	for {
		serial.SendString("Ni hao a")
		time.Sleep(time.Millisecond)
		log.Printf("%v", serial.ReadString())
		// log.Printf("%v", len(serial.Read()))
	}
}
