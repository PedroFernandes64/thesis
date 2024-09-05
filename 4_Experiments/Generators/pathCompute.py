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



path = [288,195,393,225,166]
s = 6
pnli = pnliComputer(path)
#print("NLI NOISE = ", pnli)
paseLine = paselineComputer(path)
#print("LINE AMP NOISE = ", paseLine)
paseNode = len(path) * pC.computePaseNodeCBand(pC.c,pC.h,pC.lambdC,pC.NFC,pC.Bn,pC.GdbNodeC) #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
#print("NODE AMP NOISE = ", paseNode)
pch = pC.computePchCBand(s,pC.pMaxC,pC.bwdmC,pC.Bn)

osnr = pch/(pnli+paseLine+paseNode)
osnrdb = 10.0 * math.log10(osnr) 
print("================")
print("OSNR (db) = ", osnrdb)
print("CHANNEL POWER = ", pch)
print("CHANNEL POWER in db = ", 10*math.log10(pch))
print("NOISE = ", pnli+paseLine+paseNode)
print("NOISE in db = ", 10*math.log10(pnli+paseLine+paseNode))
print("NLI = ", pnli)
print("NLI in db = ", 10*math.log10(pch)-10*math.log10(pnli))
print("ASE = ", paseLine+paseNode)
print("ASE in db = ", 10*math.log10(pch)-10*math.log10(paseLine+paseNode))

path = [288,195,393,225,166]
s = 4
pnli = pnliComputer(path)
#print("NLI NOISE = ", pnli)
paseLine = paselineComputer(path)
#print("LINE AMP NOISE = ", paseLine)
paseNode = len(path) * pC.computePaseNodeCBand(pC.c,pC.h,pC.lambdC,pC.NFC,pC.Bn,pC.GdbNodeC) #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
#print("NODE AMP NOISE = ", paseNode)
pch = pC.computePchCBand(s,pC.pMaxC,pC.bwdmC,pC.Bn)
osnr = pch/(pnli+paseLine+paseNode)
osnrdb = 10.0 * math.log10(osnr) 
print("================")
print("OSNR (db) = ", osnrdb)
print("CHANNEL POWER = ", pch)
print("CHANNEL POWER in db = ", 10*math.log10(pch))
print("NOISE = ", pnli+paseLine+paseNode)
print("NOISE in db = ", 10*math.log10(pnli+paseLine+paseNode))
print("NLI = ", pnli)
print("NLI in db = ", 10*math.log10(pch)-10*math.log10(pnli))
print("ASE = ", paseLine+paseNode)
print("ASE in db = ", 10*math.log10(pch)-10*math.log10(paseLine+paseNode))

path = [288,195,393,225,166]
s = 3
pnli = pnliComputer(path)
#print("NLI NOISE = ", pnli)
paseLine = paselineComputer(path)
#print("LINE AMP NOISE = ", paseLine)
paseNode = len(path)* pC.computePaseNodeCBand(pC.c,pC.h,pC.lambdC,pC.NFC,pC.Bn,pC.GdbNodeC) #RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
#print("NODE AMP NOISE = ", paseNode)
pch = pC.computePchCBand(s,pC.pMaxC,pC.bwdmC,pC.Bn)


osnr = pch/(pnli+paseLine+paseNode)
osnrdb = 10.0 * math.log10(osnr) 
print("================")
print("OSNR (db) = ", osnrdb)
print("CHANNEL POWER = ", pch)
print("CHANNEL POWER in db = ", 10*math.log10(pch))
print("NOISE = ", pnli+paseLine+paseNode)
print("NOISE in db = ", 10*math.log10(pnli+paseLine+paseNode))
print("NLI = ", pnli)
print("NLI in db = ", 10*math.log10(pch)-10*math.log10(pnli))
print("ASE = ", paseLine+paseNode)
print("ASE in db = ", 10*math.log10(pch)-10*math.log10(paseLine+paseNode))