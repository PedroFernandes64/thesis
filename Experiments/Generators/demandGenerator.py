import csv
import os
import random
import gnModelPrecomputingToolV2 as preCompute


def buildNetworkLinkSet():
    NetworkLinkSet = []
    Networks = [f.name for f in os.scandir("../Inputs/Networks") if f.is_dir()]
    for Network in Networks:
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
    return NetworkLinkSet

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
    srcPos = int(src) - 1
    destPos = int(dest) - 1
    nodeCounter = 0
    while nodeCounter < len(graph):
        dist.append(9999999)
        sptSet.append(False)
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
                dist[v] = dist[u] + graph[u][v]
            v = v + 1
        nodeCounter1 = nodeCounter1 + 1
        
    chosendistance = dist[destPos]
    return chosendistance

def chooseTransponder(NumberOfNetworks,NetworksDemandsSets,NetworkAsGraphs,TransponderTable):
    iteration = 0
    NewNetworksDemandsSets = []
    while iteration < NumberOfNetworks:
        newNetworkDemandTable = []
        rowCounter = 1
        for row in NetworksDemandsSets[iteration]:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                dataDemand = row[3]
                shortestDistance = shortestPath(NetworkAsGraphs[iteration], originDemand,destinationDemand)
                feasibleTransponders = []
                rowCounter2 = 1
                for row2 in TransponderTable:
                    transponder = []
                    if rowCounter2 != 1:
                        if (int(row2[7]) >= shortestDistance):
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
                lessSlotsId = 0
                lessSlots = 10
                rowCounter3 = 1
                for row3 in feasibleTransponders:
                    if rowCounter3 !=1:
                        if int(row3[1]) >= int(dataDemand) and int(row3[4]) < lessSlots:
                            lessSlotsId = row3[0]
                            lessSlots = int(row3[4])
                            chosenSlots = row3[4]
                            chosenMaxReach = row3[7]
                            chosenOsnrLim = row3[5]
                    rowCounter3 = rowCounter3 + 1
                if lessSlotsId != 0:
                    print("able to use one transponder! id: ", lessSlotsId)
                    newRow = row
                    newRow.pop()
                    newRow.append(chosenSlots)
                    newRow.append(chosenMaxReach)
                    newRow.append(chosenOsnrLim)
                    newNetworkDemandTable.append(newRow)
                else:
                    print("mamou palhaço")
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

    counter = 0
    for instance in NewNetworksDemandsSets:
        print(len(NetworkAsGraphs[counter]))
        print(len(NetworksDemandsSets[counter])-1)
        print(len(instance)-1)
        for rowzita in instance:
            print(rowzita)
        counter = counter + 1
    return 'meubau'


NetworksLinksToProcess = buildNetworkLinkSet()                                              #this produce a table for each link file
NetworksNodesToProcess = buildNetworkNodeSet()                                              #this produce a table for each node file
NumberOfNetworks = len(NetworksLinksToProcess)
preCompute.processLinks(NetworksLinksToProcess[1])                                          #this add GN model columns in the link table
NetworkAsGraphs = buildGraphSet(NetworksLinksToProcess,NetworksNodesToProcess)              #this create a graph for each table of links
NetworksDemandsSets = buildBaseDemandSet(NumberOfNetworks,NetworksNodesToProcess)           #this create a base demand set for each table of nodes
TransponderTable = buildTransponderTable()
chooseTransponder(NumberOfNetworks,NetworksDemandsSets,NetworkAsGraphs,TransponderTable)
