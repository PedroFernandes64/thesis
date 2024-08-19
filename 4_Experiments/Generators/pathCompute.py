import gnModelPrecomputingTool_v3 as pC
import math

def pnliComputer(path):
    pnli = 0.0
    for element in path:
        pnli = pnli + pC.computePnliCBand(element,pC.lambdC,pC.DC,pC.c,pC.n2,pC.aeffC,pC.bwdmC,pC.pMaxC,pC.alphaC,pC.Bn)
    return pnli

def paselineComputer(path):
    paseLine = 0.0
    for element in path:
        paseLine = paseLine + pC.computePaseCBand(element,pC.c,pC.h,pC.lambdC,pC.NFC,pC.alphaC,pC.Bn)
    return paseLine



path = [288,195,281,166]
s = 6
pnli = pnliComputer(path)
print("NLI NOISE = ", pnli)
paseLine = paselineComputer(path)
print("LINE AMP NOISE = ", paseLine)
paseNode = (len(path)-1) * pC.computePaseNodeCBand(pC.c,pC.h,pC.lambdC,pC.NFC,pC.Bn,pC.GdbNodeC) #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
print("NODE AMP NOISE = ", paseNode)
pch = pC.computePchCBand(s,pC.pMaxC,pC.bwdmC,pC.Bn)
print("CHANNEL POWER = ", paseNode)

'''
osnr = pch/(paseLine+paseNode)
osnrdb = 10.0 * math.log10(osnr) 
print("OSNR PASE (db) = ", osnrdb)

osnr = pch/(pnli)
osnrdb = 10.0 * math.log10(osnr) 
print("OSNR PNLI (db) = ", osnrdb)
'''

osnr = pch/(pnli+paseLine+paseNode)
osnrdb = 10.0 * math.log10(osnr) 
print("OSNR (db) = ", osnrdb)