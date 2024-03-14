import csv
import os
import shutil
import random
import math
import copy
import gnModelPrecomputingTool as preCompute


class Links:
    def __init__(self,topology):
        self.rows = []
        self.topology = topology

class Nodes:
    def __init__(self,topology):
        self.rows = []
        self.topology = topology

class Demands:
    def __init__(self,topology):
        self.rows = []
        self.topology = topology
        self.transponderStrategy = "none"
        self.demandStrategy = "none"

class Graph:
    def __init__(self,topology):
        self.rows = []
        self.topology = topology

class Instance:
    def __init__(self,topology,links,demands,id):
        self.links = links
        self.demands = demands
        self.topology = topology
        self.slotStrategy = "none"
        self.transponderStrategy = "none"
        self.demandStrategy = "none"
        self.id = 0

def CustomClassVerifier(NetworksLinksToProcess):
    for item in NetworksLinksToProcess:
        print(item.topology)
        for row in item.rows:
            print(row)

def DemandVerifier(demandList):
    for item in demandList:
        print(item.topology)
        print(item.transponderStrategy)
        print(item.demandStrategy)
        for row in item.rows:
            print(row)



def buildNetworkLinkSet():
    NetworkLinkSet = []
    topologyList = []
    Networks = [f.name for f in os.scandir("../Inputs/1_NetworksToGenerateDemands") if f.is_dir()]
    for Network in Networks:
        topologyList.append(Network)
        links = Links(Network)
        NetworkLinkFile = "../Inputs/1_NetworksToGenerateDemands/" + Network + '/Link.csv'
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
        links.rows=NetworkFileLines
        NetworkLinkSet.append(links)
    return NetworkLinkSet,topologyList

def buildNetworkNodeSet():
    NetworkNodeSet = []
    Networks = [f.name for f in os.scandir("../Inputs/1_NetworksToGenerateDemands") if f.is_dir()]
    for Network in Networks:
        NetworkLinkFile = "../Inputs/1_NetworksToGenerateDemands/" + Network + '/Node.csv'
        nodes = Nodes(Network)
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
        nodes.rows=NetworkFileLines
        NetworkNodeSet.append(nodes)
    return NetworkNodeSet

def buildLengthGraphSet(NetworksLinksToProcess,NetworksNodesToProcess):
    NetworkAsGraphs = []
    for network in NetworksLinksToProcess:
        topology = network.topology
        graph = Graph(topology)
        for network2 in NetworksNodesToProcess:
            if network2.topology == topology:
                NbNetworkGraphNodes = len(network2.rows) - 1
        NetworkGraphDistanceMatrix =[]
        i = 0
        while i <NbNetworkGraphNodes:
            matrixLine = []
            j = 0
            while j <NbNetworkGraphNodes:
                matrixLine.append(0)
                j = j + 1
            NetworkGraphDistanceMatrix.append(matrixLine)
            i = i +1
        rowCounter = 1
        for row in network.rows:
            if rowCounter != 1:
                NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = int(row[3])
                NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = int(row[3])
            rowCounter = rowCounter + 1
        graph.rows = NetworkGraphDistanceMatrix
        NetworkAsGraphs.append(graph)
    return NetworkAsGraphs

def buildOSNRGraphSet(NetworksLinksToProcess,NetworksNodesToProcess):
    NetworkAsGraphs = []
    for network in NetworksLinksToProcess:
        topology = network.topology
        graph = Graph(topology)
        for network2 in NetworksNodesToProcess:
            if network2.topology == topology:
                NbNetworkGraphNodes = len(network2.rows) - 1
        NetworkGraphDistanceMatrix =[]
        i = 0
        while i <NbNetworkGraphNodes:
            matrixLine = []
            j = 0
            while j <NbNetworkGraphNodes:
                matrixLine.append(0)
                j = j + 1
            NetworkGraphDistanceMatrix.append(matrixLine)
            i = i +1
        rowCounter = 1
        for row in network.rows:
            if rowCounter != 1:
                NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = float(row[7])+ float(row[8]) + float(getPaseNode())
                NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = float(row[7])+ float(row[8]) + float(getPaseNode())
            rowCounter = rowCounter + 1
        graph.rows = NetworkGraphDistanceMatrix
        NetworkAsGraphs.append(graph)
    return NetworkAsGraphs

