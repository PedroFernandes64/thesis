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



path = [306,172,273,180]
s = 3
pnli = pnliComputer(path)
print("NLI NOISE = ", pnli)
paseLine = paselineComputer(path)
print("LINE AMP NOISE = ", paseLine)
paseNode = len(path) * pC.computePaseNodeCBand(pC.c,pC.h,pC.lambdC,pC.NFC,pC.Bn,pC.GdbNodeC)
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