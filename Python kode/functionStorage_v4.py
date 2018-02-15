import ftplib

def uploadFileToWeb(filename, server, user, password, subdir):
    session = ftplib.FTP(str(server),str(user),str(password)) #open ftp session
    session.cwd(str(subdir))            #define subdirectory on ftp
    file = open(str(filename),'rb')     #open file in read binary mode
    query = "STOR " + filename          #create override query                        
    session.storbinary(query, file)     #execute query
    file.close()                    
    session.quit()
    print("File was updated via FTP(www.marksauer.dk)\n")

def addOneScore(name, filename):
    maxScoreReached = False
    name = name.rstrip()
    if isFileEmpty(filename):
        appendRecord(name, filename)        
    else:
        records = getRecords(filename)
        if isNameInRecords(name, records):
            #show that python can return more than one variable
            records, maxScoreReached = addOneToScore(name, records) 
            if not maxScoreReached: #no need to sort and override
                records = sortByScore(records)
                overrideFile(records, filename)
        else:
            appendRecord(name, filename)

def overrideFile(records, filename):
    file = open(str(filename),'w')
    size = int(len(records))
    for r in range(0,size):
        score = records[r][0]
        name = records[r][1]
        file.write("{} {}\n".format(score, name))
    file.close()

def sortByScore(records):
    count = len(records)
    if count > 1:
        i = 0
        while (i < count-1):            
            if int(records[i][0]) < int(records[i+1][0]):
                tempScore = records[i][0]
                records[i][0] = records[i+1][0]
                records[i+1][0] = tempScore

                tempName = records[i][1]
                records[i][1] = records[i+1][1]
                records[i+1][1] = tempName

            if i > 0:
                j = i
                while j > 0:
                    if int(records[j-1][0]) < int(records[j][0]) :
                        tempScore = records[j-1][0]
                        records[j-1][0] = records[j][0]
                        records[j][0] = tempScore

                        tempName = records[j-1][1]
                        records[j-1][1] = records[j][1]
                        records[j][1] = tempName
                    j -= 1
            i += 1
    return records

def addOneToScore(name, records):
    maxScoreReached = False
    count = int(len(records))
    maxScore = 999
    maxDigits = len(str(maxScore))
    if count > 0:
        i = 0
        while (i < count):
            currentName = records[i][1]
            if currentName == name:
                strScore = records[i][0]
                #cast score to int
                intScore = int(strScore)
                if intScore != maxScore:
                    intScore = intScore + 1
                    records[i][0] = formatScore(str(intScore), maxDigits)
                    break
                else:
                    maxScoreReached = True #max score is reached
                    break
            i += 1
    return records, maxScoreReached

def formatScore(strScore, maxDigits):
    zerosToAdd = maxDigits - len(strScore)
    if zerosToAdd > 0:
        while zerosToAdd > 0:
            strScore = "0" + strScore
            zerosToAdd = zerosToAdd - 1
    return strScore

def isNameInRecords(name, records):
    nameInRecords = False
    count = int(len(records))

    if count > 0:
        i = 0
        while (i < count):
            currentName = records[i][1]
            if currentName.rstrip() == name.rstrip():            
                nameInRecords = True
            i += 1
    return nameInRecords
    
def getRecords(filename):
    rowsCount = getCountRecords(filename)    
    readFile = open(str(filename),"r")
    r = 0    
    records = [["0","none"] for _ in range(int(rowsCount))]
    #first line
    line = readFile.readline()
    count = 0
    while count < rowsCount:
        line.rstrip() #get rid of whitespace
        strings = line.split(" ")

        for string in strings:
            if string.isdigit():
                records[r][0] = string.strip()
            elif string:
                records[r][1] = string.strip()        

        r += 1; #get the index for the new row
        line = readFile.readline() #get new line  
        count += 1
        
    readFile.close()
    return records
    
def appendRecord(name, filename):        
    file = open(str(filename),'a')
    file.write("{} {}\n".format("001", name))
    file.close()
    
def isFileEmpty(filename):
    return    0 == getCountRecords(filename)

def getCountRecords(filename):
    return sum(1 for line in open(str(filename)))

def printScores(records):
    for row in records:
        print("{0} {1}".format(row[0],row[1]))


    #if file is empty
        #append the name with score 1 to the empty file 
    #else
        #extract records as 2d list of score and name
        #if name in 2d list
            #add one to score of name if max score not reached else do nothing
            #if max score not reached
                #sort records by score
                #override file with updated records                
        #else
            #append the name with score 1 to the file 
            #(as the name is not in the file then it is last in the list)
        