def getPaseNode():
    c = preCompute.c
    h = preCompute.h
    lambdC = preCompute.lambdC
    NFC = preCompute.NFC
    Bn = preCompute.Bn
    GdbNodeC = preCompute.GdbNodeC
    paseNod = preCompute.computePaseNodeCBand(c,h,lambdC,NFC,Bn,GdbNodeC)

    return paseNod

def buildTransponderTable():
    #transponder limits
    with open('../Inputs/transponders.csv', newline='') as csvfile: 
        transponders = csv.reader(csvfile, delimiter=';', quotechar='|')
        transponderTable = []
        for row in transponders:
            transponderTable.append(row)
    return transponderTable  


# LEVEL 1 CHOICE - DEMANDS PAIR
#THIS STRATEGY CREATES ALL PAIRS OF ORIGIN AND DESTINATION AND BUILDS THE INITIAL SET
def buildBaseDemandSet(NetworksNodesToProcess):
    NetworksDemandsSets = []
    for network in NetworksNodesToProcess:
        demands = Demands(network.topology)
        demandSet =[]
        row0 = ["index" , "origin" , "destination"]
        demandSet.append(row0)
        meshy = []
        rowCounter = 1
        for row in network.rows:
            if rowCounter != 1:
                meshy.append(row[0])
            rowCounter = rowCounter + 1

        id = 1
        for node1 in meshy:
            for node2 in meshy:
                if (int(node1) < int(node2)) :
                    row = []
                    row.append(str(id))
                    id = id + 1
                    row.append(node1)
                    row.append(node2)
                    demandSet.append(row)    
        demands.rows = demandSet
        demands.demandStrategy = "allPair"
        NetworksDemandsSets.append(demands)
    return NetworksDemandsSets

#THIS STRATEGY CREATES CORE SETS OF ORIGIN AND DESTINATION AND ADD TO THE INITIAL SET
def addCoreToDemandSet(NetworksNodesToProcess,NetworksDemandsSets):
    caseCounter = 0
    print("Looking for core nodes at input")
    for network in NetworksNodesToProcess:
        demands = Demands(network.topology)
        demandSet =[]
        row0 = ["index" , "origin" , "destination"]
        demandSet.append(row0)
        core = []
        secondary= []
        
        rowCounter = 1
        for row in network.rows:
            if rowCounter != 1:
                if row[1] == 'core':
                    core.append(row[0])
                if row[1] == 'secondary':
                    secondary.append(row[0])
            rowCounter = rowCounter + 1
        if len(core) > 0:
            print("- Adding " + network.topology + " with core strategy")
            caseCounter = caseCounter + 1
            id = 1
            for node1 in core:
                for node2 in core:
                    if int(node1) < int(node2):
                        row = []
                        row.append(str(id))
                        id = id + 1
                        row.append(node1)
                        row.append(node2)
                        demandSet.append(row)
            for node1 in secondary:
                destinationToSelect = len(core)
                selectedDestination = random.sample(core,k=destinationToSelect-1)
                for node2 in selectedDestination:
                    row = []
                    row.append(str(id))
                    id = id + 1
                    if int(node1) < int(node2):
                        row.append(node1)
                        row.append(node2)
                    else:
                        row.append(node2)
                        row.append(node1)
                    demandSet.append(row)
            demands.rows = demandSet
            demands.demandStrategy = "coreSet"
            NetworksDemandsSets.append(demands)

    if caseCounter > 0:
        print(str(caseCounter)+" demand sets with network core generated")
    return NetworksDemandsSets

#THIS STRATEGY CREATES 75% RANGE SETS OF ORIGIN AND DESTINATION AND ADD To THE INITIAL SET
#def add75sampleToDemandSet(NetworksNodesToProcess,NetworksDemandsSets):

#THIS STRATEGY CREATES 50% RANGE SETS OF ORIGIN AND DESTINATION AND ADD To THE INITIAL SET
#def add50sampleToDemandSet(NetworksNodesToProcess,NetworksDemandsSets):

#THIS STRATEGY CREATES 25% RANGE SETS OF ORIGIN AND DESTINATION AND ADD To THE INITIAL SET
#def add25sampleToDemandSet(NetworksNodesToProcess,NetworksDemandsSets):
    
