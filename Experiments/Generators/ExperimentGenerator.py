import os

testSet = []

instances = [f.name for f in os.scandir("Networks") if f.is_dir()]
for instance in instances:
    demandList = [f.name for f in os.scandir("Networks/" + instance) if f.is_dir()]
    test =[]
    for demandNumber in demandList:
        testSet.append([instance,demandNumber.replace("_demands","")])

solverSet= ["0"]
objSet = ["1","2p","4","8"]

with open('onlineParametersBase.txt', "r") as f:
    lines = f.readlines()

os.mkdir("./parametersSet")

counter = 1
for experiment in testSet:
    instanceName =experiment[0]
    demandsNumber = experiment[1]
    stringLinks = "topologyFile=Instances/" + instanceName + "/Link.csv" "\n" 
    stringDemands = "demandToBeRoutedFolder=Instances/" + instanceName + "/" + demandsNumber + "_demands/\n"
    lines[1] = stringLinks
    lines[4] = stringDemands

    for obj in objSet:
        stringObj = "obj=" + obj + "\n"
        lines[15] = stringObj

        for solver in solverSet:
            stringSolver = "solver=" + solver + "\n"
            lines[24] = stringSolver

            parametersName = "parametersSet/oP" + "_i" + instanceName + "_d" + demandsNumber + "_of" + obj + "_s" + solver + ".txt"
            with open(parametersName, "w") as f:
                for line in lines:
                    f.write(line)

            counter = counter + 1

parameters = os.listdir("parametersSet")

os.mkdir("./outputs")

with open("script.sh", "w") as f:
    stringLine = "./exec"
    for parameter in parameters:
        line = stringLine + " " + "parametersSet/" + parameter + " >> " + "outputs/" + parameter[:-4] + "\n"
        f.write(line)

with open("results.csv", "w") as f:
    line = "Instance;Demands;UB;LB;GAP;Time;OF;Solver\n"
    f.write(line)
