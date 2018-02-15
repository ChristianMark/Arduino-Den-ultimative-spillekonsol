import smbus
import time
from functionStorage import getLines
from functionStorage import printScores
from functionStorage import sortScores
from functionStorage import writeToFile
from functionStorage import removeDuplicates

#testing scenario 1 for game Snake - RPi as master, Arduino as slave
# Assuming RPi keeps a local total record of top scores in .txt file
# Showing that the records can be:
# - extracted of the .txt file
# - sorted by score
# - removed of duplicates, but keeps the same name but differen score 

bus = smbus.SMBus(1)         #set the bus up
slaveAddress = 0x04          #setup slaves address
records = getLines("scores.txt") #get scores and names as 2d list

print("Scores NOT sorted:")  
printScores(records)             #show records to start with, be aware of duplicates
print("")

records = sortScores(records)        #sort records by score
print("Scores sorted:")      
printScores(records)             #show records to records after sorting, still duplicates

records = removeDuplicates(records)  #remove only duplicates where scores and names are same
print("\nScores sorted NO dublicates:")
printScores(records)             #show final records after sorting, removing duplicates

writeToFile('scores.txt',records, 54, "Kal9") #add custom test record to .txt file 

def writeInt(value):
    bus.write_byte(slaveAddress, value) #use smbus library to write int to slave
    return -1

def readInt():
    byte = bus.read_byte(slaveAddress) #use smbus library to read int from slave
    return byte
    
'''
#initial test if Arduino responds via TWI
while True:
    var = input("Enter 1 - 9: ") #get command to be send Arduino
    if not var:
        continue
    writeInt(int(var))   #send a byte to Arduino                  
    print("RPI sent Arduino number: ", var)
    time.sleep(1)

    var = readInt()
    print("Arduino sent number : ", var)
    print()
'''
