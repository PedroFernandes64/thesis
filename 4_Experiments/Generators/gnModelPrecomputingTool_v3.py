#CONSTANTS C BAND
lambdC = 1547.0 * pow(10,-9)                    #SI meters, m       Usually nanometer (x nanometer)
aeffC = 80 * pow (10,-12)                       #SI m^2             Usually micrometer^2
bwdmC = 5000 * pow(10,9)                        #SI #Hz             Usually gigahertz
#PsatC = 50 * pow(10,-3)                         #SI #W              Usually mW
alphaC = 0.2                                    #NOT SI dB/kilometer 
NFC = 5.0                                       #SI dB
DC = 17                                         #NOT SI ps/(nm km)
GdbNodeC = 20.0                                 #SI dB
pMaxC = 50 * pow(10,-3)

#CONSTANTS L BAND
lambdL = 1580.0 * pow(10,-9)                    #SI meters, m       Usually nanometer (x nanometer)
aeffL = 85 * pow (10,-12)                       #SI m^2             Usually micrometer^2
bwdmL = 6500 * pow(10,9)                        #SI #Hz             Usually gigahertz
pMaxL = 65 * pow(10,-3)                         #SI #W              Usually mW
alphaL = 0.22                                   #NOT SI dB/kilometer 
NFL = 5.5                                       #SI dB
DL = 20.0    
GdbNodeL = 20.0                                    #NOT SI ps/(nm km)
pMaxL = 65 * pow(10,-3)

#CONSTANTS S BAND
lambdS = 1490.0 * pow(10,-9)                    #SI meters, m             Usually nanometer (x nanometer)
aeffS = 75 * pow (10,-12)                       #SI m^2             Usually micrometer^2
bwdmS = 6500 * pow(10,9)                        #SI #Hz             Usually gigahertz
pMaxS = 65 * pow(10,-3)
alphaS = 0.22                                   #NOT SI dB/kilometer 
NFS = 7                                         #SI dB
DS = 12.5                                       #NOT SI ps/(nm km)
GdbNodeS = 20.0                                 #SI dB
pMaxS = 65 * pow(10,-3)                         #SI #W              Usually mW

#CONSTANTS 
c = 3.0 * pow(10,8)                             #SI meters by second, m
n2 = 2.7 * pow (10,-20)                         #SI m^2/W           Usually micrometer^2
Bn = 12.5 * pow(10,9)                           #SI Hertz           Usually gigahertz  (x ghz)
h = 6.62 * pow(10,-34)                          #SI Joules second, J*s

#The code here gets a demand or a link file and add the gn columns
import math
import csv

def computeAmps(l): 
    amps = math.ceil(l/80.0)
    return amps

def computePnliCBand(l,lambdC,DC,c,n2,aeffC,bwdmC,pMaxC,alphaC,Bn): 
    SI_DC = DC * pow (10,-6)                                #SI s/m^2)
    beta2C = abs(SI_DC) * pow(lambdC,2)/(2*math.pi*c)       #SI s^2/m   
    gamC = (n2/aeffC) * (2*math.pi/lambdC)                  #SI 1/W m
    gwdmC = pMaxC/bwdmC 
    aC = math.log(10)*alphaC/20 * pow(10,-3)                #SI 1/km            SI W/Hz
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                                  #SI meters
    leffC = (1.0 - math.exp(-2.0*aC*Ls))/(2.0*aC)           #SI #km
    leff_aC = 1.0/(2.0 *aC)                                 #SI #km 
    gnliSpanC = (8.0/27.0) * pow(gamC,2) * pow(gwdmC,3) * pow(leffC,2) * (math.asinh(pow(math.pi/2,2)*beta2C*leff_aC*pow(bwdmC,2))/(math.pi*beta2C*leff_aC)) * Bn
    gnliFiberC = gnliSpanC * spans
    pnliFiberC = gnliFiberC
    return pnliFiberC

