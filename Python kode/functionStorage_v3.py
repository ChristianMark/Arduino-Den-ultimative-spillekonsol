#bubble sort function, gets 2d list,
#where comulm 0 is a string representing the score
#and   column 1 is a string representing the name
def sortScores(listToSort):
    count = len(listToSort) #get number of row in the 2d list
    if count > 1:    #if more than one row, it might need sorting 
        i = 0
        while (i < count-1):            
            if int(listToSort[i][0]) < int(listToSort[i+1][0]):
                tempScore = listToSort[i][0]
                listToSort[i][0] = listToSort[i+1][0]
                listToSort[i+1][0] = tempScore

                tempName = listToSort[i][1]
                listToSort[i][1] = listToSort[i+1][1]
                listToSort[i+1][1] = tempName

            if i > 0:
                j = i
                while j > 0:
                    if int(listToSort[j-1][0]) < int(listToSort[j][0]) :
                        tempScore = listToSort[j-1][0]
                        listToSort[j-1][0] = listToSort[j][0]
                        listToSort[j][0] = tempScore

                        tempName = listToSort[j-1][1]
                        listToSort[j-1][1] = listToSort[j][1]
                        listToSort[j][1] = tempName
                    j -= 1
            i += 1
    return listToSort

# prerequest: 2d list should be sorted
def removeDuplicates(sortedList):
    count = int(len(sortedList))
    countToRemove = 0
    if count > 1:
        i = 0
        while (i < count-1):
            currentScore = sortedList[i][0]
            nextScore = sortedList[i+1][0]
            currentName = sortedList[i][1]
            nextName = sortedList[i+1][1]
            
            if currentScore == nextScore and currentName == nextName:
                sortedList[i][0] = "" #mark to be removed by setting empthy string
                sortedList[i][1] = ""
                countToRemove += 1
            i += 1

    #create the new list with less rows 
    countNew = count - countToRemove
    newList = [["0","none"] for _ in range(countNew)]
    if countNew > 0:
        iNew = 0
        i = 0
        while (i < count):
            score = sortedList[i][0]            
            if score: #if score string is not empty add record to new list
                newList[iNew][0] = sortedList[i][0]
                newList[iNew][1] = sortedList[i][1]
                iNew += 1
            i += 1

    return newList

def getLines(source):
    #get number of records, assumming not empty
    linesCount = sum(1 for line in open(str(source)))
    
    readFile = open(str(source),"r") #open file for reading

    r = 0    
    lines = [["0","none"] for _ in range(int(linesCount))]

    #first line
    line = readFile.readline()
    count = 0
    while count < linesCount:
        #get rid of whitespace
        line.rstrip()
        #gets list of strings originally separated by " "
        strings = line.split(" ") 
        for string in strings:
            if string.isdigit(): #be sure sting is made of digits
                lines[r][0] = string.strip()
            elif string:
                lines[r][1] = string.strip()        

        r += 1; #get the index for the new row
        line = readFile.readline() #get new line  
        count += 1 #keep rolling         
    readFile.close() 
    return lines

#appends one record to file,
# ignore the "lst" argument it is from TODO
def writeToFile(fileName, lst, score, name):
    appendFile = open(str(fileName),'a')
    appendFile.write("{} {}\n".format(score, name))
    appendFile.close()

def printScores(records):
    for row in records:
        print("{0} {1}".format(row[0],row[1]))
