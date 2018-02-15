import smbus
import time
from functionStorage import getLines
from functionStorage import printScores
from functionStorage import sortScores
from functionStorage import writeToFile
from functionStorage import removeDuplicates

#testing scenario 1.1 for game Snake - RPi as master, Arduino as slave
# Assuming RPi keeps a local total record of top scores in .txt file
# Showing that the records can be:
# - extracted of the .txt file
# - sorted by score
# - removed of duplicates, but keeps the same name but differen score
# in addition
# - request 5 records by sending index over the bus
# for each record and for each char

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

def writeByte(value):
    bus.write_byte(slaveAddress, value) #use smbus library to write int to slave
    return -1

def readByte():
    byte = bus.read_byte(slaveAddress) #use smbus library to read int from slave
    return byte

numberOfRecords = 5
records = [["0","none"] for _ in range(int(numberOfRecords))] #empty list
while True:       
    for i in range(0,5): # 5 records
        score = ""
        name = ""
        for n in range(0,9): # 9 chars
            if n < 3:
                writeByte(n)
                time.sleep(0.1)
                score = score + str( chr(readByte()) )
            elif n == 3:
                continue
            elif n > 3:
                writeByte(n)
                time.sleep(0.1)
                name = name + str( chr(readByte()) )
        records[i][0] = score
        records[i][1] = name.rstrip()
        print("Recieved records from Arduino: {}".format(i+1))
    print("Arduino send 5 records:")
    printScores(records)
    break
    

    '''
    command = input("Enter 1 - 9: ") #get command to be send Arduino
    if not command:
        continue
    writeByte(int(command))   #send a byte to Arduino                  
    print("RPI sent Arduino number: ", command)
    time.sleep(1)
    '''

    '''
    if command == 1:
        numberOfRecords = readNumber()
        print("Arduino: Hej RPI, I send you {} records\n.".format(numberOfRecords))
    elif command == 2:
        char = readNumber()
        print("Arduino: Hej RPI, I send you {} records\n.".format(char))
    elif command == 3:
        records = [["0","none"] for _ in range(int(numberOfRecords))] #empty list
        if numberOfRecords > 0:
            for i in range(0,numberOfRecords):
                score = ""
                name = ""
                for r in range(0, numberOfBytes):
                    char = readNumber()
                    print("Arduino: Hej RPI, I send you {} records\n.".format(char))
                    if r < 3:
                        score += str(char)
                    elif str(char) != '\0':
                        name += str(char)
                    elif str(char) == '\0':
                        name += str(char)
                        records[i][1] = name
                        records[i][0] = score
                        break
                    #if no end of string char was send just add the record 
                    records[i][1] = name
                    records[i][0] = score
        print("Arduino: Hej RPI, some scores:\n")
        printScores(records)                       
    '''
