import csv
import os
import shutil
import random
import math
import copy
import gnModelPrecomputingTool as preCompute


def buildNetworkLinkSet():
    NetworkLinkSet = []
    instanceNames = []
    Networks = [f.name for f in os.scandir("../Inputs/Networks") if f.is_dir()]
    for Network in Networks:
        instanceNames.append(Network)
        NetworkLinkFile = "../Inputs/Networks/" + Network + '/Link.csv'
        with open(NetworkLinkFile, newline='') as csvfile: 
            rows = csv.reader(csvfile, delimiter=';', quotechar='|')
            NetworkFileLines = []
            for row in rows:
                NetworkFileLine = []
                column = 0
                while column < len(row):
                    NetworkFileLine.append(row[column])
                    column = column + 1
                NetworkFileLines.append(NetworkFileLine)
        NetworkLinkSet.append(NetworkFileLines)
    return NetworkLinkSet, instanceNames

def buildNetworkNodeSet():
    NetworkNodeSet = []
    Networks = [f.name for f in os.scandir("../Inputs/Networks") if f.is_dir()]
    for Network in Networks:
        NetworkLinkFile = "../Inputs/Networks/" + Network + '/Node.csv'
        with open(NetworkLinkFile, newline='') as csvfile: 
            rows = csv.reader(csvfile, delimiter=';', quotechar='|')
            NetworkFileLines = []
            for row in rows:
                NetworkFileLine = []
                column = 0
                while column < len(row):
                    NetworkFileLine.append(row[column])
                    column = column + 1
                NetworkFileLines.append(NetworkFileLine)
        NetworkNodeSet.append(NetworkFileLines)
    return NetworkNodeSet

def buildGraphSet(NetworksLinksToProcess,NetworksNodesToProcess):
    NetworkAsGraphs = []
    netWorkNumber = 0
    for network in NetworksLinksToProcess:
        NetworkGraphNodes = len(NetworksNodesToProcess[netWorkNumber]) - 1
        NetworkGraphDistanceMatrix =[]
        i = 0
        while i <NetworkGraphNodes:
            matrixLine = []
            j = 0
            while j <NetworkGraphNodes:
                matrixLine.append(0)
                j = j + 1
            NetworkGraphDistanceMatrix.append(matrixLine)
            i = i +1
        rowCounter = 1
        for row in network:
            if rowCounter != 1:
                NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = int(row[3])
                NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = int(row[3])
            rowCounter = rowCounter + 1
        NetworkAsGraphs.append(NetworkGraphDistanceMatrix)
        netWorkNumber = netWorkNumber + 1
    return NetworkAsGraphs

def buildBaseDemandSet(NumberOfNetworks,NetworksNodesToProcess):
    NetworksDemandsSets = []
    iteration = 0
    while iteration < NumberOfNetworks:
        demandSet =[]
        row0 = ["index" , "origin" , "destination","data"]
        demandSet.append(row0)
        meshyFlag = False
        coreFlag = False
        core = []
        secondary= []
        meshy = []
        rowCounter = 1
        for row in NetworksNodesToProcess[iteration]:
            if rowCounter != 1:
                if row[1] == 'core':
                    core.append(row[0])
                if row[1] == 'secondary':
                    secondary.append(row[0])
                if row[1] == 'meshy':
                    meshy.append(row[0])
            rowCounter = rowCounter + 1
        if len(meshy) == 0:
            coreFlag = True
        else:
            meshyFlag = True
        #BEGIN OF MESHY CASE
        if (meshyFlag == True):
            id = 1
            for node1 in meshy:
                for node2 in meshy:
                    if int(node1) < int(node2):
                        row = []
                        row.append(str(id))
                        id = id + 1
                        row.append(node1)
                        row.append(node2)
                        row.append("200")
                        demandSet.append(row)    
        #BEGIN OF CORE CASE
        if (coreFlag == True):
            id = 1
            for node1 in core:
                for node2 in core:
                    if int(node1) < int(node2):
                        row = []
                        row.append(str(id))
                        id = id + 1
                        row.append(node1)
                        row.append(node2)
                        row.append("400")
                        demandSet.append(row)
            for node1 in secondary:
                destinationToSelect = len(core) - 1
                selectedDestination = random.sample(core,k=destinationToSelect)
                for node2 in selectedDestination:
                    row = []
                    row.append(str(id))
                    id = id + 1
                    if int(node1) < int(node2):
                        row.append(node1)
                        row.append(node2)
                        row.append("200")
                    else:
                        row.append(node2)
                        row.append(node1)
                        row.append("200")
                    demandSet.append(row)
        iteration = iteration + 1
        NetworksDemandsSets.append(demandSet)
    return NetworksDemandsSets

