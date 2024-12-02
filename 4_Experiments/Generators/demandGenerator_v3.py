import csv
import os
import shutil
import random
import math
import gnModelPrecomputingTool_v3 as preCompute


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
    def __init__(self,topology,links,demands,nodes,id):
        self.links = links
        self.demands = demands
        self.topology = topology
        self.nodes = nodes
        self.slotStrategy = "none"
        self.transponderStrategy = "none"
        self.demandStrategy = "none"
        self.id = id


def CustomClassVerifier(NetworksLinksToProcess):
    for item in NetworksLinksToProcess:
        print(item.topology)
        for row in item.rows:
            print(row)

def DemandVerifier(demandList):
    for item in demandList:
        print(item.topology + " "+ item.transponderStrategy +" "+item.demandStrategy)
        for row in item.rows:
            print(row)

def InstanceVerifier(instanceList):
    for item in instanceList:
        print(item.topology + " "+ item.transponderStrategy +" "+item.demandStrategy+" "+item.slotStrategy)
        #for row in item.links:
            #print(row)
        #for row2 in item.demands:
            #print(row2)

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

def buildDispersionGraphSet(NetworksLinksToProcess,NetworksNodesToProcess,Band):
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
                if Band == "c":
                    NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = int(row[3])*int(row[10])
                    NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = int(row[3])*int(row[10])
                if Band == "l":
                    NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = int(row[3])*int(row[11])
                    NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = int(row[3])*int(row[11])
            rowCounter = rowCounter + 1
        graph.rows = NetworkGraphDistanceMatrix
        NetworkAsGraphs.append(graph)
    return NetworkAsGraphs

def buildOSNRGraphSet(NetworksLinksToProcess,NetworksNodesToProcess, Band):
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
                if Band == "c":
                    NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = float(row[7])
                    NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = float(row[7])
                if Band == "l":
                    NetworkGraphDistanceMatrix[int(row[1])-1][int(row[2])-1] = float(row[8])
                    NetworkGraphDistanceMatrix[int(row[2])-1][int(row[1])-1] = float(row[8])
            rowCounter = rowCounter + 1
        graph.rows = NetworkGraphDistanceMatrix
        NetworkAsGraphs.append(graph)
    return NetworkAsGraphs

def getPaseNodeC():
    c = preCompute.c
    h = preCompute.h
    lambdC = preCompute.lambdC
    NFC = preCompute.NFC
    Bn = preCompute.Bn
    GdbNodeC = preCompute.GdbNodeC
    paseNod = preCompute.computePaseNodeCBand(c,h,lambdC,NFC,Bn,GdbNodeC)
    return paseNod

def getPaseNodeL():
    c = preCompute.c
    h = preCompute.h
    lambdL = preCompute.lambdL
    NFL = preCompute.NFL
    Bn = preCompute.Bn
    GdbNodeL = preCompute.GdbNodeL
    paseNod = preCompute.computePaseNodeCBand(c,h,lambdL,NFL,Bn,GdbNodeL)
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
def buildBaseDemandSet(NetworksDemandsSets,NetworksNodesToProcess):
    print("Building all pairs demand set")
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
def addCoreToDemandSet(NetworksDemandsSets,NetworksNodesToProcess):
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

#THIS STRATEGY CREATES RANDOM X% RANGE SETS OF ORIGIN AND DESTINATION AND ADD TO THE INITIAL SET
def addXsampleToDemandSet(NetworksDemandsSets,NetworksNodesToProcess,X):
    print("Building pairs demand set with "+str(X)+"%" +"ratio")
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
        nbNodes = len(meshy)
        ratio = math.ceil((X*nbNodes)/100)
        for node1 in meshy:
            sample = []
            for element in meshy:
                if element != node1:
                    sample.append(element) 
            sample = random.sample(sample,k=ratio)
            for node2 in sample:
                row = []
                row.append(str(id))
                id = id + 1
                row.append(node1)
                row.append(node2)
                demandSet.append(row)    
        demands.rows = demandSet
        demands.demandStrategy = str(X) + "percent"
        NetworksDemandsSets.append(demands)
    return NetworksDemandsSets

#THIS STRATEGY CREATES N RANDOM APIR OF ORIGIN AND DESTINATION AND ADD TO THE INITIAL SET
def addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,n):
    print("Building random demand set with " + str(n) + " demands")
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
        counter = 0
        while counter < n:
            d1 = random.sample(meshy,k=1)
            d2 = random.sample(meshy,k=1)
            if d1[0] != d2[0]:
                row = []
                row.append(str(id))
                id = id + 1
                row.append(d1[0])
                row.append(d2[0])
                demandSet.append(row)
                counter = counter + 1    
        demands.rows = demandSet
        demands.demandStrategy = "fullRandomPair"+str(n)
        NetworksDemandsSets.append(demands)
    return NetworksDemandsSets