def computePaseCBand(l,c,h,lambdC,NFC,alphaC,Bn): 
    nuC = c/lambdC                                          #SI 
    nspC = (1.0/2.0) * pow(10.0,NFC/10.0)    
    spans = math.ceil(l/80.0)
    lspan = l/spans 
    ls = lspan                                              #NOT SI kilometers
    GdbC = alphaC * ls                                      #SI #dB
    GlinC = pow(10,GdbC/10)                                 #LINEAR
    paseLinSpanC = 2.0* h * nuC * nspC * (GlinC) * Bn 
    paseLinFiberC = paseLinSpanC * spans 
    return paseLinFiberC 

def computePchCBand(s,pMaxC,bwdmC,Bn): 
    gwdmC = pMaxC/bwdmC
    pchDemandC = s * Bn * gwdmC
    return pchDemandC

def computePaseNodeCBand(c,h,lambdC,NFC,Bn,GdbNodeC): 
    nuC = c/lambdC                                          #SI 
    nspC = (1.0/2.0) * pow(10.0,NFC/10.0)    
    GlinC = pow(10,GdbNodeC/10)                             #LINEAR
    paseNodeC = 2.0* h * nuC * nspC * (GlinC) * Bn 
    return paseNodeC 

def computePnliLBand(l,lambdL,DL,c,n2,aeffL,bwdmL,pMaxL,alphaL,Bn): 
    SI_DL = DL * pow (10,-6)                                #SI s/m^2)
    beta2L = abs(SI_DL) * pow(lambdL,2)/(2*math.pi*c)       #SI s^2/m   
    gamL = (n2/aeffL) * (2*math.pi/lambdL)                  #SI 1/W m
    gwdmL = pMaxL/bwdmL 
    aL = math.log(10)*alphaL/20 * pow(10,-3)                #SI 1/km            SI W/Hz
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                                  #SI meters
    leffL = (1.0 - math.exp(-2.0*aL*Ls))/(2.0*aL)           #SI #km
    leff_aL = 1.0/(2.0 *aL)                                 #SI #km 
    gnliSpanL = (8.0/27.0) * pow(gamL,2) * pow(gwdmL,3) * pow(leffL,2) * (math.asinh(pow(math.pi/2,2)*beta2L*leff_aL*pow(bwdmL,2))/(math.pi*beta2L*leff_aL)) * Bn
    gnliFiberL = gnliSpanL * spans
    pnliFiberL = gnliFiberL
    return pnliFiberL

def computePaseLBand(l,c,h,lambdL,NFL,alphaL,Bn): 
    nuL = c/lambdL                                          #SI 
    nspL = (1.0/2.0) * pow(10.0,NFL/10.0)         
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                              #NOT SI kilometers
    GdbL = alphaL * ls                                      #SI #dB
    GlinL = pow(10,GdbL/10)                                 #LINEAR
    paseLinSpanL = 2.0* h * nuL * nspL * (GlinL) * Bn 
    paseLinFiberL = paseLinSpanL * spans 
    return paseLinFiberL 

def computePchLBand(s,pMaxL,bwdmL,Bn): 
    gwdmL = pMaxL/bwdmL
    pchDemandL = s * Bn * gwdmL
    return pchDemandL

def computePaseNodeLBand(c,h,lambdL,NFL,Bn,GdbNodeL): 
    nuL = c/lambdL                                          #SI 
    nspL = (1.0/2.0) * pow(10.0,NFL/10.0)    
    GlinL = pow(10,GdbNodeL/10)                             #LINEAR
    paseNodeL = 2.0* h * nuL * nspL * (GlinL) * Bn 
    return paseNodeL 


