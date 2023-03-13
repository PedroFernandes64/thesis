import math
import csv

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

def computePch(s): 
    pSat = 50 * pow(10,-3)
    bwdm = 5000 * pow(10,9)
    gwdm = pSat/bwdm
    Bn = 12.5 * pow(10,9)
    pchDemand = s * Bn * gwdm
    return pchDemand

def processLinks(file):
    with open(file + '.csv', newline='') as csvfile: 
        links = csv.reader(csvfile, delimiter=';', quotechar='|')
        newFile1 = []
        rowCounter = 0
        for row in links: 
            if rowCounter == 0: 
                row.append('lineAmps')
                row.append('pnli')
                row.append('pase')
                rowCounter = rowCounter + 1
                newFile1.append(row)
            else:
                amps = computeAmps(float(row[3]))
                pnli = computePnli(float(row[3]))
                pase = computePase(float(row[3]))
                row.append(amps)
                row.append(pnli)
                row.append(pase)
                rowCounter = rowCounter + 1
                newFile1.append(row)
        
    with open(file + 'sGN.csv', 'w', newline='') as file:
        writer = csv.writer(file, delimiter=';')
        writer.writerows(newFile1)

def processDemands(file):
    with open(file + '.csv', newline='') as csvfile: 
        demands = csv.reader(csvfile, delimiter=';', quotechar='|')
        newFile2 = []
        rowCounter = 0
        for row in demands: 
            if rowCounter == 0:
                row.append('pch')
                rowCounter = rowCounter + 1
                newFile2.append(row)
            else:
                pch = computePch(float(row[3]))
                row.append(pch)
                rowCounter = rowCounter + 1
                newFile2.append(row)

    with open(demandfile + 'GN.csv', 'w', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerows(newFile2)


#linkfile = 'Link'
demandfile = 'demands'
#processLinks(linkfile)
processDemands(demandfile)