def buildTransponderTable():
    #transponder limits
    with open('../Inputs/transponders.csv', newline='') as csvfile: 
        transponders = csv.reader(csvfile, delimiter=';', quotechar='|')
        transponderTable = []
        for row in transponders:
            transponderTable.append(row)
    return transponderTable  

def minDistance(dist,sptSet):
    # Initialize min value
    min = 9999999
    min_index = 0
    nodeCounter = 0
    while nodeCounter < len(sptSet):
        if (sptSet[nodeCounter] == False and dist[nodeCounter] <= min):
            min = dist[nodeCounter]
            min_index = nodeCounter
        nodeCounter = nodeCounter + 1
    return min_index

def shortestPath(graph, src, dest):

    dist = []	#The output array.  dist[i] will hold the shortest distance from src to i
    sptSet = [] #sptSet[i] will be true if vertex i is included in shortest path tree or shortest distance from src to i is finalized
    pathNodes = []

    srcPos = int(src) - 1
    destPos = int(dest) - 1
    nodeCounter = 0
    while nodeCounter < len(graph):
        dist.append(9999999)
        sptSet.append(False)
        nodes =[]
        pathNodes.append(nodes)
        nodeCounter = nodeCounter + 1
    # Distance of source vertex from itself is always 0
    dist[srcPos] = 0
    # Find shortest path for all vertices
    nodeCounter1 = 0
    while nodeCounter1 < len(graph):
        ## Pick the minimum distance vertex from the set of vertices not yet processed. u is always equal to src in the first iteration.
        u = minDistance(dist, sptSet)
        # Mark the picked vertex as processed
        sptSet[u] = True
        ## Update dist value of the adjacent vertices of the picked vertex.
        v = 0
        while v < len(graph):
            # Update dist[v] only if is not in sptSet, there is an edge from u to v, and total weight of path from src to  v through u is smaller than current value of dist[v]            
            if (sptSet[v] == False and graph[u][v] > 0 and dist[u] != 999999 and dist[u] + graph[u][v] < dist[v]):
                if len(pathNodes[v]) > 0:
                    pathNodes[v].clear()

                dist[v] = dist[u] + graph[u][v]
                pathNodes[v].append(u)
                for node in pathNodes[u]:
                    pathNodes[v].append(node)
            v = v + 1
        nodeCounter1 = nodeCounter1 + 1
        
    chosendistance = dist[destPos]
    for path in pathNodes:
        path.reverse()
    auxCounter = 0
    while auxCounter < len(pathNodes):
        pathNodes[auxCounter].append(auxCounter)
        auxCounter = auxCounter + 1

    return chosendistance, pathNodes[destPos]

def getPaseNode():
    h = 6.62 * pow(10,-34)
    lambd = 1545.0 * pow(10,-9)
    c = 3.0 *pow(10,8)
    nu = c/lambd
    NF = 5.0
    nsp = (1.0/2.0) * pow(10.0,NF/10.0)
    alpha = 0.2
    a = math.log(10)*alpha/20 * pow(10,-3)
    Bn = 12.5 * pow(10,9)
    Gdb = 20
    Glin = pow(10,Gdb/10)
    paseNod = 2.0* h * nu * nsp * (Glin-1.0) * Bn
    return paseNod

def osnrLhs(links, path):
    numberOfNodes = len(path)
    counter = 0
    lhs = 0 
    while counter < numberOfNodes - 1:
        u = path[counter] + 1
        v = path[counter+1] + 1
        counter = counter + 1
        for link in links[1:]:
            if ((int(link[1]) == u and int(link[2]) == v) or (int(link[1]) == v and int(link[2]) == u)):
                lhs = lhs + float(link[7]) + float(link[8])
                break
    paseNode = getPaseNode()
    lhs = lhs + (len(path) * paseNode)
    return lhs

