import math

class TRX:
    def __init__(self,tU,node):
    #gnpy param
        self.uid = "trx-" + str(tU) + "-n"+str(node)
        self.type = "Transceiver"
        #metadata #location
        self.latitude = 0
        self.longitude = 0
        self.city = ""
        self.region = ""
    #translator param
        self.typeUnity = tU      #trx number
        self.node = node
        self.ownId = 0           #equip number
        self.connectionId = 0    #equip number connected roadm

    #gnpy param
    def changeType(self,type):
        self.type = type

    def changeMetadata(self,lat,long,city,region):
        self.latitude = lat
        self.longitude = long
        self.city = city
        self.region = region

    #translator param
    def setOwnId(self,oId):
        self.ownId = oId


    def setConnectionId(self,cId):
        self.connectionId = cId

    def toJson(self):
        self

    def print(self):
        print("=================== ")
        print("Name: " + self.uid)
        print("Type: " + self.type)
        print("TRX nb: " + str(self.typeUnity)) 
        print("Localized at node: " + str(self.node))
        print("Equip nb: " + str(self.ownId))
        print("Connected to roadm equip nb: " + str(self.connectionId))
   
class ROADM:
    def __init__(self,tU,node):
    #gnpy param
        self.uid = "roadm-"+ str(node)
        self.type = "Roadm"

        #metadata #location
        self.latitude = 0
        self.longitude = 0
        self.city = ""
        self.region = ""
        #params
        self.target_pch_out_db = -20
            #restrictions
        self.preamp_variety_list = []
        self.booster_variety_list = []
        self.per_degree_pch_out_db = -20 #one for each booster with booster name

    #translator param
        self.typeUnity = tU                 #roadm number
        self.node = node
        self.ownId = 0                      #equip number    
        self.connectorsBoosterId = []       #equip number all boster leaving roadm
        self.connectedTRXId = []            #equip number all trx connected to roadm

    #gnpy param
    def changeType(self,type):
        self.type = type

    def changeMetadata(self,lat,long,city,region):
        self.latitude = lat
        self.longitude = long
        self.city = city
        self.region = region

    #translator param
    def setOwnId(self,oId):
        self.ownId = oId

    def addConnectorsBoosterId(self,cId):
        self.connectorsBoosterId.append(cId)
    
    def addConnectedTRXId(self,cId):
        self.connectedTRXId.append(cId)

    def toJson(self):
        self

    def print(self):
        print("=================== ")
        print("Name: " + self.uid)
        print("Type: " + self.type)
        print("ROADM nb: " + str(self.typeUnity)) 
        print("Localized at node: " + str(self.node))
        print("Equip nb: " + str(self.ownId))
        print("Connected to TRX equip nb: ")
        for trx in self.connectedTRXId:
            print(str(trx)+ " ")
        print("Connected to EDFA equip nb: ")
        for booster in self.connectorsBoosterId:
            print(str(booster)+ " ")


