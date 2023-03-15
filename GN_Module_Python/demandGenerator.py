import random
import csv
import gnModelPrecomputingTool as preCompute

#generates all possible demands within transponder limits
def buildTransponderTable():
    #transponder limits
    osnrub = 23
    osnrlb = 0
    maxlengthub = 2000
    maxlengthlb = 0
    with open('transponders.csv', newline='') as csvfile: 
        transponders = csv.reader(csvfile, delimiter=';', quotechar='|')
        transponderTable = []
        rowcounter = 0
        for row in transponders:
            if rowcounter > 0:
                if (int(row[7]) <= maxlengthub  and int(row[7]) >= maxlengthlb) and (int(row[5]) >= osnrlb and int(row[5]) <= osnrub): #LIMIT WHICH TRANSPONDER USE
                    transponderTable.append(row)
            rowcounter = rowcounter + 1
    return transponderTable    

def allDemandGenerator(file, n, tTable):
    transponderOptions=len(tTable)-1
    nodes = n
    newFile1 = []
    header = ['Index', 'Origin', 'Destination','slots', 'max_length', 'osnr_limit']
    newFile1.append(header)
    counter = 1
    originaux = 1
    destinationaux = 2
    transponderaux= 1
    while counter <= (nodes*(nodes-1)/2)*transponderOptions:
        id = counter
        origin = originaux
        destination = destinationaux
        slots = tTable[transponderaux][4]
        maxlength = tTable[transponderaux][7]
        osnrlimit = tTable[transponderaux][5]
        row = []
        row.append(id)
        row.append(origin)
        row.append(destination)
        row.append(slots)
        row.append(maxlength)
        row.append(osnrlimit)
        newFile1.append(row)
        counter = counter + 1
        transponderaux = transponderaux + 1
        if transponderaux > transponderOptions:
            transponderaux = 1
            destinationaux = destinationaux + 1
            if destinationaux > nodes:
                originaux = originaux + 1
                destinationaux = originaux + 1
    with open(file, 'w', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerows(newFile1)

def randomDemandGenerator(file, n, tTable, d):
    transponderOptions=len(tTable)-1
    nodes = n
    demands = d
    newFile1 = []
    header = ['Index', 'Origin', 'Destination','slots', 'max_length', 'osnr_limit']
    newFile1.append(header)
    counter = 1
    while counter <= demands:
        id = counter
        origin = random.randint(1,nodes)
        destination = random.randint(1,nodes)
        while origin == destination:
            destination = random.randint(1,nodes)
        usedTransponder = random.randint(1,transponderOptions)
        slots = tTable[usedTransponder][4]
        maxlength = tTable[usedTransponder][7]
        osnrlimit = tTable[usedTransponder][5]
        row = []
        row.append(id)
        row.append(origin)
        row.append(destination)
        row.append(slots)
        row.append(maxlength)
        row.append(osnrlimit)
        newFile1.append(row)
        counter = counter + 1

    with open(file, 'w', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerows(newFile1)

tTable = buildTransponderTable()
demandfile = 'demands'
#allDemandGenerator('demands.csv',5,tTable)
randomDemandGenerator(demandfile + '.csv', 17,tTable,30)
preCompute.processDemands(demandfile)
        