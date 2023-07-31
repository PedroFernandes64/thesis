import math
import csv
import os


#COMPUTING DEFINITIVE#
def computeAmps(l): 
    amps = math.ceil(l/80.0)
    return amps

def computePnli(l): 
    lambd = 1545.0 * pow(10,-9)                     #SI meters, m       Usually nanometer (x nanometer)
    D = 16.5                                        #NOT SI ps/(nm km)
    SI_D = D * pow (10,-6)                          #SI s/m^2)
    c = 3.0 *pow(10,8)                              #SI meters by second, m
    beta2 = abs(SI_D) * pow(lambd,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeff = 85 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gam = (n2/aeff) * (2*math.pi/lambd)             #SI 1/W m
    bwdm = 5000 * pow(10,9)                         #SI #Hz             Usually gigahertz
    Psat = 50 * pow(10,-3)                          #SI #W              Usually mW
    gwdm = Psat/bwdm 
    alpha = 0.2                                     #NOT SI dB/kilometer 
    a = math.log(10)*alpha/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leff = (1.0 - math.exp(-2.0*a*Ls))/(2.0*a)           #SI #km
    leff_a = 1.0/(2.0 *a)                           #SI #km 
    gnliSpan = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdm,2))/(math.pi*beta2*leff_a)) * Bn
    gnliFiber = gnliSpan * spans
    pnliFiber = gnliFiber
    return pnliFiber

def computePase(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambd = 1545.0 * pow(10,-9)                   #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nu = c/lambd                                  #SI 
    NF = 5.0                                      #SI dB
    nsp = (1.0/2.0) * pow(10.0,NF/10.0)         
    alpha = 0.2                                   #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    Gdb = alpha * ls                              #SI #dB
    Glin = pow(10,Gdb/10)                         #LINEAR
    paseLinSpan = 2.0* h * nu * nsp * (Glin-1.0) * Bn 
    paseLinFiber = paseLinSpan * spans 
    return paseLinFiber 

def computePaseNode(): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambd = 1545.0 * pow(10,-9)                   #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nu = c/lambd                                  #SI 
    NF = 5.0                                      #SI dB
    nsp = (1.0/2.0) * pow(10.0,NF/10.0)         
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    Gdb = 20 
    Glin = pow(10,Gdb/10)                         #LINEAR
    paseNode = 2.0* h * nu * nsp * (Glin-1.0) * Bn 
    return paseNode 

def computePch(): 
    s = 6
    pSat = 50 * pow(10,-3)
    bwdm = 5000 * pow(10,9)
    gwdm = pSat/bwdm
    Bn = 12.5 * pow(10,9)
    pchDemand = s * Bn * gwdm
    return pchDemand

def computeOsnrConstraint(cdata):
    paseLineTotal = 0
    PNLITotal = 0
    rowCounter = 0
    for row in cdata:
        if rowCounter >0:
            paseLineTotal = paseLineTotal+cdata[rowCounter][2]
            PNLITotal = PNLITotal+cdata[rowCounter][3]
        rowCounter = rowCounter + 1

    paseNodeTotal = (len(cdata) - 1 + 1) * computePaseNode()
    pch = computePch()
    osnr = pch/(paseLineTotal+PNLITotal+paseNodeTotal)
    osnrdb = 10.0 * math.log10(osnr)
    return osnrdb

#COMPUTING POGGIOLINI#
def harmonic(n):
	total_sum = 0.0
	for i in range(1, n+1):
		total_sum = total_sum + 1/i
	return total_sum

def computePnliPoggioliniEpsilon(l): 
    lambd = 1545.0 * pow(10,-9)                     #SI meters, m       Usually nanometer (x nanometer)
    D = 16.5                                        #NOT SI ps/(nm km)
    SI_D = D * pow (10,-6)                          #SI s/m^2)
    c = 3.0 *pow(10,8)                              #SI meters by second, m
    beta2 = abs(SI_D) * pow(lambd,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeff = 85 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gam = (n2/aeff) * (2*math.pi/lambd)             #SI 1/W m
    bwdm = 5000 * pow(10,9)                         #SI #Hz             Usually gigahertz
    Psat = 50 * pow(10,-3)                          #SI #W              Usually mW
    gwdm = Psat/bwdm 
    alpha = 0.2                                     #NOT SI dB/kilometer 
    a = math.log(10)*alpha/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leff = (1.0 - math.exp(-2.0*a*Ls))/(2.0*a)           #SI #km
    leff_a = 1.0/(2.0 *a)                           #SI #km 
    gnliSpan = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdm,2))/(math.pi*beta2*leff_a)) * Bn

    #Epsilon
    if spans != 1:
        epsilon = 0.0
        epsilon =   math.log(1+((2*leff_a/spans*Ls) * ((1-spans+spans*harmonic(spans-1)) / (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdm,2))))))/(math.log(spans))
    else:
        epsilon = 0.0

    epsilon = epsilon * Bn

    epsilon2 = 0.0
    epsilon2 = (3.0/10.0) * math.log(1 + (6.0/Ls) * (leff_a/(math.asinh((pow(math.pi,2)/2)*beta2*leff_a*pow(bwdm,2)))))

    print(epsilon, epsilon2)
    gnliFiber = gnliSpan * pow(spans,1+epsilon) 
    pnliFiber = gnliFiber
    return pnliFiber