# LEVEL 2 CHOICE - TRANSPONDER
#THIS STRATEGY SELECTS THE TRANSPONDER WITH LARGER DATA/SLOT RATIO
def chooseMostEfficientTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess):
    print("Choosing most efficient transponders")
    for network in NetworksDemandsSets:
        demands = Demands(network.topology)
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda, fazer o processo
        for row in network.rows:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                for graph in NetworkAsDispersionGraphsC:
                    if graph.topology == network.topology:
                        shortestDispersionC, pathC = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseC = noiseOfPath(links.rows,pathC,"c")
                for graph in NetworkAsDispersionGraphsL:
                    if graph.topology == network.topology:
                        shortestDispersionL, pathL = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseL = noiseOfPath(links.rows,pathL,"l") 
                for graph in NetworkAsOSNRGraphsC:
                    if graph.topology == network.topology:
                        shortestNoiseC, pathosC = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseC = shortestNoiseC #+getPaseNodeC() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionC  = dispersionOfPath(links.rows,pathosC,"c")
                for graph in NetworkAsOSNRGraphsL:
                    if graph.topology == network.topology:
                        shortestNoiseL, pathosL = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseL = shortestNoiseL #+getPaseNodeL() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionL  = dispersionOfPath(links.rows,pathosL,"l")
                feasibleTranspondersC = []
                feasibleTranspondersL = []
                '''
                print("DEMAND = ", str(originDemand) + " to " + str(destinationDemand) )
                
                print("Shortest path in c band by cd = ", str(shortestDispersionC)+" with noise = ", str(noiseC))
                for node in pathC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by cd = ", str(shortestDispersionL)+" with noise = ", str(noiseL))
                for node in pathL:
                    print(str(node+1) + " ")
                print("Shortest path in c band by OSNR = ", str(shortestNoiseC)+" with dispersion = ", str(dispersionC))
                for node in pathosC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by OSNR = ", str(shortestNoiseL)+" with dispersion = ", str(dispersionL))
                for node in pathosL:
                    print(str(node+1) + " ")
                print("__________________________________________________")
                #print(path) #check shortest path 
                '''
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                for row2 in TransponderTable:
                    transponderC = []
                    transponderL = []
                    if rowCounter2 != 1:
                        pchC = osnrPchC(row2[4])
                        pchL = osnrPchL(row2[4])
                        osnrC = pchC/noiseC
                        osnrL = pchL/noiseL
                        #calculo usando shortest dispersion primeiro e osnr a partir dele
                        osnrdbC1 = 10.0 * math.log10(osnrC) 
                        osnrdbL1 = 10.0 * math.log10(osnrL)
                        dispersionC1 = shortestDispersionC
                        dispersionL1 = shortestDispersionL
                        #calculo usando shortest osnr primeiro e dispersion a partir dele
                        osnrC = pchC/shortestNoiseC
                        osnrL = pchL/shortestNoiseL
                        osnrdbC2 = 10.0 * math.log10(osnrC) 
                        osnrdbL2 = 10.0 * math.log10(osnrL)
                        dispersionC2 = dispersionC
                        dispersionL2 = dispersionL
                        '''
                        print("C band Method 1 dispersion = ", str(dispersionC1)+" with noise = ", str(osnrdbC1))
                        print("L band Method 1 dispersion = ", str(dispersionL1)+" with noise = ", str(osnrdbL1))
                        print("C band Method 2 dispersion = ", str(dispersionC2)+" with noise = ", str(osnrdbC2))
                        print("L band Method 2 dispersion = ", str(dispersionL2)+" with noise = ", str(osnrdbL2))
                        print("Slots = ", str(str(row2[4]))+" cd = ", str(row2[6])+" osnr = ", str(row2[5]))
                        '''
                        if ((int(row2[6]) >= dispersionC1) and float(row2[5]) <= osnrdbC1) or ((int(row2[6]) >= dispersionC2) and float(row2[5]) <= osnrdbC2):
                            #print("approvue in c")
                            transponderC.append(row2[0])
                            transponderC.append(row2[1])
                            transponderC.append(row2[2])
                            transponderC.append(row2[3])
                            transponderC.append(row2[4])
                            transponderC.append(row2[5])
                            transponderC.append(row2[6])
                            transponderC.append(row2[7])
                            feasibleTranspondersC.append(transponderC) 
                        if ((int(row2[6]) >= dispersionL1) and float(row2[5]) <= osnrdbL1) or ((int(row2[6]) >= dispersionL2) and float(row2[5]) <= osnrdbL2):
                            #print("approvue in l")
                            transponderL.append(row2[0])
                            transponderL.append(row2[1])
                            transponderL.append(row2[2])
                            transponderL.append(row2[3])
                            transponderL.append(row2[4])
                            transponderL.append(row2[5])
                            transponderL.append(row2[6])
                            transponderL.append(row2[7])
                            feasibleTranspondersL.append(transponderL) 
                    else:
                        transponderC.append(row2[0])
                        transponderC.append(row2[1])
                        transponderC.append(row2[2])
                        transponderC.append(row2[3])
                        transponderC.append(row2[4])
                        transponderC.append(row2[5])
                        transponderC.append(row2[6])
                        transponderC.append(row2[7])
                        transponderL.append(row2[0])
                        transponderL.append(row2[1])
                        transponderL.append(row2[2])
                        transponderL.append(row2[3])
                        transponderL.append(row2[4])
                        transponderL.append(row2[5])
                        transponderL.append(row2[6])
                        transponderL.append(row2[7])
                        feasibleTranspondersC.append(transponderC) 
                        feasibleTranspondersL.append(transponderL) 
                    rowCounter2 = rowCounter2 + 1
                
                #STRATEGY ONE: PICKING THE LARGEST DATA/SLOT RATIO POSSIBLE
                
                data = 400
                pickC = False
                pickL = False
                while data > 0 and pickC == False:
                    if len(feasibleTranspondersC)>1:
                        #for abba in feasibleTranspondersC:
                        #    print(abba)
                        rowCounter3 = 1
                        efficiency = 10
                        for row3 in feasibleTranspondersC:
                            if rowCounter3 !=1:
                                if (int(row3[4]) < efficiency) and (int(row3[1]) == data): 
                                    chosenSlotsC = row3[4]
                                    chosenMaxCDC = row3[6]
                                    chosenOsnrLimC = row3[5]
                                    efficiency = int(row3[4])
                                    transpCId = int(row3[0])
                                    pickC = True
                            rowCounter3 = rowCounter3 + 1
                        if pickC == True:
                            newRow = row.copy()
                            newRow.append(chosenSlotsC)
                            newRow.append(chosenMaxCDC)
                            newRow.append(chosenOsnrLimC)
                            newRow.append(transpCId)
                            

                            if len(feasibleTranspondersL)>1:
                                #for abba in feasibleTranspondersL:
                                #    print(abba)
                                rowCounter4 = 1
                                efficiency = 10
                                for row4 in feasibleTranspondersL:
                                    if rowCounter4 !=1:
                                        if (int(row4[4]) < efficiency) and (int(row4[1]) == data): 
                                            chosenSlotsL = row4[4]
                                            chosenMaxCDL = row4[6]
                                            chosenOsnrLimL = row4[5]
                                            efficiency = int(row4[4])
                                            transpLId = int(row4[0])
                                            pickL = True
                                    rowCounter4 = rowCounter4 + 1
                                if pickL == True:
                                    newRow.append(chosenSlotsL)
                                    newRow.append(chosenMaxCDL)
                                    newRow.append(chosenOsnrLimL)
                                    newRow.append(transpLId)
                                    newRow.append(data)
                                else:
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(data)
                            else:
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(data)
                        if pickC == True:
                            newNetworkDemandTable.append(newRow)
                        else:
                            data = data - 100
                if pickC == False:
                    print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
            else:
                newRow = row.copy()
                newRow.append("slotsC")
                newRow.append("max_cdC")
                newRow.append("osnr_limitC")
                newRow.append("TranpCId")
                newRow.append("slotsL")
                newRow.append("max_cdL")
                newRow.append("osnr_limitL")  
                newRow.append("TranspLId")
                newRow.append("Gbits/s")

                newNetworkDemandTable.append(newRow)
            rowCounter = rowCounter + 1
        demands.demandStrategy = network.demandStrategy
        demands.transponderStrategy = "efficient"
        demands.rows = newNetworkDemandTable  
        NetworksDemandsSetsWithTransponders.append(demands)
    return NetworksDemandsSetsWithTransponders