# LEVEL 2 CHOICE - TRANSPONDER
#THIS STRATEGY SELECTS THE TRANSPONDER WITH LARGER DATA/SLOT RATIO
def chooseMostEfficientTransponder(NetworksDemandsSetsWithTransponders,NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess):
    for network in NetworksDemandsSets:
        print("Processing: " + network.topology + " with " + str(len(network.rows)-1) + " demands")
        demands = Demands(network.topology)
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda da, fazer o processo
        for row in network.rows:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                for graph in NetworkAsLengthGraphs:
                    if graph.topology == network.topology:
                        shortestDistance, path = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noise = noiseOfPath(links.rows,path)
                feasibleTransponders = []
                #print(path) #check shortest path 
                
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                for row2 in TransponderTable:
                    transponder = []
                    if rowCounter2 != 1:
                        pch = osnrPch(row2[4])
                        osnr = pch/noise
                        osnrdb = 10.0 * math.log10(osnr)
                        #print(str(osnrdb) +" x "+ row2[5])      #check if transponder is feasbile  
                        if (int(row2[7]) >= shortestDistance) and float(row2[5]) <= osnrdb:
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
                
                #STRATEGY ONE: PICKING THE LARGEST DATA/SLOT RATIO POSSIBLE
                efficiency = 0
                data = 0
                rowCounter3 = 1
                if len(feasibleTransponders)>1:
                    for row3 in feasibleTransponders:
                        if rowCounter3 !=1:
                            if (int(row3[1])/int(row3[4]) > efficiency) or ((int(row3[1])/int(row3[4]) == efficiency) and (int(row3[1]) > data)): 

                                data = int(row3[1])
                                chosenSlots = row3[4]
                                chosenMaxReach = row3[7]
                                chosenOsnrLim = row3[5]
                        rowCounter3 = rowCounter3 + 1

                    newRow = row
                    newRow.append(chosenSlots)
                    newRow.append(chosenMaxReach)
                    newRow.append(chosenOsnrLim)
                    newNetworkDemandTable.append(newRow)
                else:
                    print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
            else:
                newRow = row
                newRow.append("slots")
                newRow.append("max_length")
                newRow.append("osnr_limit")
                newNetworkDemandTable.append(newRow)
            rowCounter = rowCounter + 1
        demands.demandStrategy = network.demandStrategy
        demands.transponderStrategy = "efficient"
        demands.rows = newNetworkDemandTable  
        NetworksDemandsSetsWithTransponders.append(demands)
    return NetworksDemandsSetsWithTransponders
#THIS STRATEGY SELECTS THE TRANSPONDER WITH SMALL DATA CHARGE (200 OR LESS)
def addLowDataTransponder(NetworksDemandsSetsWithTransponders,NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess):
    for network in NetworksDemandsSets:
        print("Processing: " + network.topology + " with " + str(len(network.rows)-1) + " demands")
        demands = Demands(network.topology)
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda da, fazer o processo
        for row in network.rows:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                for graph in NetworkAsLengthGraphs:
                    if graph.topology == network.topology:
                        shortestDistance, path = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noise = noiseOfPath(links.rows,path)
                feasibleTransponders = []
                #print(path) #check shortest path 
                
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                for row2 in TransponderTable:
                    transponder = []
                    if rowCounter2 != 1:
                        pch = osnrPch(row2[4])
                        osnr = pch/noise
                        osnrdb = 10.0 * math.log10(osnr)
                        #print(str(osnrdb) +" x "+ row2[5])      #check if transponder is feasbile  
                        if (int(row2[7]) >= shortestDistance) and float(row2[5]) <= osnrdb and (int(row2[1]) <= 200) and (int(row2[4]) < 6):
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
                
                #STRATEGY ONE: PICKING THE LARGEST DATA/SLOT RATIO POSSIBLE
                efficiency = 0
                data = 0
                rowCounter3 = 1
                if len(feasibleTransponders)>1:
                    for row3 in feasibleTransponders:
                        if rowCounter3 !=1:
                            if (int(row3[1])/int(row3[4]) > efficiency) or ((int(row3[1])/int(row3[4]) == efficiency) and (int(row3[1]) > data)): 
                                data = int(row3[1])
                                chosenSlots = row3[4]
                                chosenMaxReach = row3[7]
                                chosenOsnrLim = row3[5]
                        rowCounter3 = rowCounter3 + 1

                    newRow = row
                    newRow.append(chosenSlots)
                    newRow.append(chosenMaxReach)
                    newRow.append(chosenOsnrLim)
                    newNetworkDemandTable.append(newRow)
                else:
                    print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
            else:
                newRow = row
                newRow.append("slots")
                newRow.append("max_length")
                newRow.append("osnr_limit")
                newNetworkDemandTable.append(newRow)
            rowCounter = rowCounter + 1
        demands.demandStrategy = network.demandStrategy
        demands.transponderStrategy = "lowData"
        demands.rows = newNetworkDemandTable  
        NetworksDemandsSetsWithTransponders.append(demands)
    return NetworksDemandsSetsWithTransponders