def computeOsnrPoggioepsilon(cdata):
    paseLineTotal = 0
    PNLITotal = 0
    rowCounter = 0
    for row in cdata:
        if rowCounter >0:
            paseLineTotal = paseLineTotal+cdata[rowCounter][2]
            PNLITotal = PNLITotal+cdata[rowCounter][4]
        rowCounter = rowCounter + 1

    paseNodeTotal = (len(cdata) - 1 + 1) * computePaseNode()
    pch = computePch()
    osnr = pch/(paseLineTotal+PNLITotal+paseNodeTotal)
    osnrdb = 10.0 * math.log10(osnr)
    return osnrdb

#############################################################

def compute(data):
    solution=[]
    row=[]
    row.append("length")
    row.append("amps")
    row.append("pase")
    row.append("pnliconstraint")
    row.append("pnlipogioepsilon")
    solution.append(row)
    a = 0
    newRow = []
    while a < len(data):
        if a != 0 and a != 1 and a != 2:
            newRow.append(data[a])
        a = a + 1
    for e in newRow:
        row=[]
        row.append(e)
        row.append(computeAmps(int(e)))
        row.append(computePase(int(e)))
        row.append(computePnli(int(e)))
        row.append(computePnliPoggioliniEpsilon(int(e)))
        solution.append(row)
    return solution

def sumLength(cdata):
    length = 0
    rowCounter = 0
    for row in cdata:
        if rowCounter >0:
            length = length+int(cdata[rowCounter][0])
        rowCounter = rowCounter + 1
    return length

def sumLamps(cdata):
    lamps = 0
    rowCounter = 0
    for row in cdata:
        if rowCounter >0:
            
            lamps = lamps+cdata[rowCounter][1]
        rowCounter = rowCounter + 1
    return lamps

def experiments(cData,final):

    rowSolution=[]
    rowSolution.append(len(final))
    rowSolution.append(sumLength(cData))
    rowSolution.append(sumLamps(cData))
    rowSolution.append(len(cData)-1+1)

    rowSolution.append(math.floor(computeOsnrConstraint(cData)*1000)/1000)
    rowSolution.append(math.floor(computeOsnrPoggioepsilon(cData)*1000)/1000)
    rowSolution.append((math.floor(computeOsnrConstraint(cData)*1000)/1000)-(math.floor(computeOsnrPoggioepsilon(cData)*1000)/1000))
    final.append(rowSolution)
    return final

def writefile(file,table):
    with open(file, 'w', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerows(table)

def createData():
    table2 =[]
    maxNamp = 50
    maxLamp = 50
    i = 2
    j = 1
    line = []
    line.append("Namp")
    line.append("Lamp")
    line.append("Total Length")
    table2.append(line)

    while i <= maxNamp:
        while j <= maxLamp:
            line = []
            line.append(i)
            line.append(j)
            length = j * 80
            line.append(length)
            links = i - 1
            linkVector = []
            auxLinks = 1
            distributed = 0
            while auxLinks<=links:
                linkVector.append(80)
                distributed = distributed + 80
                auxLinks = auxLinks + 1
            while distributed < length:
                a = 0
                while a < len(linkVector):
                    if distributed < length:
                        linkVector[a] = linkVector[a]+80
                        distributed = distributed + 80
                    a = a + 1

            for element in linkVector:
                line.append(element)
            table2.append(line)
            j = j + 1
        j = i
        i = i + 1
    i = 2
    with open("paths.csv", 'w', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerows(table2)

def readData():
    with open("paths.csv", newline='') as csvfile: 
        rows = csv.reader(csvfile, delimiter=';', quotechar='|')
        PathTable = []
        for row in rows:
            Path = []
            column = 0
            while column < len(row):
                Path.append(row[column])
                column = column + 1
            PathTable.append(Path)
    return PathTable

'''
data1_a = [313,95,194,79,66,80,92] #1.9.8.11.12.14.15.16
data1_b = [313,37,35,42,194,79,66,80,92] #1.9.6.7.8.11.12.14.15.16
data1_c = [160,132,352,79,66,80,92] #1.3.5.11.12.14.15.16

data2_a = [313,95,194,225,166] #1.9.8.11.13.17
data2_b = [160,132,264,281,166] #1.3.5.10.13.17
data2_c = [313,37,35,42,194,225,166] #1.9.6.7.8.11.13.17

data3_a = [159,352,79,64,80,92] #2.5.11.12.14.15.16
data3_b = [124,132,352,79,64,80,92] #2.3.5.11.12.14.15.16
data3_c = [159,183,95,194,79,64,80,92] #2.5.9.8.11.12.14.15.16

data4_a = [159,264,281,166] #2.5.10.13.17
data4_b = [159,352,225,166] #2.5.11.13.17
data4_c = [159,352,79,66,80,92,156] #2.5.11.12.14.15.16.17 

data5_a = [132,264,281,166] #3.5.10.13.17
data5_b = [132,352,225,166] #3.5.11.13.17
data5_c = [132,352,79,66,80,92,156] #3.5.11.12.14.15.16.17
'''
'''
allData.append(data1_a)
allData.append(data1_b)
allData.append(data1_c)
allData.append(data2_a)
allData.append(data2_b)
allData.append(data2_c)
allData.append(data3_a)
allData.append(data3_b)
allData.append(data3_c)
allData.append(data4_a)
allData.append(data4_b)
allData.append(data4_c)
allData.append(data5_a)
allData.append(data5_b)
allData.append(data5_c)
'''

createData()
allData = readData()

final=[]
row=[]
row.append("id")
row.append("Length")
row.append("l_amps")
row.append("n_amps")
row.append("OSNRCONSTRAINT")
row.append("OSNRPOGGIO")
row.append("DELTA")
final.append(row)

counter = 0
while counter < len(allData):
    if counter != 0:
        computedData = compute(allData[counter])
        final = experiments(computedData,final)
    counter = counter + 1
    
#print(final)
writefile("results.csv",final)