def osnrRhs(slots,osnrLimit):
    pSat = 50 * pow(10,-3)
    bwdm = 5000 * pow(10,9)
    gwdm = pSat/bwdm
    Bn = 12.5 * pow(10,9)
    pchDemand = int(slots) * Bn * gwdm
    rhs = pchDemand/float(osnrLimit)
    return rhs

def chooseTransponder(NumberOfNetworks,NetworksDemandsSets,NetworkAsGraphs,TransponderTable, NetworksLinksToProcess):
    iteration = 0
    NewNetworksDemandsSets = []
    #para cada network, fazer o processo
    while iteration < NumberOfNetworks:
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda da, fazer o processo
        for row in NetworksDemandsSets[iteration]:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                dataDemand = row[3]
                shortestDistance, path = shortestPath(NetworkAsGraphs[iteration], originDemand,destinationDemand)
                lhs = osnrLhs(NetworksLinksToProcess[iteration],path)
                feasibleTransponders = []
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                for row2 in TransponderTable:
                    transponder = []
                    if rowCounter2 != 1:
                        rhs = osnrRhs(row2[4],row2[5])             
                        if (int(row2[7]) >= shortestDistance) and lhs <= rhs:
                            transponder.append(row2[0])
                            transponder.append(row2[1])
                            transponder.append(row2[2])
                            transponder.append(row2[3])
                            transponder.append(row2[4])
                            transponder.append(row2[5])
                            transponder.append(row2[6])
                            transponder.append(row2[7])
                            feasibleTransponders.append(transponder) 
                    else:
                        transponder.append(row2[0])
                        transponder.append(row2[1])
                        transponder.append(row2[2])
                        transponder.append(row2[3])
                        transponder.append(row2[4])
                        transponder.append(row2[5])
                        transponder.append(row2[6])
                        transponder.append(row2[7])
                        feasibleTransponders.append(transponder) 
                    rowCounter2 = rowCounter2 + 1
                #for rowAux in feasibleTransponders:
                #trying to pick one transponder
                #para feasible, tentar escolher um
                lessSlotsId = 0
                lessSlots = 1000
                rowCounter3 = 1
                for row3 in feasibleTransponders:
                    if rowCounter3 !=1:
                        #if int(row3[1]) >= int(dataDemand) and int(row3[1])/int(row3[4]) < lessSlots: #DATA EFFICIENCY VERSION    
                        if int(row3[1]) >= int(dataDemand) and int(row3[4]) < lessSlots: #DATA SLOTS VERSION
                            lessSlotsId = row3[0]
                            lessSlots = int(row3[4])
                            chosenSlots = row3[4]
                            chosenMaxReach = row3[7]
                            chosenOsnrLim = row3[5]
                    rowCounter3 = rowCounter3 + 1
                if lessSlotsId != 0:
                    #print("able to use one transponder! id: ", lessSlotsId)
                    newRow = row
                    newRow.pop()
                    newRow.append(chosenSlots)
                    newRow.append(chosenMaxReach)
                    newRow.append(chosenOsnrLim)
                    newNetworkDemandTable.append(newRow)
                else:
                    #se nao for possivel escolher um, reduzir a demand Gbit/s
                    transponderChosen = False
                    while transponderChosen == False:
                        dataDemand = str(int(dataDemand) - 100)
                        row[3] = str(int(row[3]) - 100)
                        secondaryLessSlotsId = 0
                        secondaryLessSlots = 10
                        secondaryRowCounter3 = 1
                        for row4 in feasibleTransponders:
                            if secondaryRowCounter3 !=1:
                                if int(row4[1]) >= int(dataDemand) and int(row4[4]) < secondaryLessSlots:
                                    secondaryLessSlotsId = row4[0]
                                    secondaryLessSlots = int(row4[4])
                                    chosenSlots = row4[4]
                                    chosenMaxReach = row4[7]
                                    chosenOsnrLim = row4[5]
                                    transponderChosen = True
                            secondaryRowCounter3 = secondaryRowCounter3 + 1     
                    #print("able to use one transponder! id: ", lessSlotsId)
                    newRow = row
                    newRow.pop()
                    newRow.append(chosenSlots)
                    newRow.append(chosenMaxReach)
                    newRow.append(chosenOsnrLim)
                    newNetworkDemandTable.append(newRow)
            else:
                newRow = row
                newRow.pop()
                newRow.append("slots")
                newRow.append("max_length")
                newRow.append("osnr_limit")
                newNetworkDemandTable.append(newRow)
            rowCounter = rowCounter + 1
        NewNetworksDemandsSets.append(newNetworkDemandTable)
        iteration = iteration + 1
    #print auxiliar
    counter = 0
    '''
    for instance in NewNetworksDemandsSets:
        print("nodes = " , len(NetworkAsGraphs[counter]))
        print("demands = " , len(NetworksDemandsSets[counter])-1)
        print("demands chosen = " , len(instance)-1)
        for rowzita in instance:
            print(rowzita)
        counter = counter + 1
    '''
    return NewNetworksDemandsSets

