import math
import csv
import sys

path = ""

if len(sys.argv) >1:
    path = path +sys.argv[1]
    
class REQUEST:
    def __init__(self,tU,demand,transponder,path):
    #gnpy param
        self.request_id = tU
        self.source = "trx-"+ str(demand[1])+"-n"+ str(demand[1])
        self.destination = "trx-"+ str(demand[2])+"-n"+ str(demand[2])
        self.src_tp_id = "trx-"+ str(demand[1])+"-n"+ str(demand[1])
        self.dst_tp_id = "trx-"+ str(demand[2])+"-n"+ str(demand[2])
        self.bidirectional = "false"
        self.technology = "flexi-grid"
        self.trx_type = "transp"
        self.trx_mode = ""     
        self.spacing = 0
        self.max_nb_of_channel = 0
        self.out_power = 0.001
        self.path_bandwidth = 0
        '''
        self.trx_type = "transp"
        self.trx_mode = ""
        self.baud_rate = ""
        self.bit_rate = "Gb/s"
        self.spacing = "GHz"
        self.power = "0.0 dBm"
        self.nb_channels = 0
        self.path_bandwidth = "Gbit/s"
        '''
        self.nodes_list = []
    #translator param
        self.trxId = transponder[0]
        self.demandId = demand[0]
        self.band = path[2]
        self.jsonRows = []
        self.setAttributes(transponder)
        self.setPath(path)

    #gnpy param
    def setAttributes(self,transponder):

        self.trx_mode = transponder[1] +" Gbit/s, " +str(float(transponder[4])*12.5) +" Gbaud, " + transponder[3]+", " + transponder[4] + " slots"
        self.spacing = float(transponder[4])*12.5 *math.pow(10,9)
        self.max_nb_of_channel = math.floor(5000/float(str(float(transponder[4])*12.5)))
        self.path_bandwidth = float(transponder[1]) * math.pow(10,9)
        

    def setPath(self,path):
        nodes =path[3].split("-")
        for node in nodes:
            if node != "-":
                self.nodes_list.append("roadm-" + node)

    def toJson(self):
        row1 = "{"
        row2 = "  \"request-id\": \"" + str(self.request_id) + "\","
        row3 = "  \"source\": \""+self.source  + "\","
        row4 = "  \"destination\": \""+self.destination  + "\","
        row5 = "  \"src-tp-id\": \""+self.src_tp_id  + "\","
        row6 = "  \"dst-tp-id\": \""+self.dst_tp_id  + "\","
        row37 = "  \"bidirectional\": "+self.bidirectional + ","
        row7 = "  \"path-constraints\": {"
        row8 = "     \"te-bandwidth\": {"
        row9 = "       \"technology\": \"" + self.technology + "\","
        row10 = "       \"trx_type\": \"" + self.trx_type + "\","
        row11 = "       \"trx_mode\": \"" + self.trx_mode + "\","
        row12 = "       \"effective-freq-slot\": ["
        row13 = "         {"
        row14 = "           \"N\": null,"
        row15 = "           \"M\": null"
        row16 = "         }"
        row17 = "       ],"
        row18 = "       \"spacing\": " + str(self.spacing) + ","
        row19 = "       \"max-nb-of-channel\": " + str(self.max_nb_of_channel) + ","
        row20 = "       \"output-power\": " + str(self.out_power) + ","
        row21 = "       \"path_bandwidth\": " + str(self.path_bandwidth)
        row22 = "     }"
        row23 = "  },"
        self.jsonRows.append(row1)
        self.jsonRows.append(row2)
        self.jsonRows.append(row3)
        self.jsonRows.append(row4)
        self.jsonRows.append(row5)
        self.jsonRows.append(row6)
        self.jsonRows.append(row37)
        self.jsonRows.append(row7)
        self.jsonRows.append(row8)
        self.jsonRows.append(row9)
        self.jsonRows.append(row10)
        self.jsonRows.append(row11)
        self.jsonRows.append(row12)
        self.jsonRows.append(row13)
        self.jsonRows.append(row14)
        self.jsonRows.append(row15)
        self.jsonRows.append(row16)
        self.jsonRows.append(row17)
        self.jsonRows.append(row18)
        self.jsonRows.append(row19)
        self.jsonRows.append(row20)
        self.jsonRows.append(row21)
        self.jsonRows.append(row22)
        self.jsonRows.append(row23)
        row24 = "  \"explicit-route-objects\": {"
        row25 = "     \"route-object-include-exclude\": ["
        self.jsonRows.append(row24)
        self.jsonRows.append(row25)
        for node in self.nodes_list:
            row26 = "       {"
            row27 = "         \"index\": 0,"
            row28 = "         \"num-unnum-hop\": {"
            row29 = "           \"node-id\": \""+str(node)+"\","
            row30 = "           \"link-tp-id\": \"link-tp-id is not used\","
            row31 = "           \"hop-type\": \"STRICT\""
            row32 = "          }"
            row33 = "       },"
            self.jsonRows.append(row26)
            self.jsonRows.append(row27)
            self.jsonRows.append(row28)
            self.jsonRows.append(row29)
            self.jsonRows.append(row30)
            self.jsonRows.append(row31)
            self.jsonRows.append(row32)
            self.jsonRows.append(row33)
        self.jsonRows[len( self.jsonRows)-1] =  self.jsonRows[len( self.jsonRows)-1][:-1]  
        row34 = "      ]"
        row35 = "  }"
        row36 = "},"
        self.jsonRows.append(row34)
        self.jsonRows.append(row35)
        self.jsonRows.append(row36)
        


    def print(self):
        for row in self.jsonRows:
            print(row)

def read(file):
    demands = []
    with open(file, newline='') as csvfile: 
        rows = csv.reader(csvfile, delimiter=';', quotechar='|')
        for row in rows:
            topologyLine = []
            column = 0
            while column < len(row):
                topologyLine.append(row[column])
                column = column + 1
            demands.append(topologyLine)
    return demands

requestList = []

demands = read(path+"demands/demand_1.csv")
paths = read(path+"paths.csv")
transponders = read(path+"transponders.csv")



rowCount = 0
for row in paths:
    if rowCount > 0:
        transp = int(paths[rowCount][1])
        request = REQUEST(rowCount,demands[rowCount],transponders[transp],paths[rowCount])
        request.toJson()
        requestList.append(request)
        rowCount = rowCount+1
    else:
        rowCount = rowCount+1

for request in requestList:
    finalJson = []
    row = "{"
    rowa = " \"path-request\": ["
    finalJson.append(row)
    finalJson.append(rowa)
    for row in request.jsonRows:
        finalJson.append(row)
    finalJson[len(finalJson)-1] = finalJson[len(finalJson)-1][:-1]  
    rowb = "]"
    finalJson.append(rowb)
    rowc = "}"
    finalJson.append(rowc)
    for row in finalJson:
        print(row)