import os
import shutil
import csv

testSet = []

files = [f.name for f in os.scandir("../Outputs/Instances") if f.is_dir()]
for file in files:
    demandList = [f.name for f in os.scandir("../Outputs/Instances/" + file) if f.is_dir()]
    test =[]
    for demandNumber in demandList:
        testSet.append([file,demandNumber.replace("_demands","")])

solverSet= ["0","3"]
objSet = ["1", "1p", "2","2p","4","4p","8"]
gnSet=["0"]

with open('../Inputs/onlineParametersBase.txt', "r") as f:
    lines = f.readlines()
    f.close() 

auxOutputFolder = [f.name for f in os.scandir("../Outputs") if f.is_dir()]
if "parametersSet" not in auxOutputFolder:
    os.mkdir("../Outputs/parametersSet")
else:
    shutil.rmtree("../Outputs/parametersSet")
    os.mkdir("../Outputs/parametersSet")


counter = 1
for experiment in testSet:
    instanceName =experiment[0]
    demandsNumber = experiment[1]
    stringLinks = "topologyFile=Instances/" + instanceName + "/Link.csv" "\n" 
    stringDemands = "demandToBeRoutedFolder=Instances/" + instanceName + "/" + demandsNumber + "_demands\n"
    lines[1] = stringLinks
    lines[4] = stringDemands

    for obj in objSet:
        stringObj = "obj=" + obj + "\n"
        lines[18] = stringObj

        for solver in solverSet:
            stringSolver = "solver=" + solver + "\n"
            lines[27] = stringSolver

            for gn in gnSet:
                stringGN = "GNModel_activation=" + gn + "\n"
                lines[12] = stringGN

                parametersName = "../Outputs/parametersSet/oP" + "_i" + instanceName + "_d" + demandsNumber + "_of" + obj + "_s" + solver + "_gn" + gn + ".txt"
                with open(parametersName, "w") as f:
                    for line in lines:
                        f.write(line)
                    f.close() 
                counter = counter + 1



parameters = os.listdir("../Outputs/parametersSet")

with open("../Outputs/script.sh", "w") as f:
    stringLine = "./exec"
    for parameter in parameters:
        line = stringLine + " " + "parametersSet/" + parameter + " >> " + "executionOutputs/" + parameter[:-4] + "\n"
        f.write(line)
    f.close() 

jobsRows = []
with open("../Inputs/jobsBase.sh", newline='') as jobsFile: 
    aux = csv.reader(jobsFile, delimiter=';', quotechar='|')
    for row in aux:
        jobsRows.append(row)
    f.close() 

with open("../Outputs/jobsHPC.sh", "w") as f:
    counter = 1
    for row in jobsRows:
        if counter == 4:
            line = "#SBATCH --array=0-"+str(len(parameters)-1)+"             # création d'un tableau de "+str(len(parameters))+ " jobs indicés de 0 à "+str(len(parameters)-1)
            f.write(str(line)+"\n")
        else:
            line = row[0]
            f.write(str(line)+"\n")
        counter = counter + 1

    f.write("\n")
    stringLine1 = "tab1=("
    for parameter in parameters:
        stringLine1 = stringLine1 + "/" + parameter + " "
    stringLine1 = stringLine1 + ")\n"
    f.write(stringLine1)

    stringLine2 = "tab2=("
    for parameter in parameters:
        stringLine2 = stringLine2 + "/" + parameter[:-4] + " "
    stringLine2 = stringLine2 + ")\n"

    f.write(stringLine2)

    echoLine = "echo parametersSet${tab1[$SLURM_ARRAY_TASK_ID]}\n"
    f.write(echoLine)
    lastLine = "./exec parametersSet${tab1[$SLURM_ARRAY_TASK_ID]} >> executionOutputs${tab2[$SLURM_ARRAY_TASK_ID]}.txt" 
    f.write(lastLine)




    f.close() 

with open("../Outputs/results.csv", "w") as f:
    line = "Instance;Demands;UB;LB;GAP;Time;OF;Solver;Gn\n"
    f.write(line)
    f.close() 
    
auxOutputFolder = [f.name for f in os.scandir("../Outputs") if f.is_dir()]
if "executionOutputs" not in auxOutputFolder:
    os.mkdir("../Outputs/executionOutputs")
    with open("../Outputs/executionOutputs/about.txt", "w") as f:
        line = "outputs from hpc"
        f.write(line)
        f.close() 
else:
    shutil.rmtree("../Outputs/executionOutputs")
    os.mkdir("../Outputs/executionOutputs")
    with open("../Outputs/executionOutputs/about.txt", "w") as f:
        line = "outputs from hpc"
        f.write(line)
        f.close() 