def computePnliSBand(l,lambdS,DS,c,n2,aeffS,bwdmS,pMaxS,alphaS,Bn): 
    SI_DS = DS * pow (10,-6)                                #SI s/m^2)
    beta2S = abs(SI_DS) * pow(lambdS,2)/(2*math.pi*c)       #SI s^2/m   
    gamS = (n2/aeffS) * (2*math.pi/lambdS)                  #SI 1/W m
    gwdmS = pMaxS/bwdmS 
    aS = math.log(10)*alphaS/20 * pow(10,-3)                #SI 1/km            SI W/Hz
    spans = math.ceil(l/80.0)
    lspan = l/spans
    Ls = lspan * pow(10,3)                                  #SI meters
    leffS = (1.0 - math.exp(-2.0*aS*Ls))/(2.0*aS)           #SI #km
    leff_aS = 1.0/(2.0 *aS)                                 #SI #km 
    gnliSpanS = (8.0/27.0) * pow(gamS,2) * pow(gwdmS,3) * pow(leffS,2) * (math.asinh(pow(math.pi/2,2)*beta2S*leff_aS*pow(bwdmS,2))/(math.pi*beta2S*leff_aS)) * Bn
    gnliFiberS = gnliSpanS * spans
    pnliFiberS = gnliFiberS
    return pnliFiberS


def computePaseSBand(l,c,h,lambdS,NFS,alphaS,Bn):  
    nuS = c/lambdS                                          #SI 
    nspS = (1.0/2.0) * pow(10.0,NFS/10.0)         
    spans = math.ceil(l/80.0) 
    lspan = l/spans 
    ls = lspan                                              #NOT SI kilometers
    GdbS = alphaS * ls                                      #SI #dB
    GlinS = pow(10,GdbS/10)                                 #LINEAR
    paseLinSpanS = 2.0* h * nuS * nspS * (GlinS) * Bn 
    paseLinFiberS = paseLinSpanS * spans 
    return paseLinFiberS 

def computePchSBand(s,pMaxS,bwdmS,Bn): 
    gwdmS = pMaxS/bwdmS
    pchDemandS = s * Bn * gwdmS
    return pchDemandS

def computePaseNodeSBand(c,h,lambdS,NFS,Bn,GdbNodeS): 
    nuS = c/lambdS                                          #SI 
    nspS = (1.0/2.0) * pow(10.0,NFS/10.0)    
    GlinS = pow(10,GdbNodeS/10)                             #LINEAR
    paseNodeS = 2.0* h * nuS * nspS* (GlinS) * Bn 
    return paseNodeS 

def processLinks(table):
    rowCounter = 0
    for row in table: 
        if rowCounter == 0: 
            row.append('lineAmps')
            row.append('noiseC')
            row.append('noiseL')
            row.append('noiseS')
            row.append('dispC')
            row.append('dispL')
            row.append('dispS')
            rowCounter = rowCounter + 1
        else:
            amps = computeAmps(float(row[3]))
            pnliC = computePnliCBand(float(row[3]),lambdC,DC,c,n2,aeffC,bwdmC,pMaxC,alphaC,Bn)
            paseC = computePaseCBand(float(row[3]),c,h,lambdC,NFC,alphaC,Bn)
            pnliL = computePnliLBand(float(row[3]),lambdL,DL,c,n2,aeffL,bwdmL,pMaxL,alphaL,Bn)
            paseL = computePaseLBand(float(row[3]),c,h,lambdL,NFL,alphaL,Bn)
            pnliS = computePnliSBand(float(row[3]),lambdS,DS,c,n2,aeffS,bwdmS,pMaxS,alphaS,Bn)
            paseS = computePaseSBand(float(row[3]),c,h,lambdS,NFS,alphaS,Bn)
            row.append(amps)
            row.append(pnliC+paseC+computePaseNodeCBand(c,h,lambdC,NFC,Bn,GdbNodeC)) #aqui adiciono pnli, paseC é o pase ampli linha e o pase node c band é o ampli da origem do link
            row.append(pnliL+paseL+computePaseNodeLBand(c,h,lambdL,NFL,Bn,GdbNodeL))
            row.append(pnliS+paseS+computePaseNodeSBand(c,h,lambdS,NFS,Bn,GdbNodeS))
            row.append(DC)
            row.append(DL)
            row.append(DS)
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
            pchC = computePchCBand(float(row[3]),pMaxC,bwdmC,Bn)
            pchL = computePchLBand(float(row[3]),pMaxL,bwdmL,Bn)
            pchS = computePchSBand(float(row[3]),pMaxS,bwdmS,Bn)
            row.append(pchC)
            row.append(pchL)
            row.append(pchS)
            rowCounter = rowCounter + 1