#THIS STRATEGY SELECTS THE TRANSPONDER WITH SMALL DATA CHARGE (200 OR LESS)
def addCustomDataTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess,dataLimit):
    print("Choosing custom data transponders, maximum: " + str(dataLimit) + " gbits")
    for network in NetworksDemandsSets:
        demands = Demands(network.topology)
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda, fazer o processo
        for row in network.rows:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                for graph in NetworkAsDispersionGraphsC:
                    if graph.topology == network.topology:
                        shortestDispersionC, pathC = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseC = noiseOfPath(links.rows,pathC,"c")
                for graph in NetworkAsDispersionGraphsL:
                    if graph.topology == network.topology:
                        shortestDispersionL, pathL = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseL = noiseOfPath(links.rows,pathL,"l") 
                for graph in NetworkAsOSNRGraphsC:
                    if graph.topology == network.topology:
                        shortestNoiseC, pathosC = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseC = shortestNoiseC #+getPaseNodeC() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionC  = dispersionOfPath(links.rows,pathosC,"c")
                for graph in NetworkAsOSNRGraphsL:
                    if graph.topology == network.topology:
                        shortestNoiseL, pathosL = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseL = shortestNoiseL #+getPaseNodeL() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionL  = dispersionOfPath(links.rows,pathosL,"l")
                feasibleTranspondersC = []
                feasibleTranspondersL = []
                '''
                print("DEMAND = ", str(originDemand) + " to " + str(destinationDemand) )
                
                print("Shortest path in c band by cd = ", str(shortestDispersionC)+" with noise = ", str(noiseC))
                for node in pathC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by cd = ", str(shortestDispersionL)+" with noise = ", str(noiseL))
                for node in pathL:
                    print(str(node+1) + " ")
                print("Shortest path in c band by OSNR = ", str(shortestNoiseC)+" with dispersion = ", str(dispersionC))
                for node in pathosC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by OSNR = ", str(shortestNoiseL)+" with dispersion = ", str(dispersionL))
                for node in pathosL:
                    print(str(node+1) + " ")
                print("__________________________________________________")
                #print(path) #check shortest path 
                '''
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                for row2 in TransponderTable:
                    transponderC = []
                    transponderL = []
                    if rowCounter2 != 1:
                        pchC = osnrPchC(row2[4])
                        pchL = osnrPchL(row2[4])
                        osnrC = pchC/noiseC
                        osnrL = pchL/noiseL
                        #calculo usando shortest dispersion primeiro e osnr a partir dele
                        osnrdbC1 = 10.0 * math.log10(osnrC) 
                        osnrdbL1 = 10.0 * math.log10(osnrL)
                        dispersionC1 = shortestDispersionC
                        dispersionL1 = shortestDispersionL
                        #calculo usando shortest osnr primeiro e dispersion a partir dele
                        osnrC = pchC/shortestNoiseC
                        osnrL = pchL/shortestNoiseL
                        osnrdbC2 = 10.0 * math.log10(osnrC) 
                        osnrdbL2 = 10.0 * math.log10(osnrL)
                        dispersionC2 = dispersionC
                        dispersionL2 = dispersionL
                        '''
                        print("C band Method 1 dispersion = ", str(dispersionC1)+" with noise = ", str(osnrdbC1))
                        print("L band Method 1 dispersion = ", str(dispersionL1)+" with noise = ", str(osnrdbL1))
                        print("C band Method 2 dispersion = ", str(dispersionC2)+" with noise = ", str(osnrdbC2))
                        print("L band Method 2 dispersion = ", str(dispersionL2)+" with noise = ", str(osnrdbL2))
                        print("Slots = ", str(str(row2[4]))+" cd = ", str(row2[6])+" osnr = ", str(row2[5]))
                        '''
                        if ((int(row2[6]) >= dispersionC1) and float(row2[5]) <= osnrdbC1 and int(row2[1]) <= dataLimit) or ((int(row2[6]) >= dispersionC2) and float(row2[5]) <= osnrdbC2 and int(row2[1]) <= dataLimit):
                            #print("approvue in c")
                            transponderC.append(row2[0])
                            transponderC.append(row2[1])
                            transponderC.append(row2[2])
                            transponderC.append(row2[3])
                            transponderC.append(row2[4])
                            transponderC.append(row2[5])
                            transponderC.append(row2[6])
                            transponderC.append(row2[7])
                            feasibleTranspondersC.append(transponderC) 
                        if ((int(row2[6]) >= dispersionL1) and float(row2[5]) <= osnrdbL1 and int(row2[1]) <= dataLimit) or ((int(row2[6]) >= dispersionL2) and float(row2[5]) <= osnrdbL2 and int(row2[1]) <= dataLimit):
                            #print("approvue in l")
                            transponderL.append(row2[0])
                            transponderL.append(row2[1])
                            transponderL.append(row2[2])
                            transponderL.append(row2[3])
                            transponderL.append(row2[4])
                            transponderL.append(row2[5])
                            transponderL.append(row2[6])
                            transponderL.append(row2[7])
                            feasibleTranspondersL.append(transponderL) 
                    else:
                        transponderC.append(row2[0])
                        transponderC.append(row2[1])
                        transponderC.append(row2[2])
                        transponderC.append(row2[3])
                        transponderC.append(row2[4])
                        transponderC.append(row2[5])
                        transponderC.append(row2[6])
                        transponderC.append(row2[7])
                        transponderL.append(row2[0])
                        transponderL.append(row2[1])
                        transponderL.append(row2[2])
                        transponderL.append(row2[3])
                        transponderL.append(row2[4])
                        transponderL.append(row2[5])
                        transponderL.append(row2[6])
                        transponderL.append(row2[7])
                        feasibleTranspondersC.append(transponderC) 
                        feasibleTranspondersL.append(transponderL) 
                    rowCounter2 = rowCounter2 + 1
                
                #STRATEGY ONE: PICKING THE LARGEST DATA/SLOT RATIO POSSIBLE
                
                data = dataLimit
                pickC = False
                pickL = False
                while data > 0 and pickC == False:
                    if len(feasibleTranspondersC)>1:
                        #for abba in feasibleTranspondersC:
                        #    print(abba)
                        rowCounter3 = 1
                        efficiency = 10
                        for row3 in feasibleTranspondersC:
                            if rowCounter3 !=1:
                                if (int(row3[4]) < efficiency) and (int(row3[1]) == data): 
                                    chosenSlotsC = row3[4]
                                    chosenMaxCDC = row3[6]
                                    chosenOsnrLimC = row3[5]
                                    transpCId = int(row3[0])
                                    efficiency = int(row3[4])
                                    pickC = True
                            rowCounter3 = rowCounter3 + 1
                        if pickC == True:
                            newRow = row.copy()
                            newRow.append(chosenSlotsC)
                            newRow.append(chosenMaxCDC)
                            newRow.append(chosenOsnrLimC)
                            newRow.append(transpCId)
                            
                            if len(feasibleTranspondersL)>1:
                                #for abba in feasibleTranspondersL:
                                #    print(abba)
                                rowCounter4 = 1
                                efficiency = 10
                                for row4 in feasibleTranspondersL:
                                    if rowCounter4 !=1:
                                        if (int(row4[4]) < efficiency) and (int(row4[1]) == data): 
                                            chosenSlotsL = row4[4]
                                            chosenMaxCDL = row4[6]
                                            chosenOsnrLimL = row4[5]
                                            efficiency = int(row4[4])
                                            transpLId = int(row4[0])
                                            pickL = True
                                    rowCounter4 = rowCounter4 + 1
                                if pickL == True:
                                    newRow.append(chosenSlotsL)
                                    newRow.append(chosenMaxCDL)
                                    newRow.append(chosenOsnrLimL)
                                    newRow.append(transpLId)
                                    newRow.append(data)
                                else:
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(0)
                                    newRow.append(data)
                            else:
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(0)
                                newRow.append(data)
                        if pickC == True:
                            newNetworkDemandTable.append(newRow)
                        else:
                            data = data - 100
                if pickC == False:
                    print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
            else:
                newRow = row.copy()
                newRow.append("slotsC")
                newRow.append("max_cdC")
                newRow.append("osnr_limitC")
                newRow.append("TranpCId")
                newRow.append("slotsL")
                newRow.append("max_cdL")
                newRow.append("osnr_limitL")  
                newRow.append("TranspLId")
                newRow.append("Gbits/s")
                newNetworkDemandTable.append(newRow)
            rowCounter = rowCounter + 1
        demands.demandStrategy = network.demandStrategy
        demands.transponderStrategy = "maxData"+str(dataLimit)
        demands.rows = newNetworkDemandTable  
        NetworksDemandsSetsWithTransponders.append(demands)
    return NetworksDemandsSetsWithTransponders