class FIBER:
    def __init__(self,tU,link,part,nbPart,aORb,u,v):
    #gnpy param
        self.uid = "fiber-" + str(link) +"-"+ aORb+"-("+str(part)+"/"+str(nbPart)+")-("+str(u)+","+str(v)+")"
        self.type = "Fiber"
        self.type_variety = "SSMF",
        #metadata #location
        self.latitude = 0
        self.longitude = 0
        self.city = ""
        self.region = ""
        #params
        self.length = 0
        self.loss_coef = 0.2
        self.length_units = "km"
        self.att_in = 0
        self.con_in = 0
        self.con_out = 0

    #translator param
        self.typeUnity = tU                    #fiber number
        self.link = link
        self.u = u
        self.v = v
        self.ownId = 0                      #equip number    
        self.connectorIn = 0                #equip number booster or EDFA
        self.connectorOut = 0               #equip number EDFA or preamp

    #gnpy param
    def changeType(self,type):
        self.type = type

    def changeTypeVariety(self,typev):
        self.type_variety = typev

    def changeMetadata(self,lat,long,city,region):
        self.latitude = lat
        self.longitude = long
        self.city = city
        self.region = region

    def setLength(self,l):
        self.length = l

    def setLossCoeff(self,l):
        self.loss_coef = l

    #translator param
    def setOwnId(self,oId):
        self.ownId = oId

    def setConnectorIn(self,cId):
        self.connectorIn = cId
    
    def setConnectorOut(self,cId):
        self.connectorOut = cId

    def toJson(self):
        self

    def print(self):
        print("=================== ")
        print("Name: " + self.uid)
        print("Type: " + self.type)
        print("Fiber nb: " + str(self.typeUnity)) 
        print("Link: " + str(self.u) + "-" + str(self.v))
        print("Equip nb: " + str(self.ownId))
        print("Length: " + str(self.length))
        print("Connector in: " + str(self.connectorIn))
        print("Connector out: " + str(self.connectorOut))
    

class EDFA:
    def __init__(self,tU,link,part,nbPart,aORb,u,v): #if preamp, part 4, if booster, part 0
    #gnpy param
        self.uid = "Edfa_fiber-" + str(link) +"-"+ aORb+"-("+str(part)+"/"+str(nbPart)+")-("+str(u)+","+str(v)+")"
        self.type = "Edfa"
        self.type_variety = "std_fixed_gain",
        #metadata #location
        self.latitude = 0
        self.longitude = 0
        self.city = ""
        self.region = ""
        #operational
        self.gain_target = 0
        self.delta_p = 0
        self.tilt_target = 0
        self.out_voa = 0

    #translator param
        self.typeUnity = tU                  #EDFA number
        self.link = link
        self.u = u
        self.v = v
        self.ownId = 0                      #equip number    
        self.connectorIn = 0                #equip number
        self.connectorOut = 0               #equip number

    #gnpy param
    def changeType(self,type):
        self.type = type

    def changeTypeVariety(self,typev):
        self.type_variety = typev

    def changeMetadata(self,lat,long,city,region):
        self.latitude = lat
        self.longitude = long
        self.city = city
        self.region = region

    def setGainTarget(self,g):
        self.gain_target = g

    #translator param
    def setOwnId(self,oId):
        self.ownId = oId

    def setConnectorIn(self,cId):
        self.connectorIn = cId
    
    def setConnectorOut(self,cId):
        self.connectorOut = cId

    def convertToPreAmp(self):
        self.uid= "Edfa_preamp_roadm" + str(self.uid[10:])

    def convertToBooster(self):
        self.uid= "Edfa_booster_roadm" + str(self.uid[10:])
        self.gain_target = 20

    def toJson(self):
        self

    def print(self):
        print("=================== ")
        print("Name: " + self.uid)
        print("Type: " + self.type)
        print("EDFA nb: " + str(self.typeUnity)) 
        print("Link: " + str(self.u) + "-" + str(self.v))
        print("Equip nb: " + str(self.ownId))
        print("Gain: " + str(self.gain_target))
        print("Connector in: " + str(self.connectorIn))
        print("Connector out: " + str(self.connectorOut))


class CONNECTION:
    def __init__(self,tU,aId,bId):
        self.from_node=aId
        self.to_node=bId
    #translator param
        self.typeUnity = tU #connection number
        self.ownId = 0 
    #translator param
    def setOwnId(self,oId):
        self.ownId = oId
    def print(self):
        print("=================== ")
        print("Connection nb " + str(self.typeUnity))
        print("Equip nb: " + str(self.ownId))
        print("Equip A nb: " + str(self.from_node))
        print("Equip B nb: " + str(self.to_node))



nodeSet = [1,2,3]
linkSet = [[1,2,120],[2,3,225]]

for node in nodeSet:
    print(node)

for link in linkSet:
    print(link)