def buildNetworkLinkSetWithoutPNLI(NetworksLinksToProcess):
    NetworksLinksToProcessWithoutPNLI = []

    NetworksLinksToProcessWithoutPNLI=copy.deepcopy(NetworksLinksToProcess)
    for instance in NetworksLinksToProcessWithoutPNLI:
        rowCounter = 1
        for row in instance:
            if rowCounter !=1 :
                row[7] = "0"
            rowCounter = rowCounter +1
    return NetworksLinksToProcessWithoutPNLI

def writeLinkFile(linkTable,network):
    filename = "../Outputs/Instances/" + network + "/Link.csv"
    print(filename)
    with open(filename, "w") as f:
        for row in linkTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)

def writeDemandFile(demandTable,network):
    os.mkdir("../Outputs/Instances/" + network + "/" + str(len(demandTable)-1) + "_demands/")
    filename = "../Outputs/Instances/" + network + "/" + str(len(demandTable)-1) + "_demands/demands.csv"
    print(filename)
    with open(filename, "w") as f:
        for row in demandTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)
    
def writeLinkFileNonPNLI(linkTable,network):
    filename = "../Outputs/Instances/" + network + "/LinkWithoutPnli.csv"
    print(filename)
    with open(filename, "w") as f:
        for row in linkTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)

NetworksLinksToProcess, instanceNames = buildNetworkLinkSet()                               #this produce a table for each link file
NetworksNodesToProcess = buildNetworkNodeSet()                                              #this produce a table for each node file
NumberOfNetworks = len(NetworksLinksToProcess)
for network in NetworksLinksToProcess:
    preCompute.processLinks(network)                                                        #this add GN model columns in the link table
NetworkAsGraphs = buildGraphSet(NetworksLinksToProcess,NetworksNodesToProcess)              #this create a graph for each table of links
NetworksDemandsSets = buildBaseDemandSet(NumberOfNetworks,NetworksNodesToProcess)           #this create a base demand set for each table of nodes
TransponderTable = buildTransponderTable()
NewNetworksDemandsSets = chooseTransponder(NumberOfNetworks,NetworksDemandsSets,NetworkAsGraphs,TransponderTable,NetworksLinksToProcess)
for network in NewNetworksDemandsSets:
    preCompute.processDemands(network)   

#dar a opçao de criar as demandas sem Pnli. Para criar sem GN Model no geral, basta mudar o onlineParameters
createNonPNLI = True
if createNonPNLI == True:
    NetworksLinksToProcessWithoutPNLI = buildNetworkLinkSetWithoutPNLI(NetworksLinksToProcess)
#write outputs
netId = 0
outputFolder = [f.name for f in os.scandir("../Outputs") if f.is_dir()]
if "Instances" not in outputFolder:
    os.mkdir("../Outputs/Instances")
else:
    shutil.rmtree("../Outputs/Instances")
    os.mkdir("../Outputs/Instances")

for network in instanceNames:
    os.mkdir("../Outputs/Instances/" + network)
    writeLinkFile(NetworksLinksToProcess[netId],network)
    writeDemandFile(NewNetworksDemandsSets[netId],network)
    if createNonPNLI == True:
        writeLinkFileNonPNLI(NetworksLinksToProcessWithoutPNLI[netId],network)
    netId = netId + 1
