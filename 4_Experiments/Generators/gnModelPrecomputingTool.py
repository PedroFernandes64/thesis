
#The code here gets a demand or a link file and add the gn columns

import math
import csv

def computeAmps(l): 
    amps = math.ceil(l/80.0)
    return amps

def computePnliCBand(l): 
    lambdC = 1545.0 * pow(10,-9)                     #SI meters, m       Usually nanometer (x nanometer)
    DC = 16.5                                        #NOT SI ps/(nm km)
    SI_DC = DC * pow (10,-6)                          #SI s/m^2)
    c = 3.0 * pow(10,8)                              #SI meters by second, m
    beta2C = abs(SI_DC) * pow(lambdC,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeffC = 80 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gamC = (n2/aeffC) * (2*math.pi/lambdC)             #SI 1/W m
    bwdmC = 5000 * pow(10,9)                         #SI #Hz             Usually gigahertz
    PsatC = 50 * pow(10,-3)                          #SI #W              Usually mW
    gwdmC = PsatC/bwdmC 
    alphaC = 0.2                                     #NOT SI dB/kilometer 
    aC = math.log(10)*alphaC/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leffC = (1.0 - math.exp(-2.0*aC*Ls))/(2.0*aC)           #SI #km
    leff_aC = 1.0/(2.0 *aC)                           #SI #km 
    gnliSpanC = (8.0/27.0) * pow(gamC,2) * pow(gwdmC,3) * pow(leffC,2) * (math.asinh(pow(math.pi/2,2)*beta2C*leff_aC*pow(bwdmC,2))/(math.pi*beta2C*leff_aC)) * Bn
    gnliFiberC = gnliSpanC * spans
    pnliFiberC = gnliFiberC
    return pnliFiberC

def computePnliLBand(l): 
    lambdL = 1580.0 * pow(10,-9)                    #SI meters, m       Usually nanometer (x nanometer)
    DL = 20.0                                 #NOT SI ps/(nm km)
    SI_DL = DL * pow (10,-6)                          #SI s/m^2)
    c = 3.0 * pow(10,8)                              #SI meters by second, m
    beta2L = abs(SI_DL) * pow(lambdL,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeffL = 85 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gamL = (n2/aeffL) * (2*math.pi/lambdL)             #SI 1/W m
    bwdmL = 6500 * pow(10,9)                       #SI #Hz             Usually gigahertz
    PsatL = 65 * pow(10,-3)                          #SI #W              Usually mW
    gwdmL = PsatL/bwdmL 
    alphaL = 0.22                                 #NOT SI dB/kilometer 
    aL = math.log(10)*alphaL/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leffL = (1.0 - math.exp(-2.0*aL*Ls))/(2.0*aL)           #SI #km
    leff_aL = 1.0/(2.0 *aL)                           #SI #km 
    gnliSpanL = (8.0/27.0) * pow(gamL,2) * pow(gwdmL,3) * pow(leffL,2) * (math.asinh(pow(math.pi/2,2)*beta2L*leff_aL*pow(bwdmL,2))/(math.pi*beta2L*leff_aL)) * Bn
    gnliFiberL = gnliSpanL * spans
    pnliFiberL = gnliFiberL
    return pnliFiberL

def computePnliSBand(l): 
    lambdS = 1490.0 * pow(10,-9)                    #SI meters, m       Usually nanometer (x nanometer)
    DS = 12.5                                 #NOT SI ps/(nm km)
    SI_DS = DS * pow (10,-6)                          #SI s/m^2)
    c = 3.0 * pow(10,8)                              #SI meters by second, m
    beta2S = abs(SI_DS) * pow(lambdS,2)/(2*math.pi*c)  #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
    aeffS = 75 * pow (10,-12)                        #SI m^2             Usually micrometer^2
    gamS = (n2/aeffS) * (2*math.pi/lambdS)             #SI 1/W m
    bwdmS = 6500 * pow(10,9)                       #SI #Hz             Usually gigahertz
    PsatS = 65 * pow(10,-3)                          #SI #W              Usually mW
    gwdmS = PsatS/bwdmS 
    alphaS = 0.2                                 #NOT SI dB/kilometer 
    aS = math.log(10)*alphaS/20 * pow(10,-3)               #SI 1/km            SI W/Hz
    Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                          #SI meters
    leffS = (1.0 - math.exp(-2.0*aS*Ls))/(2.0*aS)           #SI #km
    leff_aS = 1.0/(2.0 *aS)                           #SI #km 
    gnliSpanS = (8.0/27.0) * pow(gamS,2) * pow(gwdmS,3) * pow(leffS,2) * (math.asinh(pow(math.pi/2,2)*beta2S*leff_aS*pow(bwdmS,2))/(math.pi*beta2S*leff_aS)) * Bn
    gnliFiberS = gnliSpanS * spans
    pnliFiberS = gnliFiberS
    return pnliFiberS

def computePaseCBand(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambdC = 1545.0 * pow(10,-9)                   #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nuC = c/lambdC                                  #SI 
    NFC = 5.0                                      #SI dB
    nspC = (1.0/2.0) * pow(10.0,NFC/10.0)         
    alphaC = 0.2                                   #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    GdbC = alphaC * ls                              #SI #dB
    GlinC = pow(10,GdbC/10)                         #LINEAR
    paseLinSpanC = 2.0* h * nuC * nspC * (GlinC-1.0) * Bn 
    paseLinFiberC = paseLinSpanC * spans 
    return paseLinFiberC 

def computePaseLBand(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambdL = 1580.0 * pow(10,-9)                  #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nuL = c/lambdL                                  #SI 
    NFL = 5.5                                 #SI dB
    nspL = (1.0/2.0) * pow(10.0,NFL/10.0)         
    alphaL = 0.22                               #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    GdbL = alphaL * ls                              #SI #dB
    GlinL = pow(10,GdbL/10)                         #LINEAR
    paseLinSpanL = 2.0* h * nuL * nspL * (GlinL-1.0) * Bn 
    paseLinFiberL = paseLinSpanL * spans 
    return paseLinFiberL 

def computePaseSBand(l): 
    h = 6.62 * pow(10,-34)                        #SI Joules second, J*s
    lambdS = 1490.0 * pow(10,-9)                  #SI meters, m             Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                            #SI meters by second, m 
    nuS = c/lambdS                                  #SI 
    NFS = 7                                 #SI dB
    nspS = (1.0/2.0) * pow(10.0,NFS/10.0)         
    alphaS = 0.22                               #NOT SI dB/kilometer 
    Bn = 12.5 * pow(10,9)                         #SI Hertz                 Usually gigahertz  (x ghz)
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                    #NOT SI kilometers
    GdbS = alphaS * ls                              #SI #dB
    GlinS = pow(10,GdbS/10)                         #LINEAR
    paseLinSpanS = 2.0* h * nuS * nspS * (GlinS-1.0) * Bn 
    paseLinFiberS = paseLinSpanS * spans 
    return paseLinFiberS 

def computePchCBand(s): 
    pSatC = 50 * pow(10,-3)
    bwdmC = 5000 * pow(10,9)
    gwdmC = pSatC/bwdmC
    Bn = 12.5 * pow(10,9)
    pchDemandC = s * Bn * gwdmC
    return pchDemandC

def computePchLBand(s): 
    pSatL = 65 * pow(10,-3)
    bwdmL = 6500 * pow(10,9)
    gwdmL = pSatL/bwdmL
    Bn = 12.5 * pow(10,9)
    pchDemandL = s * Bn * gwdmL
    return pchDemandL

def computePchSBand(s): 
    pSatS = 65 * pow(10,-3)
    bwdmS = 6500 * pow(10,9)
    gwdmS = pSatS/bwdmS
    Bn = 12.5 * pow(10,9)
    pchDemandS = s * Bn * gwdmS
    return pchDemandS


def processLinks(table):
    rowCounter = 0
    for row in table: 
        if rowCounter == 0: 
            row.append('lineAmps')
            row.append('pnliC')
            row.append('pnliL')
            row.append('pnliS')
            row.append('paseC')
            row.append('paseL')
            row.append('paseS')
            rowCounter = rowCounter + 1
        else:
            amps = computeAmps(float(row[3]))
            pnliC = computePnliCBand(math.ceil(float(row[3])*1000)/1000)
            pnliL = computePnliLBand(math.ceil(float(row[3])*1000)/1000)
            pnliS = computePnliSBand(math.ceil(float(row[3])*1000)/1000)
            #print("pnliC: ", pnliC)
            #print("pnliL: ", pnliL)
            #print("pnliS: ", pnliS)
            paseC = computePaseCBand(math.ceil(float(row[3])*1000)/1000)
            paseL = computePaseLBand(math.ceil(float(row[3])*1000)/1000)
            paseS = computePaseSBand(math.ceil(float(row[3])*1000)/1000)
            #print(pase)
            row.append(amps)
            row.append(pnliC)
            row.append(pnliL)
            row.append(pnliS)
            row.append(paseC)
            row.append(paseL)
            row.append(paseS)
            rowCounter = rowCounter + 1

def processDemands(table):
    rowCounter = 0
    for row in table: 
        if rowCounter == 0:
            row.append('pchC')
            row.append('pchL')
            row.append('pchS')
            rowCounter = rowCounter + 1
        else:
            pchC = computePchCBand(math.floor(float(row[3])*1000)/1000)
            pchL = computePchLBand(math.floor(float(row[3])*1000)/1000)
            pchS = computePchSBand(math.floor(float(row[3])*1000)/1000)
            row.append(pchC)
            row.append(pchL)
            row.append(pchS)
            rowCounter = rowCounter + 1