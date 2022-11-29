import math

if __name__ == "__main__":

    #DEMANDE
    origin_demande = 0
    destination_demande = 0
    slots = 0

    #PATH
    edges = {}
    distance= 0.0

	#GN MODEL
	#PASE
    pase =0
    h = 6.62 * pow(10,-34)                      #SI Joules second, J*s
    lambd = 1545.0 * pow(10,-9)                 #SI meters, m                   #Usually nanometer (x nanometer)
    c = 3.0 *pow(10,8)                          #SI meters by second, m 
    nu = c/lambd                                #SI hertz
    NF = 5.0                                    #SI dB
    nsp = (1.0/2.0) * pow(10.0,NF/10.0)     
    alpha = 0.2                                 #NOT SI dB/kilometer 
    a = math.log(10)*alpha/20 * pow(10,-3)      #SI 1/km
    ls = 80                                     #NOT SI kilometers
    Ls = 80 * pow(10,3)                         #SI meters
    Gdb = alpha * ls                            #SI #dB
    Glin = pow(10,Gdb/10)                       #LINEAR
    Bn = 12.5 * pow(10,9)                       #SI Hertz                       #Usually gigahertz  (x ghz)
    pase = 2.0* h * nu * nsp * (Glin-1.0) * Bn

	#GNLI
    gnli =0.0
    D = 16.5                                            #NOT SI ps/(nm km)
    SI_D = D * pow (10,-6)                              #SI s/m^2)
    beta2 = abs(SI_D) * pow(lambd,2)/(2*math.pi*c)      #SI s^2/m   
    n2 = 2.7 * pow (10,-20)                             #SI m^2/W               #Usually micrometer^2
    aeff = 85 * pow (10,-12)                            #SI m^2                 #Usually micrometer^2
    gam = (n2/aeff) * (2*math.pi/lambd)                 #SI 1/W m
    bwdm = 5000 * pow(10,9)                             #SI #Hz                 #Usually gigahertz
    Psat = 50 * pow(10,-3)                              #SI #W                  #Usually mW
    gwdm = Psat/bwdm                                    #SI #W/Hz

    leff = (1.0 - math.exp(-2.0*a*Ls))/(2.0*a)          #SI #km
    leff_a = 1.0/(2.0 *a)                               #SI #km 
    gnli = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (math.asinh(pow(math.pi/2,2)*beta2*leff_a*pow(bwdm,2))/(math.pi*beta2*leff_a))

    #PNLI
    pnli = 0.0
    pnli = gnli * Bn
    pch = 0.0

    #Epsilon

    epsilon = 0.0
    epsilon = (3.0/10.0) * math.log(1 + (6.0/Ls) * (leff_a/(math.asinh((pow(math.pi,2)/2)*beta2*leff_a*pow(bwdm,2)))))
	#OSNR
    l_amp = 0
    n_amp = 0
    osnrdb = 0.0
    osnrlimdb = 20.0
    slots = int(input("Slots: "))
    n_amp = int(input("Node amplis: "))
    l_amp = int(input("Line amplis: "))
    pch = slots * Bn * gwdm
    osnr = pch/(pase * (l_amp + n_amp) + pnli * pow(l_amp,1+epsilon))
    osnrdb = 10.0 * math.log10(osnr)
    print("OSNR: ", osnrdb)
    if osnrlimdb <= osnrdb:
        print("ok, osnr lim", osnrlimdb)
    else:
        print("not ok, osnr lim", osnrlimdb)
    print(osnr, " = ", pch, "/", "(", pase, "(", l_amp, "+", n_amp, ") + ", pnli, "*", pow(l_amp,1+epsilon))
    
    print("Approximation")
    print("C1 = Pch = ", pch)
    c1 = pch
    print("C2 = Pase = ", pase)
    c2 = pase
    print("C3 = Pnli = ", pnli)
    c3 = pnli
    print("C4 = Epsilon = ", epsilon)
    c4 = epsilon
    
    osnrlim = pow(10,osnrlimdb/10)
    aux = c1/osnrlim
    normalizer = 1 #c2*c3*aux
    print("normalizador ", normalizer)
    aa = c2/normalizer
    cc = c3/normalizer
    rhs = aux/normalizer
    lhs = aa * (l_amp + n_amp) +  cc * l_amp
    print(lhs,rhs)
    if lhs <= rhs:
        print("ok")
    else:
        print("not ok")
    print("Constraint")
    print(aa ,"(Alin + Anoeud) +", cc, "Alin <=", rhs)
    print(aa ," ( ", l_amp, " + ", n_amp, " ) + ", cc, " ", l_amp, " <= ", rhs)
    print(aa * (l_amp + n_amp) +  cc * l_amp, " <= ", rhs)
    print("Testing OSNR approximation without 1 + epsilon")
    osnrtest = pch/(pase * (l_amp + n_amp) + pnli * l_amp)
    osnrtestdb = 10.0 * math.log10(osnrtest)
    print("Approached OSNR =", osnrtestdb)