#THIS STRATEGY SELECTS A RANDOM TRANSPONDER
def addRandomTransponder(NetworksDemandsSetsWithTransponders,NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess):
    for network in NetworksDemandsSets:
            print("Processing: " + network.topology + " with " + str(len(network.rows)-1) + " demands")
            demands = Demands(network.topology)
            newNetworkDemandTable = []
            rowCounter = 1
            #para cada demanda da, fazer o processo
            for row in network.rows:
                if rowCounter != 1:
                    originDemand = row[1]
                    destinationDemand = row[2]
                    for graph in NetworkAsLengthGraphs:
                        if graph.topology == network.topology:
                            shortestDistance, path = shortestPath(graph.rows, originDemand,destinationDemand)
                            for links in NetworksLinksToProcess:
                                if links.topology == network.topology:
                                    noise = noiseOfPath(links.rows,path)
                    feasibleTransponders = []
                    #print(path) #check shortest path 
                    
                    rowCounter2 = 1
                    #para cada transponder da tabela, escolher os feasibles
                    #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                    #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                    for row2 in TransponderTable:
                        transponder = []
                        if rowCounter2 != 1:
                            pch = osnrPch(row2[4])
                            osnr = pch/noise
                            osnrdb = 10.0 * math.log10(osnr)
                            #print(str(osnrdb) +" x "+ row2[5])      #check if transponder is feasbile  
                            if (int(row2[7]) >= shortestDistance) and float(row2[5]) <= osnrdb:
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
                    
                    #STRATEGY TWO: PICKING A RANDOM TRANSPONDER
                    if len(feasibleTransponders)>1:
                        options = len(feasibleTransponders)
                        chosen = random.randrange(1, options)
                        row3 = feasibleTransponders[chosen]
                        chosenSlots = row3[4]
                        chosenMaxReach = row3[7]
                        chosenOsnrLim = row3[5]
                        newRow = row
                        newRow.append(chosenSlots)
                        newRow.append(chosenMaxReach)
                        newRow.append(chosenOsnrLim)
                        newNetworkDemandTable.append(newRow)
                    else:
                        print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
                else:
                    newRow = row
                    newRow.append("slots")
                    newRow.append("max_length")
                    newRow.append("osnr_limit")
                    newNetworkDemandTable.append(newRow)
                rowCounter = rowCounter + 1
            demands.demandStrategy = network.demandStrategy
            demands.transponderStrategy = "random"
            demands.rows = newNetworkDemandTable  
            NetworksDemandsSetsWithTransponders.append(demands)
    return NetworksDemandsSetsWithTransponders

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

def noiseOfPath(links, path):
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

def osnrPch(slots):
    pSatC = preCompute.PsatC
    bwdmC = preCompute.bwdmC
    Bn = preCompute.Bn
    pchDemand = preCompute.computePchCBand(float(slots),pSatC,bwdmC,Bn)
    return pchDemand


# LEVEL 3 CHOICE - LINK CAPACITY
#def buildInstanceSet():

def linkCapacityAdaptator(nbDemands):
    newCap = nbDemands * 2
    if newCap > 320:
        newCap = 320
    return newCap

'''
def lengthOSNR(links, path):
    numberOfNodes = len(path)
    counter = 0
    length = 0 
    while counter < numberOfNodes - 1:
        u = path[counter] + 1
        v = path[counter+1] + 1
        counter = counter + 1
        for link in links[1:]:
            if ((int(link[1]) == u and int(link[2]) == v) or (int(link[1]) == v and int(link[2]) == u)):
                length = length + float(link[3])
                break
    return length


def osnrRhs(slots,osnrLimit):
    pchDemand = osnrPch(slots)
    rhs = pchDemand/float(osnrLimit)
    return rhs
'''

def writeLinkFile(linkTable,topology,newCap):
    os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Links/" + str(nbDemands) + "_demandsLinks/")
    filename = "../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Links/" + str(nbDemands) + "_demandsLinks/Link.csv"
    print(filename)
    with open(filename, "w") as f:
        rowCounter = 1
        for row in linkTable:
            line = ""
            elementCounter = 0
            for element in row:
                elementCounter = elementCounter + 1
                if (elementCounter == 5) and (rowCounter!=1):
                    line = line + str(newCap) + ";"
                else:
                    line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)
            rowCounter = rowCounter + 1

