import smbus
import time
import RPi.GPIO as GPIO

from functionStorage import addOneScore
from functionStorage import uploadFileToWeb

GPIO.setmode(GPIO.BCM)    #use BCM
GPIO.setup(21,GPIO.IN)    #use pin 21 as signal from slave to request data
bus = smbus.SMBus(1)      #activate the two way interface bus
slaveAddress = 0x04       #the address of the slave

def writeByte(value):
    bus.write_byte(slaveAddress, value)    
    return -1

def readByte():
    value = bus.read_byte(slaveAddress)
    return value

name = ""

while True:  
    while not GPIO.input(21):               #check if slave signals 
        time.sleep(0.1)                     #delay
        print("wait for slave to call")
    print("slave to called")    
    
    for n in range(0,5):                    #request name of 5 chars
        name = name + str( chr(readByte()) )
        time.sleep(0.1)                     #enable delay during debug on arduino
        
    print("Recieved name from Arduino: {}".format(name))
    addOneScore(name, "scores_v2.txt")
    uploadFileToWeb("scores_v2.txt",'files.000webhost.com','marksauer','V1IpWadgMOD',"public_html")
    time.sleep(2)
    name = ""    
