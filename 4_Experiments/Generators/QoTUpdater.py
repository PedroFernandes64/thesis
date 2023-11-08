import csv
import os
import shutil
import random
import math
import copy
import gnModelPrecomputingTool as preCompute

#USE THIS FALE IN BASE INSTANCES ONLY WITH LINKS AND NODE TYPES IF YOU WANT TO GENERATE DEMAND SETS AND COMPUTE OPTICAL PHYSICS VALUES

def buildNetworkLinkSet():
    NetworkLinkSet = []
    linkFolderFinalNames = []
    linkFolderNames = []
    Networks = [f.name for f in os.scandir("../Inputs/NetworksToUpdate") if f.is_dir()]
    for Network in Networks:
        if Network not in "../Inputs/NetworksToGenerateExperiments/":
            os.mkdir("../Inputs/NetworksToGenerateExperiments/" + Network)
            os.mkdir("../Inputs/NetworksToGenerateExperiments/" + Network + "/Links/")
        Links = [f.name for f in os.scandir("../Inputs/NetworksToUpdate/" + Network + "/Links") if f.is_dir()]
        for LinkSet in Links:
            if LinkSet not in "../Inputs/NetworksToGenerateExperiments/" + Network + "/Links/":
                os.mkdir("../Inputs/NetworksToGenerateExperiments/" + Network + "/Links/" + LinkSet)
            linkFolderFinalNames.append("../Inputs/NetworksToGenerateExperiments/" + Network + "/Links/"+ LinkSet + "/Link.csv")
            NetworkLinkFile = "../Inputs/NetworksToUpdate/" + Network + "/Links/"+ LinkSet + "/Link.csv"
            linkFolderNames.append(NetworkLinkFile)
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
    return NetworkLinkSet, linkFolderNames, linkFolderFinalNames

def buildNetworkDemandSet():
    NetworkDemandSet = []
    demandFolderFinalNames = []
    demandFolderNames = []
    Networks2 = [f.name for f in os.scandir("../Inputs/NetworksToUpdate") if f.is_dir()]
    for Network in Networks2:    
        os.mkdir("../Inputs/NetworksToGenerateExperiments/" + Network + "/Demands/")
        Demands = [f.name for f in os.scandir("../Inputs/NetworksToUpdate/" + Network + "/Demands") if f.is_dir()]
        for DemandSet in Demands:
            if DemandSet not in "../Inputs/NetworksToGenerateExperiments/" + Network + "/Demands/":
                os.mkdir("../Inputs/NetworksToGenerateExperiments/" + Network + "/Demands/" + DemandSet)
            demandFolderFinalNames.append("../Inputs/NetworksToGenerateExperiments/" + Network + "/Demands/"+ DemandSet + "/demands_1.csv")
            NetworkDemandFile = "../Inputs/NetworksToUpdate/" + Network + "/Demands/"+ DemandSet + "/demands_1.csv"
            demandFolderNames.append(NetworkDemandFile)
            with open(NetworkDemandFile, newline='') as csvfile: 
                rows = csv.reader(csvfile, delimiter=';', quotechar='|')
                NetworkFileLines = []
                for row in rows:
                    NetworkFileLine = []
                    column = 0
                    while column < len(row):
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


NetworksLinksToProcess, linkNames, linkFinalNames = buildNetworkLinkSet()
NetworksDemandsToProcess, demandNames, demandFinalNames = buildNetworkDemandSet()

for links in NetworksLinksToProcess:
    preCompute.processLinks(links)    

for demands in NetworksDemandsToProcess:
    preCompute.processDemands(demands)  

writeLinkFile(NetworksLinksToProcess,linkFinalNames)
writeDemandFile(NetworksDemandsToProcess,demandFinalNames)