def writeDemandFile(demandTable,network):
    os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + network + "/Demands/" + str(len(demandTable)-1) + "_demands/")
    filename = "../Inputs/2_NetworksAfterDemandsGenerated/" + network + "/Demands/"+ str(len(demandTable)-1) + "_demands/demands_1.csv"
    print(filename)
    with open(filename, "w") as f:
        for row in demandTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            f.write(line)


NetworksLinksToProcess, topologyList = buildNetworkLinkSet()                               #this produce a table for each link file
#CustomClassVerifier(NetworksLinksToProcess)
print("Link set created")

NetworksNodesToProcess = buildNetworkNodeSet()                                              #this produce a table for each node file
#CustomClassVerifier(NetworksNodesToProcess)
print("Node set created")

NumberOfNetworks = len(topologyList)
print(str(NumberOfNetworks) + " topologies")

for network in NetworksLinksToProcess:
    preCompute.processLinks(network.rows)                                                        #this add GN model columns in the link table
#CustomClassVerifier(NetworksLinksToProcess)
print("Attributes computed for links")

NetworkAsLengthGraphs = buildLengthGraphSet(NetworksLinksToProcess,NetworksNodesToProcess)              #this create a graph for each table of links
print("Graphs weighetd by length built")
#CustomClassVerifier(NetworkAsGraphs)

NetworkAsOSNRGraphs = buildOSNRGraphSet(NetworksLinksToProcess,NetworksNodesToProcess) 
print("Graphs weighetd by noise contribution built")
#CustomClassVerifier(NetworkAsOSNRGraphs)

TransponderTable = buildTransponderTable()
print("Transponder table built")

print("Topologies to generate demands")
for topology in topologyList:
    print("- " +topology)


#====== LEVEL ONE CHOICES - DO FOR EACH NODE SET
NetworksDemandsSets = buildBaseDemandSet(NetworksNodesToProcess)           #this create a base demand set for each table of nodes
#CustomClassVerifier(NetworksDemandsSets)

addCoreToDemandSet(NetworksNodesToProcess,NetworksDemandsSets)
#CustomClassVerifier(NetworksDemandsSets)

#====== LEVEL TWO CHOICES - DO FOR EACH DEMAND SET
NetworksDemandsSetsWithTransponders = []
print("Choosing most efficient transponders")
chooseMostEfficientTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess)
#DemandVerifier(NetworksDemandsSetsWithTransponders)
print("Choosing random transponders")
#addRandomTransponder(NetworksDemandsSetsWithTransponders,NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess)
#DemandVerifier(NetworksDemandsSetsWithTransponders)
print("Choosing low data transponders")
#addLowDataTransponder(NetworksDemandsSetsWithTransponders,NetworksDemandsSets,NetworkAsLengthGraphs,NetworkAsOSNRGraphs,TransponderTable,NetworksLinksToProcess)
DemandVerifier(NetworksDemandsSetsWithTransponders)


for network in NetworksDemandsSetsWithTransponders:
    preCompute.processDemands(network.rows)   
#DemandVerifier(NetworksDemandsSetsWithTransponders)
#====== LEVEL THREE CHOICES - DO FOR EACH DEMAND SET WITH TRANSPONDER
#instanceSet = buildInstanceSet(NetworksDemandsSetsWithTransponders,NetworksLinksToProcess)

#write outputs
outputFolder = [f.name for f in os.scandir("../Inputs/2_NetworksAfterDemandsGenerated") if f.is_dir()]

for topology in topologyList:
    if topology not in outputFolder:
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology)
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Demands/")
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Links/")
    else:
        shutil.rmtree("../Inputs/2_NetworksAfterDemandsGenerated/" + topology)
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology)
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Demands/")
        os.mkdir("../Inputs/2_NetworksAfterDemandsGenerated/" + topology + "/Links/")
    for demandSet in NetworksDemandsSetsWithTransponders:
        nbDemands = len(demandSet.rows)-1
        if demandSet.topology == topology:
            writeDemandFile(demandSet.rows,topology)
            for linkSet in NetworksLinksToProcess:
                if linkSet.topology == demandSet.topology:
                    newCap = linkCapacityAdaptator(nbDemands)
                    writeLinkFile(linkSet.rows,topology,newCap)