equipList = []
nbEquips = 0
nbROADM = 0
nbTRX = 0
nbFIBER  = 0
nbEDFA = 0

connectionList = []
nbConnections = 0

#initiating roadms

for node in nodeSet:
    nbEquips = nbEquips+1
    nbROADM = nbROADM +1
    roadm = ROADM(nbROADM,node)
    roadm.setOwnId(nbEquips)
    equipList.append(roadm)

#initiating TRX
for node in nodeSet:
    nbEquips = nbEquips+1
    nbTRX = nbTRX +1
    trx = TRX(nbTRX,node)
    trx.setOwnId(nbEquips)
    for element in equipList:
        if element.type == "Roadm" and element.node == node:
            trx.setConnectionId(element.ownId)
            element.addConnectedTRXId(trx.ownId)
            nbConnections = nbConnections + 1
            connection = CONNECTION(nbConnections,trx.ownId,element.ownId)
            connectionList.append(connection)
            nbConnections = nbConnections + 1            
            connection2 = CONNECTION(nbConnections,element.ownId,trx.ownId)
            connectionList.append(connection2)

    equipList.append(trx)

#installing links
linkNb = 0
for link in linkSet:
    linkNb = linkNb + 1 
    node1 = link[0]
    node2 = link[1]
    length = link[2]
    spans = math.ceil(length/80)
    parts = spans
    #creating fiber A and boosters
        #boooster
    nbEquips = nbEquips+1
    nbEDFA = nbEDFA +1
    booster = EDFA(nbEDFA,linkNb,0,parts,"a",node1,node2)
    booster.setOwnId(nbEquips)
        #connecting booster to roadm
    for element in equipList:
        if element.type == "Roadm" and element.node == node1:
            booster.setConnectorIn(element.ownId)
            element.addConnectorsBoosterId(booster.ownId)
            nbConnections = nbConnections + 1
            connection = CONNECTION(nbConnections,element.ownId,booster.ownId)
            connectionList.append(connection)
    nbConnections = nbConnections + 1
    connection = CONNECTION(nbConnections,booster.ownId,nbEquips+1)
    connectionList.append(connection)
    booster.setConnectorOut(nbEquips+1)
    booster.convertToBooster()
    equipList.append(booster)
         #fibers and EDFA
    for part in range(1,parts+1):       
        #creating fiber
        nbEquips = nbEquips+1
        nbFIBER = nbFIBER +1
        fiber = FIBER(nbFIBER,linkNb,part,parts,"a",node1,node2)
        fiber.setOwnId(nbEquips)
        #connecting fiber to previous edfa
        fiber.setConnectorIn(nbEquips-1)
        nbConnections = nbConnections + 1
        connection = CONNECTION(nbConnections,fiber.ownId,nbEquips+1)
        connectionList.append(connection)
        fiber.setConnectorOut(nbEquips+1)
        fiber.setLength(length/parts)
        equipList.append(fiber)
        #creating edfa if not last edfa
        if (part != parts):
            nbEquips = nbEquips+1
            nbEDFA = nbEDFA +1
            edfa = EDFA(nbEDFA,linkNb,part,parts,"a",node1,node2)
            edfa.setOwnId(nbEquips)
            edfa.setConnectorIn(nbEquips-1)
            nbConnections = nbConnections + 1
            connection = CONNECTION(nbConnections,edfa.ownId,nbEquips+1)
            connectionList.append(connection)
            edfa.setConnectorOut(nbEquips+1)
            for element in equipList:
                if element.type == "Fiber" and element.connectorOut == edfa.ownId:
                    edfa.setGainTarget((length/parts)*element.loss_coef)
            equipList.append(edfa)
        #creating last edfa
        else:
            nbEquips = nbEquips+1
            nbEDFA = nbEDFA +1
            edfa = EDFA(nbEDFA,linkNb,part,parts,"a",node1,node2)
            edfa.setOwnId(nbEquips)
            edfa.setConnectorIn(nbEquips-1)
            for element in equipList:
                if element.type == "Roadm" and element.node == node2:
                    element.addConnectorsBoosterId(edfa.ownId)
                    nbConnections = nbConnections + 1
                    connection = CONNECTION(nbConnections,edfa.ownId,element.ownId)
                    connectionList.append(connection)
                    edfa.setConnectorOut(element.ownId)
            edfa.convertToPreAmp()
            for element in equipList:
                if element.type == "Fiber" and element.connectorOut == edfa.ownId:
                    edfa.setGainTarget((length/parts)*element.loss_coef)
            equipList.append(edfa)

    #creating fiber B and boosters
        #boooster
    nbEquips = nbEquips+1
    nbEDFA = nbEDFA +1
    booster = EDFA(nbEDFA,linkNb,0,parts,"b",node2,node1)
    booster.setOwnId(nbEquips)
        #connecting booster to roadm
    for element in equipList:
        if element.type == "Roadm" and element.node == node2:
            booster.setConnectorIn(element.ownId)
            element.addConnectorsBoosterId(booster.ownId)
            nbConnections = nbConnections + 1
            connection = CONNECTION(nbConnections,element.ownId,booster.ownId)
            connectionList.append(connection)
    nbConnections = nbConnections + 1
    connection = CONNECTION(nbConnections,booster.ownId,nbEquips+1)
    connectionList.append(connection)
    booster.setConnectorOut(nbEquips+1)
    booster.convertToBooster()
    equipList.append(booster)
         #fibers and EDFA
    for part in range(1,parts+1):       
        #creating fiber
        nbEquips = nbEquips+1
        nbFIBER = nbFIBER +1
        fiber = FIBER(nbFIBER,linkNb,part,parts,"b",node2,node1)
        fiber.setOwnId(nbEquips)
        #connecting fiber to previous edfa
        fiber.setConnectorIn(nbEquips-1)
        nbConnections = nbConnections + 1
        connection = CONNECTION(nbConnections,fiber.ownId,nbEquips+1)
        connectionList.append(connection)
        fiber.setConnectorOut(nbEquips+1)
        fiber.setLength(length/parts)
        equipList.append(fiber)
        #creating edfa if not last edfa
        if (part != parts):
            nbEquips = nbEquips+1
            nbEDFA = nbEDFA +1
            edfa = EDFA(nbEDFA,linkNb,part,parts,"b",node2,node1)
            edfa.setOwnId(nbEquips)
            edfa.setConnectorIn(nbEquips-1)
            nbConnections = nbConnections + 1
            connection = CONNECTION(nbConnections,edfa.ownId,nbEquips+1)
            connectionList.append(connection)
            edfa.setConnectorOut(nbEquips+1)
            for element in equipList:
                if element.type == "Fiber" and element.connectorOut == edfa.ownId:
                    edfa.setGainTarget((length/parts)*element.loss_coef)
            equipList.append(edfa)
        #creating last edfa
        else:
            nbEquips = nbEquips+1
            nbEDFA = nbEDFA +1
            edfa = EDFA(nbEDFA,linkNb,part,parts,"b",node2,node1)
            edfa.setOwnId(nbEquips)
            edfa.setConnectorIn(nbEquips-1)
            for element in equipList:
                if element.type == "Roadm" and element.node == node1:
                    element.addConnectorsBoosterId(edfa.ownId)
                    nbConnections = nbConnections + 1
                    connection = CONNECTION(nbConnections,edfa.ownId,element.ownId)
                    connectionList.append(connection)
                    edfa.setConnectorOut(element.ownId)
            edfa.convertToPreAmp()
            for element in equipList:
                if element.type == "Fiber" and element.connectorOut == edfa.ownId:
                    edfa.setGainTarget((length/parts)*element.loss_coef)
            equipList.append(edfa)


for element in equipList:
    element.print()

for element in connectionList:
    element.print()
