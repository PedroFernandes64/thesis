import csv
import os
import shutil
import random
import math
import copy

import gnModelPrecomputingTool as preCompute

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
                lhs = lhs + float(link[7]) + float(link[10])
                break
    paseNode = getPaseNode()
    lhs = lhs + (len(path) * paseNode)
    return lhs

def osnrPch(slots):
    pSat = 50 * pow(10,-3)
    bwdm = 5000 * pow(10,9)
    gwdm = pSat/bwdm
    Bn = 12.5 * pow(10,9)
    pchDemand = int(slots) * Bn * gwdm
    return pchDemand


#

def buildNetworkLinkSet():
    NetworkLinkSet = []
    linkFolderFinalNames = []
    linkFolderNames = []
    Networks = [f.name for f in os.scandir("../Inputs/3_NetworksToUpdate") if f.is_dir()]
    Destination = [f.name for f in os.scandir("../Inputs/4_NetworksAfterUpdate") if f.is_dir()]
    for Network in Networks:
        if Network not in Destination:
            os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network)
            os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network + "/Links/")
        else:
            shutil.rmtree("../Inputs/4_NetworksAfterUpdate/" + Network)
            os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network)
            os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network + "/Links/")

        Links = [f.name for f in os.scandir("../Inputs/3_NetworksToUpdate/" + Network + "/Links") if f.is_dir()]
        for LinkSet in Links:
            if LinkSet not in "../Inputs/4_NetworksAfterUpdate/" + Network + "/Links/":
                os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network + "/Links/" + LinkSet)
            linkFolderFinalNames.append("../Inputs/4_NetworksAfterUpdate/" + Network + "/Links/"+ LinkSet + "/Link.csv")
            NetworkLinkFile = "../Inputs/3_NetworksToUpdate/" + Network + "/Links/"+ LinkSet + "/Link.csv"
            linkFolderNames.append(NetworkLinkFile)
            with open(NetworkLinkFile, newline='') as csvfile: 
                rows = csv.reader(csvfile, delimiter=';', quotechar='|')
                NetworkFileLines = []
                for row in rows:
                    NetworkFileLine = []
                    column = 0
                    while column < 6:
                        NetworkFileLine.append(row[column])
                        column = column + 1
                    NetworkFileLines.append(NetworkFileLine)
            NetworkLinkSet.append(NetworkFileLines)
    return NetworkLinkSet, linkFolderNames, linkFolderFinalNames

def buildNetworkDemandSet():
    NetworkDemandSet = []
    demandFolderFinalNames = []
    demandFolderNames = []
    Networks2 = [f.name for f in os.scandir("../Inputs/3_NetworksToUpdate") if f.is_dir()]
    for Network in Networks2:    
        os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network + "/Demands/")
        Demands = [f.name for f in os.scandir("../Inputs/3_NetworksToUpdate/" + Network + "/Demands") if f.is_dir()]
        for DemandSet in Demands:
            if DemandSet not in "../Inputs/4_NetworksAfterUpdate/" + Network + "/Demands/":
                os.mkdir("../Inputs/4_NetworksAfterUpdate/" + Network + "/Demands/" + DemandSet)
            demandFolderFinalNames.append("../Inputs/4_NetworksAfterUpdate/" + Network + "/Demands/"+ DemandSet + "/demands_1.csv")
            NetworkDemandFile = "../Inputs/3_NetworksToUpdate/" + Network + "/Demands/"+ DemandSet + "/demands_1.csv"
            demandFolderNames.append(NetworkDemandFile)
            with open(NetworkDemandFile, newline='') as csvfile: 
                rows = csv.reader(csvfile, delimiter=';', quotechar='|')
                NetworkFileLines = []
                for row in rows:
                    NetworkFileLine = []
                    column = 0
                    while column < 5:
                        NetworkFileLine.append(row[column])
                        column = column + 1
                    NetworkFileLines.append(NetworkFileLine)
            NetworkDemandSet.append(NetworkFileLines)
    return NetworkDemandSet, demandFolderNames, demandFolderFinalNames
    
def writeLinkFile(linkTable,linkNames):
    for i in range(len(linkTable)):
        filename = linkNames[i]
        print(filename)
        with open(filename, "w") as f:
            for row in linkTable[i]:
                line = ""
                for element in row:
                    line = line + str(element) + ";"
                line = line[:-1]
                line = line + "\n"
                f.write(line)
        f.close

def writeDemandFile(demandTable,demandNames):
    for i in range(len(demandTable)):
        filename = demandNames[i]
        print(filename)
        with open(filename, "w") as f:
            for row in demandTable[i]:
                line = ""
                for element in row:
                    line = line + str(element) + ";"
                line = line[:-1]
                line = line + "\n"
                f.write(line)
        f.close


def buildGraphSet(NetworksLinksToProcess):
    NetworkAsGraphs = []
    netWorkNumber = 0

    for network in NetworksLinksToProcess:
        rowCounter = 1
        numberOfNodes = 0
        for row in network:
            if rowCounter > 1:
                if int(row[1]) > numberOfNodes:
                    numberOfNodes = int(row[1])
                if int(row[2]) > numberOfNodes:
                    numberOfNodes = int(row[2])
            rowCounter= rowCounter + 1

        NetworkGraphNodes = numberOfNodes
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

NetworksLinksToProcess, linkNames, linkFinalNames = buildNetworkLinkSet()
for links in NetworksLinksToProcess:
    preCompute.processLinks(links)  

NetworksDemandsToProcess, demandNames, demandFinalNames = buildNetworkDemandSet()

graphs = buildGraphSet(NetworksLinksToProcess)

NumberOfNetworks = len(NetworksLinksToProcess)
iteration = 0
while iteration < NumberOfNetworks:
    print("Creating transponder for: ", demandNames[iteration])
    rowCounter = 1
    for row in NetworksDemandsToProcess[iteration]:
        if rowCounter != 1:
            originDemand = row[1]
            destinationDemand = row[2]
            shortestDistance, path = shortestPath(graphs[iteration], originDemand,destinationDemand)
            osnrdenominator = osnrLhs(NetworksLinksToProcess[iteration],path)
            pch = osnrPch(row[3])
            osnr = pch/osnrdenominator
            osnrdb = 10.0 * math.log10(osnr)
            row.append(int(math.floor(osnrdb))) 
            if(shortestDistance > int(row[4])):
                print("ALERT DEMAND INFEASIBLE BECAUSE OF LENGTH")
                print(shortestDistance,int(row[4]),int(row[0]))
        else:
            row.append("osnr_limit")
        rowCounter = rowCounter + 1
    iteration = iteration + 1
 
for demands in NetworksDemandsToProcess:
    preCompute.processDemands(demands)  

writeLinkFile(NetworksLinksToProcess,linkFinalNames)
writeDemandFile(NetworksDemandsToProcess,demandFinalNames)