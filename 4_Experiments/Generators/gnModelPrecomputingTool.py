
#The code here gets a demand or a link file and add the gn columns

import math
import csv

def computeAmps(l): 
    amps = math.ceil(l/80.0)
    return amps

def computePnliCBand(l): 
    lambdC = 1545.0 * pow(10,-9)                     #SI meters, m       Usually nanometer (x nanometer)
    DC = 16.5                                        #NOT SI ps/(nm km)
    SI_D = DC * pow (10,-6)                          #SI s/m^2)
    c = 3.0 * pow(10,8)                              #SI meters by second, m
    beta2 = abs(SI_D) * pow(lambdC,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeff = 85 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gam = (n2/aeff) * (2*math.pi/lambdC)             #SI 1/W m
    bwdmC = 5000 * pow(10,9)                         #SI #Hz             Usually gigahertz
    Psat = 50 * pow(10,-3)                          #SI #W              Usually mW
    gwdm = Psat/bwdmC 
    alphaC = 0.2                                     #NOT SI dB/kilometer 
    a = math.log(10)*alphaC/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leff = (1.0 - math.exp(-2.0*a*Ls))/(2.0*a)           #SI #km
    leff_a = 1.0/(2.0 *a)                           #SI #km 
    gnliSpan = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdmC,2))/(math.pi*beta2*leff_a)) * Bn
    gnliFiber = gnliSpan * spans
    pnliFiber = gnliFiber
    return pnliFiber

def computePnliLBand(l): 
    lambdL = 1580.0 * pow(10,-9)                    #SI meters, m       Usually nanometer (x nanometer)
    DL = 20.0                                 #NOT SI ps/(nm km)
    SI_D = DL * pow (10,-6)                          #SI s/m^2)
    c = 3.0 * pow(10,8)                              #SI meters by second, m
    beta2 = abs(SI_D) * pow(lambdL,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeff = 85 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gam = (n2/aeff) * (2*math.pi/lambdL)             #SI 1/W m
    bwdmL = 6500 * pow(10,9)                       #SI #Hz             Usually gigahertz
    Psat = 50 * pow(10,-3)                          #SI #W              Usually mW
    gwdm = Psat/bwdmL 
    alphaL = 0.22                                 #NOT SI dB/kilometer 
    a = math.log(10)*alphaL/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leff = (1.0 - math.exp(-2.0*a*Ls))/(2.0*a)           #SI #km
    leff_a = 1.0/(2.0 *a)                           #SI #km 
    gnliSpan = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdmL,2))/(math.pi*beta2*leff_a)) * Bn
    gnliFiber = gnliSpan * spans
    pnliFiber = gnliFiber
    return pnliFiber

def computePaseCBand(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambdC = 1545.0 * pow(10,-9)                   #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nu = c/lambdC                                  #SI 
    NFC = 5.0                                      #SI dB
    nsp = (1.0/2.0) * pow(10.0,NFC/10.0)         
    alphaC = 0.2                                   #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    Gdb = alphaC * ls                              #SI #dB
    Glin = pow(10,Gdb/10)                         #LINEAR
    paseLinSpan = 2.0* h * nu * nsp * (Glin-1.0) * Bn 
    paseLinFiber = paseLinSpan * spans 
    return paseLinFiber 

def computePaseLBand(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambdL = 1580.0 * pow(10,-9)                  #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nu = c/lambdL                                  #SI 
    NFL = 5.5                                 #SI dB
    nsp = (1.0/2.0) * pow(10.0,NFL/10.0)         
    alphaL = 0.22                               #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    Gdb = alphaL * ls                              #SI #dB
    Glin = pow(10,Gdb/10)                         #LINEAR
    paseLinSpan = 2.0* h * nu * nsp * (Glin-1.0) * Bn 
    paseLinFiber = paseLinSpan * spans 
    return paseLinFiber 


def computePchCBand(s): 
    pSat = 50 * pow(10,-3)
    bwdmC = 5000 * pow(10,9)
    gwdm = pSat/bwdmC
    Bn = 12.5 * pow(10,9)
    pchDemand = s * Bn * gwdm
    return pchDemand

def computePchLBand(s): 
    pSat = 50 * pow(10,-3)
    bwdmL = 6500 * pow(10,9)
    gwdm = pSat/bwdmL
    Bn = 12.5 * pow(10,9)
    pchDemand = s * Bn * gwdm
    return pchDemand


def processLinks(table):
    rowCounter = 0
    for row in table: 
        if rowCounter == 0: 
            row.append('lineAmps')
            row.append('pnliC')
            row.append('pnliL')
            row.append('paseC')
            row.append('paseL')
            rowCounter = rowCounter + 1
        else:
            amps = computeAmps(float(row[3]))
            pnliC = computePnliCBand(math.ceil(float(row[3])*1000)/1000)
            pnliL = computePnliLBand(math.ceil(float(row[3])*1000)/1000)
            paseC = computePaseCBand(math.ceil(float(row[3])*1000)/1000)
            paseL = computePaseLBand(math.ceil(float(row[3])*1000)/1000)
            #print(pase)
            row.append(amps)
            row.append(pnliC)
            row.append(pnliL)
            row.append(paseC)
            row.append(paseL)
            rowCounter = rowCounter + 1

def processDemands(table):
    rowCounter = 0
    for row in table: 
        if rowCounter == 0:
            row.append('pchC')
            row.append('pchL')
            rowCounter = rowCounter + 1
        else:
            pchC = computePchCBand(math.floor(float(row[3])*1000)/1000)
            pchL = computePchLBand(math.floor(float(row[3])*1000)/1000)
            row.append(pchC)
            row.append(pchL)
            rowCounter = rowCounter + 1