#THIS STRATEGY SELECTS A RANDOM TRANSPONDER
def addRandomDataTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess):
    print("Choosing random transponders")
    for network in NetworksDemandsSets:
        demands = Demands(network.topology)
        newNetworkDemandTable = []
        rowCounter = 1
        #para cada demanda, fazer o processo
        for row in network.rows:
            if rowCounter != 1:
                originDemand = row[1]
                destinationDemand = row[2]
                for graph in NetworkAsDispersionGraphsC:
                    if graph.topology == network.topology:
                        shortestDispersionC, pathC = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseC = noiseOfPath(links.rows,pathC,"c")
                for graph in NetworkAsDispersionGraphsL:
                    if graph.topology == network.topology:
                        shortestDispersionL, pathL = shortestPath(graph.rows, originDemand,destinationDemand)
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                noiseL = noiseOfPath(links.rows,pathL,"l") 
                for graph in NetworkAsOSNRGraphsC:
                    if graph.topology == network.topology:
                        shortestNoiseC, pathosC = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseC = shortestNoiseC #+getPaseNodeC() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionC  = dispersionOfPath(links.rows,pathosC,"c")
                for graph in NetworkAsOSNRGraphsL:
                    if graph.topology == network.topology:
                        shortestNoiseL, pathosL = shortestPath(graph.rows, originDemand,destinationDemand)
                        shortestNoiseL = shortestNoiseL #+getPaseNodeL() #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
                        for links in NetworksLinksToProcess:
                            if links.topology == network.topology:
                                dispersionL  = dispersionOfPath(links.rows,pathosL,"l")
                feasibleTranspondersC = []
                feasibleTranspondersL = []
                '''
                print("DEMAND = ", str(originDemand) + " to " + str(destinationDemand) )
                print("Shortest path in c band by cd = ", str(shortestDispersionC)+" with noise = ", str(noiseC))
                for node in pathC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by cd = ", str(shortestDispersionL)+" with noise = ", str(noiseL))
                for node in pathL:
                    print(str(node+1) + " ")
                print("Shortest path in c band by OSNR = ", str(shortestNoiseC)+" with dispersion = ", str(dispersionC))
                for node in pathosC:
                    print(str(node+1) + " ")
                print("Shortest path in l band by OSNR = ", str(shortestNoiseL)+" with dispersion = ", str(dispersionL))
                for node in pathosL:
                    print(str(node+1) + " ")
                print("__________________________________________________")
                #print(path) #check shortest path 
                '''
                rowCounter2 = 1
                #para cada transponder da tabela, escolher os feasibles
                #seleciono o numero de slots pra descobrir qual seria o osnr do caminho com esse transponder
                #USANDO O SHORTEST PATH COMO REFERENCIA, NAO O SHORTEST OSNR
                for row2 in TransponderTable:
                    transponderC = []
                    transponderL = []
                    if rowCounter2 != 1:
                        pchC = osnrPchC(row2[4])
                        pchL = osnrPchL(row2[4])
                        osnrC = pchC/noiseC
                        osnrL = pchL/noiseL
                        #calculo usando shortest dispersion primeiro e osnr a partir dele
                        osnrdbC1 = 10.0 * math.log10(osnrC) 
                        osnrdbL1 = 10.0 * math.log10(osnrL)
                        dispersionC1 = shortestDispersionC
                        dispersionL1 = shortestDispersionL
                        #calculo usando shortest osnr primeiro e dispersion a partir dele
                        osnrC = pchC/shortestNoiseC
                        osnrL = pchL/shortestNoiseL
                        osnrdbC2 = 10.0 * math.log10(osnrC) 
                        osnrdbL2 = 10.0 * math.log10(osnrL)
                        dispersionC2 = dispersionC
                        dispersionL2 = dispersionL
                        '''
                        print("C band Method 1 dispersion = ", str(dispersionC1)+" with noise = ", str(osnrdbC1))
                        print("L band Method 1 dispersion = ", str(dispersionL1)+" with noise = ", str(osnrdbL1))
                        print("C band Method 2 dispersion = ", str(dispersionC2)+" with noise = ", str(osnrdbC2))
                        print("L band Method 2 dispersion = ", str(dispersionL2)+" with noise = ", str(osnrdbL2))
                        print("Slots = ", str(str(row2[4]))+" cd = ", str(row2[6])+" osnr = ", str(row2[5]))
                        '''
                        if ((int(row2[6]) >= dispersionC1) and float(row2[5]) <= osnrdbC1) or ((int(row2[6]) >= dispersionC2) and float(row2[5]) <= osnrdbC2):
                            #print("approvue in c")
                            transponderC.append(row2[0])
                            transponderC.append(row2[1])
                            transponderC.append(row2[2])
                            transponderC.append(row2[3])
                            transponderC.append(row2[4])
                            transponderC.append(row2[5])
                            transponderC.append(row2[6])
                            transponderC.append(row2[7])
                            feasibleTranspondersC.append(transponderC) 
                        if ((int(row2[6]) >= dispersionL1) and float(row2[5]) <= osnrdbL1) or ((int(row2[6]) >= dispersionL2) and float(row2[5]) <= osnrdbL2):
                            #print("approvue in l")
                            transponderL.append(row2[0])
                            transponderL.append(row2[1])
                            transponderL.append(row2[2])
                            transponderL.append(row2[3])
                            transponderL.append(row2[4])
                            transponderL.append(row2[5])
                            transponderL.append(row2[6])
                            transponderL.append(row2[7])
                            feasibleTranspondersL.append(transponderL) 
                    else:
                        transponderC.append(row2[0])
                        transponderC.append(row2[1])
                        transponderC.append(row2[2])
                        transponderC.append(row2[3])
                        transponderC.append(row2[4])
                        transponderC.append(row2[5])
                        transponderC.append(row2[6])
                        transponderC.append(row2[7])
                        transponderL.append(row2[0])
                        transponderL.append(row2[1])
                        transponderL.append(row2[2])
                        transponderL.append(row2[3])
                        transponderL.append(row2[4])
                        transponderL.append(row2[5])
                        transponderL.append(row2[6])
                        transponderL.append(row2[7])
                        feasibleTranspondersC.append(transponderC) 
                        feasibleTranspondersL.append(transponderL) 
                    rowCounter2 = rowCounter2 + 1
                
                #STRATEGY ONE: PICKING THE LARGEST DATA/SLOT RATIO POSSIBLE
                
                dataVector = []
                pickC = False
                pickL = False
                #print("DEMAND = ", str(originDemand) + " to " + str(destinationDemand))
                if len(feasibleTranspondersC)>1:
                    number=1
                    for abba in feasibleTranspondersC:
                        if number != 1:
                            #print(abba[1])
                            dataVector.append(abba[1])
                        number = number + 1
                    #print(list(set(dataVector)))
                    data = random.sample(list(set(dataVector)),k=1)
                    data = data[0]
                    rowCounter3 = 1
                    efficiency = 10
                    for row3 in feasibleTranspondersC:
                        if rowCounter3 !=1:
                            if (int(row3[4]) < efficiency) and (int(row3[1]) == int(data)): 
                                chosenSlotsC = row3[4]
                                chosenMaxCDC = row3[6]
                                chosenOsnrLimC = row3[5]
                                transpCId = int(row3[0])
                                efficiency = int(row3[4])
                                pickC = True
                        rowCounter3 = rowCounter3 + 1
                    if pickC == True:
                        #print("deeeeeeeeeenis")
                        newRow = row.copy()
                        newRow.append(chosenSlotsC)
                        newRow.append(chosenMaxCDC)
                        newRow.append(chosenOsnrLimC)
                        newRow.append(transpCId)
                        if len(feasibleTranspondersL)>1:
                            rowCounter4 = 1
                            efficiency = 10
                            for row4 in feasibleTranspondersL:
                                if rowCounter4 !=1:
                                    if (int(row4[4]) < efficiency) and (int(row4[1]) == int(data)): 
                                        chosenSlotsL = row4[4]
                                        chosenMaxCDL = row4[6]
                                        chosenOsnrLimL = row4[5]
                                        efficiency = int(row4[4])
                                        transpLId = int(row4[0])
                                        pickL = True
                                rowCounter4 = rowCounter4 + 1
                        if pickL == True:
                            newRow.append(chosenSlotsL)
                            newRow.append(chosenMaxCDL)
                            newRow.append(chosenOsnrLimL)
                            newRow.append(transpLId)
                            newRow.append(data)
                        else:
                            newRow.append(0)
                            newRow.append(0)
                            newRow.append(0)
                            newRow.append(0)                            
                            newRow.append(data)
                        newNetworkDemandTable.append(newRow)
                else:
                    print("Demand " + row[0] + " from " +  row[1], "to",  row[2] + " has no feasible transponders")
            else:
                newRow = row.copy()
                newRow.append("slotsC")
                newRow.append("max_cdC")
                newRow.append("osnr_limitC")
                newRow.append("TranpCId")
                newRow.append("slotsL")
                newRow.append("max_cdL")
                newRow.append("osnr_limitL")  
                newRow.append("TranspLId")
                newRow.append("Gbits/s")
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

#RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
def noiseOfPath(links, path, Band):
    numberOfNodes = len(path)
    counter = 0
    lhs = 0 
    while counter < numberOfNodes - 1:
        u = path[counter] + 1
        v = path[counter+1] + 1
        counter = counter + 1
        for link in links[1:]:
            if ((int(link[1]) == u and int(link[2]) == v) or (int(link[1]) == v and int(link[2]) == u)):
                if Band == "c":
                    lhs = lhs + float(link[7])
                if Band == "l":
                    lhs = lhs + float(link[8])
                break
    #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    '''
    if Band == "c":
        paseNode = getPaseNodeC()
    if Band == "l":
        paseNode = getPaseNodeL()
    lhs = lhs + paseNode
    '''
    return lhs

def dispersionOfPath(links, path, Band):
    numberOfNodes = len(path)
    counter = 0
    lhs = 0 
    while counter < numberOfNodes - 1:
        u = path[counter] + 1
        v = path[counter+1] + 1
        counter = counter + 1
        for link in links[1:]:
            if ((int(link[1]) == u and int(link[2]) == v) or (int(link[1]) == v and int(link[2]) == u)):
                if Band == "c":
                    lhs = lhs + float(link[10])*float(link[3])
                if Band == "l":
                    lhs = lhs + float(link[11])*float(link[3])
                break
    return lhs

def osnrPchC(slots):
    pSatC = preCompute.pMaxC
    bwdmC = preCompute.bwdmC
    Bn = preCompute.Bn
    pchDemand = preCompute.computePchCBand(float(slots),pSatC,bwdmC,Bn)
    return pchDemand

def osnrPchL(slots):
    pMaxL = preCompute.pMaxL
    bwdmL = preCompute.bwdmL
    Bn = preCompute.Bn
    pchDemand = preCompute.computePchCBand(float(slots),pMaxL,bwdmL,Bn)
    return pchDemand


# LEVEL 3 CHOICE - LINK CAPACITY
def buildInstanceSet(NetworksDemandsSetsWithTransponders,NetworksLinksToProcess,NetworksNodesToProcess,linkPolicies):
    instanceSet = []
    instanceCounter = 0
    for demandSet in NetworksDemandsSetsWithTransponders:
        nbDemands = len(demandSet.rows)-1
        for linkSet in NetworksLinksToProcess:
            if linkSet.topology == demandSet.topology:
                for nodeSet in NetworksNodesToProcess:
                        if nodeSet.topology == linkSet.topology:
                            for policy in linkPolicies:
                                adaptedLinks = []
                                rowCounter = 1
                                for row in linkSet.rows:
                                    elementCounter = 0
                                    newRow = []
                                    for element in row:
                                        elementCounter = elementCounter + 1
                                        if ((elementCounter == 5)or(elementCounter == 6)) and (rowCounter!=1):
                                            newCap = linkCapacityAdaptator(nbDemands,policy)
                                            newRow.append(newCap)
                                        else:
                                            newRow.append(element)
                                    rowCounter = rowCounter + 1
                                    adaptedLinks.append(newRow)
                                instanceCounter = instanceCounter + 1
                                instance = Instance(demandSet.topology,adaptedLinks,demandSet.rows,nodeSet.rows,instanceCounter)
                                instance.slotStrategy = str(policy) +"x"
                                instance.transponderStrategy = demandSet.transponderStrategy
                                instance.demandStrategy = demandSet.demandStrategy
                
                                instanceSet.append(instance)
    return instanceSet

def linkCapacityAdaptator(nbDemands,policy):
    newCap = round(nbDemands * policy)
    #newCap = policy
    if newCap > 320:
        newCap = 320
    return newCap

def writeInstanceFiles(instance,adress):
    nbDemands = len(instance.demands)-1
    writeLinkFile(instance.links,instance.topology,nbDemands,adress)
    writeDemandFile(instance.demands,instance.topology,nbDemands,adress)
    writeNodeFile(instance.nodes,instance.topology,nbDemands,adress)

def writeLinkFile(linkTable,topology,nbDemands,adress):
    outputFolder = [f.name for f in os.scandir(adress+ "/Links") if f.is_dir()]
    folderTocreate = adress + "/Links/"

    candidateName = str(nbDemands) + "_demands"

    while candidateName in outputFolder:
        candidateName = candidateName + "-b"

    folderTocreate = folderTocreate + candidateName
    os.mkdir(folderTocreate)
    filename = folderTocreate + "/Link.csv"
    print(filename)
    with open(filename, "w") as f:
        rowCounter = 1
        for row in linkTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)
            rowCounter = rowCounter + 1

def writeNodeFile(nodeTable,topology,nbDemands,adress):
    outputFolder = [f.name for f in os.scandir(adress+ "/Nodes") if f.is_dir()]
    folderTocreate = adress + "/Nodes/"

    candidateName = str(nbDemands) + "_demands"

    while candidateName in outputFolder:
        candidateName = candidateName + "-b"

    folderTocreate = folderTocreate + candidateName
    os.mkdir(folderTocreate)
    filename = folderTocreate + "/Nodes.csv"
    print(filename)
    with open(filename, "w") as f:
        rowCounter = 1
        for row in nodeTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)
            rowCounter = rowCounter + 1

def writeDemandFile(demandTable,topology,nbDemands,adress):
    outputFolder = [f.name for f in os.scandir(adress+ "/Demands") if f.is_dir()]
    folderTocreate = adress + "/Demands/"

    candidateName = str(nbDemands) + "_demands"

    while candidateName in outputFolder:
        candidateName = candidateName + "-b"

    folderTocreate = folderTocreate + candidateName
    os.mkdir(folderTocreate)
    filename = folderTocreate + "/demands_1.csv"
    print(filename)
    with open(filename, "w") as f:
        rowCounter = 1
        for row in demandTable:
            line = ""
            for element in row:
                line = line + str(element) + ";"
            line = line[:-1]
            line = line + "\n"
            #print(line)
            f.write(line)
            rowCounter = rowCounter + 1


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

NetworkAsDispersionGraphsC = buildDispersionGraphSet(NetworksLinksToProcess,NetworksNodesToProcess, "c")              #this create a graph for each table of links
NetworkAsDispersionGraphsL = buildDispersionGraphSet(NetworksLinksToProcess,NetworksNodesToProcess, "l") 
print("Graphs weighetd by dispersion built")
#CustomClassVerifier(NetworkAsDispersionGraphsC)
#CustomClassVerifier(NetworkAsDispersionGraphsL)

NetworkAsOSNRGraphsC = buildOSNRGraphSet(NetworksLinksToProcess,NetworksNodesToProcess, "c") 
NetworkAsOSNRGraphsL = buildOSNRGraphSet(NetworksLinksToProcess,NetworksNodesToProcess, "l") 
print("Graphs weighetd by noise contribution built")
#CustomClassVerifier(NetworkAsOSNRGraphsC)
#CustomClassVerifier(NetworkAsOSNRGraphsL)

TransponderTable = buildTransponderTable()
print("Transponder table built")

print("Topologies to generate demands")
for topology in topologyList:
    print("- " +topology)


#====== LEVEL ONE CHOICES - DO FOR EACH NODE SET
NetworksDemandsSets = []
demandStragegylist = []
#buildBaseDemandSet(NetworksDemandsSets,NetworksNodesToProcess)           #this create a base demand set for each table of nodes
#demandStragegylist.append("allPair")
#CustomClassVerifier(NetworksDemandsSets)

#addXsampleToDemandSet(NetworksDemandsSets,NetworksNodesToProcess,20)
#demandStragegylist.append("10percent")
#CustomClassVerifier(NetworksDemandsSets)


#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,10)
#demandStragegylist.append("fullRandomPair10")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,20)
#demandStragegylist.append("fullRandomPair20")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,30)
#demandStragegylist.append("fullRandomPair30")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,40)
#demandStragegylist.append("fullRandomPair40")
addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,50)
demandStragegylist.append("fullRandomPair50")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,60)
#demandStragegylist.append("fullRandomPair60")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,70)
#demandStragegylist.append("fullRandomPair70")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,80)
#demandStragegylist.append("fullRandomPair80")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,90)
#demandStragegylist.append("fullRandomPair90")
#addFullRandomN(NetworksDemandsSets,NetworksNodesToProcess,100)
#demandStragegylist.append("fullRandomPair100")
#CustomClassVerifier(NetworksDemandsSets)

#addCoreToDemandSet(NetworksDemandsSets,NetworksNodesToProcess)
#demandStragegylist.append("coreSet")
#CustomClassVerifier(NetworksDemandsSets)


#====== LEVEL TWO CHOICES - DO FOR EACH DEMAND SET
NetworksDemandsSetsWithTransponders = []
transponderStragegylist = []

chooseMostEfficientTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess)
transponderStragegylist.append("efficient")
#DemandVerifier(NetworksDemandsSetsWithTransponders)

#addRandomDataTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess)
#transponderStragegylist.append("random")
#DemandVerifier(NetworksDemandsSetsWithTransponders)

#dataLimit = 200
#addCustomDataTransponder(NetworksDemandsSetsWithTransponders, NetworksDemandsSets,NetworkAsDispersionGraphsC, NetworkAsDispersionGraphsL ,NetworkAsOSNRGraphsC, NetworkAsOSNRGraphsL,TransponderTable,NetworksLinksToProcess,dataLimit)
#transponderStragegylist.append("maxData"+str(dataLimit))
#DemandVerifier(NetworksDemandsSetsWithTransponders)


for network in NetworksDemandsSetsWithTransponders:
    preCompute.processDemands(network.rows)   
#DemandVerifier(NetworksDemandsSetsWithTransponders)

#====== LEVEL THREE CHOICES - DO FOR EACH DEMAND SET WITH TRANSPONDER
linkPolicies = [2]
#linkPolicies = [30,40,50]
instanceSet = buildInstanceSet(NetworksDemandsSetsWithTransponders,NetworksLinksToProcess,NetworksNodesToProcess,linkPolicies)

#InstanceVerifier(instanceSet)
print(str(len(instanceSet)) + " instances sets")
#write outputs
outputFolder1 = [f.name for f in os.scandir("../Inputs/2_NetworksAfterDemandsGenerated") if f.is_dir()]
counter = 0
for linkStrategy in linkPolicies:
    address1 = "../Inputs/2_NetworksAfterDemandsGenerated/" + str(linkStrategy)+"x"
    if str(linkStrategy)+"x" not in outputFolder1:
        os.mkdir(address1)
    else:
        shutil.rmtree(address1)
        os.mkdir(address1)
    outputFolder2 = [f.name for f in os.scandir(address1) if f.is_dir()]    
    for transponderStrategy in transponderStragegylist:
        address2 = address1 + "/"+str(transponderStrategy)
        if transponderStrategy not in outputFolder2:
            os.mkdir(address2)
        else:
            shutil.rmtree(address2)
            os.mkdir(address2)
        outputFolder3 = [f.name for f in os.scandir(address2) if f.is_dir()] 
        for topology in topologyList:
            adress3 = address2 +"/" + topology 
            if topology not in outputFolder3:
                os.mkdir(adress3)
                os.mkdir(adress3+ "/Demands")
                os.mkdir(adress3 + "/Links")
                os.mkdir(adress3 + "/Nodes")
            else:
                shutil.rmtree(adress3)
                os.mkdir(adress3)
                os.mkdir(adress3 + "/Demands")
                os.mkdir(adress3 + "/Links")
                os.mkdir(adress3 + "/Nodes")
            
            for instance in instanceSet:
                #if len(instance.demands) >1 and len(instance.demands) <1000 and instance.topology == topology and instance.slotStrategy == str(linkStrategy) + "x" and instance.transponderStrategy == transponderStrategy:
                if instance.topology == topology and instance.slotStrategy == str(linkStrategy) + "x" and instance.transponderStrategy == transponderStrategy:
                    counter = counter + 1
                    writeInstanceFiles(instance,adress3)
print(str(counter) + " instances created")
