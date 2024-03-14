import os
import shutil
import csv

testSet = []

files = [f.name for f in os.scandir("../Outputs/Instances") if f.is_dir()]
for file in files:
    demandList = [f.name for f in os.scandir("../Outputs/Instances/" + file +"/Demands") if f.is_dir()]
    test =[]
    for demandNumber in demandList:
        testSet.append([file,demandNumber.replace("_demands","")])

solverSet = ["0"]
formulationSet = ["0","2","3"]
objSet = ["2","2p","4","8","10","1010"]
maxReachSet=["0","1"]
osnrSet=["0","1"]
cutSet=["0","1"]
TFlowSet=["0","1","2","3","4"]

with open('../Inputs/onlineParametersBase.txt', "r") as f:
    lines = f.readlines()
    f.close() 

auxOutputFolder = [f.name for f in os.scandir("../Outputs") if f.is_dir()]
if "parametersSet" not in auxOutputFolder:
    os.mkdir("../Outputs/parametersSet")
else:
    shutil.rmtree("../Outputs/parametersSet")
    os.mkdir("../Outputs/parametersSet")

auxParameterFolder = [f.name for f in os.scandir("../Outputs/parametersSet/") if f.is_dir()]


batchs = 0
counter = 0

for experiment in testSet:
    print(experiment)
    instanceName =experiment[0]
    demandsNumber = experiment[1]
    stringLinks = "topologyFile=Instances/" + instanceName + "/Links/" + demandsNumber + "_demandsLinks" + "/Link.csv" "\n" 
    stringDemands = "demandToBeRoutedFolder=Instances/" + instanceName + "/Demands/" + demandsNumber + "_demands/\n"
    lines[1] = stringLinks
    lines[4] = stringDemands

    for obj in objSet:
        stringObj = "obj=" + obj + "\n"
        lines[22] = stringObj

        for form in formulationSet:
            stringForm = "formulation=" + form + "\n"
            lines[20] = stringForm

            for solver in solverSet:
                stringSolver = "solver=" + solver + "\n"
                lines[31] = stringSolver

                for reach in maxReachSet:
                    stringReach = "MaxReach_activation=" + reach + "\n"
                    lines[12] = stringReach

                    for osnr in osnrSet:
                        stringGN = "OSNR_activation=" + osnr + "\n"
                        lines[13] = stringGN

                        for cut in cutSet:
                            stringCut = "userCuts=" + cut  + "\n"
                            lines[21] = stringCut

                            #verifying if tflow
                            if form == '2':
                                for tflow in TFlowSet:
                                    stringTflow = "TFlow_Policy=" + tflow  + "\n"
                                    lines[15] = stringTflow

                                    #FOLDER MANAGEMENT
                                    if (counter % 400 == 0) or (counter == 0):
                                        batchs = batchs + 1
                                        currentBatch = "batch_" + str(batchs)
                                        currentBatchFolder = "../Outputs/parametersSet/" + currentBatch
                                        if currentBatch not in auxParameterFolder:
                                                os.mkdir(currentBatchFolder)
                                        else:
                                            shutil.rmtree(currentBatchFolder)
                                            os.mkdir(currentBatchFolder)
                                        print("Batch folder created")

                                    parametersName = currentBatchFolder + "/oP" + "_i" + instanceName + "_d" + demandsNumber + "_of" + obj + "_f" + form + "_mr" + reach + "_os" + osnr + "_cu" + cut + "_tf" + tflow+ ".txt"
                                    with open(parametersName, "w") as f:
                                        for line in lines:
                                            f.write(line)
                                        f.close() 
                                    counter = counter + 1
                            else:
                                stringTflow = "TFlow_Policy=0" + "\n"
                                lines[15] = stringTflow

                                #FOLDER MANAGEMENT
                                if (counter % 400 == 0) or (counter == 0):
                                    batchs = batchs + 1
                                    currentBatch = "batch_" + str(batchs)
                                    currentBatchFolder = "../Outputs/parametersSet/" + currentBatch
                                    if currentBatch not in auxParameterFolder:
                                            os.mkdir(currentBatchFolder)
                                    else:
                                        shutil.rmtree(currentBatchFolder)
                                        os.mkdir(currentBatchFolder)
                                    print("Batch folder created")

                                parametersName = currentBatchFolder + "/oP" + "_i" + instanceName + "_d" + demandsNumber + "_of" + obj + "_f" + form + "_mr" + reach + "_os" + osnr + "_cu" + cut + ".txt"
                                with open(parametersName, "w") as f:
                                    for line in lines:
                                        f.write(line)
                                    f.close() 
                                counter = counter + 1

print("Parameter Set created")
print(str(counter) + " Experiments")

batchsList = os.listdir("../Outputs/parametersSet")

for batch in batchsList:
    parameters = os.listdir("../Outputs/parametersSet/"+batch)
    jobsRows = []
    with open("../Inputs/jobsBase.sh", newline='') as jobsFile: 
        aux = csv.reader(jobsFile, delimiter=';', quotechar='|')
        for row in aux:
            jobsRows.append(row)
        f.close() 
    jobsName = "../Outputs/jobs" + batch + ".sh"
    with open(jobsName, "w") as f:
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
            stringLine1 = stringLine1 + "/" + parameter[:-4] + " "
        stringLine1 = stringLine1 + ")\n"
        f.write(stringLine1)
        echoLine = "echo parametersSet/" + batch + "/${tab1[$SLURM_ARRAY_TASK_ID]}\n"
        f.write(echoLine)
        lastLine = "./exec parametersSet/" + batch + "/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt" 
        f.write(lastLine)
        f.close() 
print("Jobs script created")

with open("../Outputs/script.sh", "w") as f:
    for batch in batchsList:
        stringLine = "sbatch " + "jobs" + batch + ".sh\n"
        f.write(stringLine)
f.close() 
print("Experiments script created")

with open("../Outputs/results.csv", "w") as f:
    line = "Instance;Demands;UB;LB;GAP;Time;OF;Formulation;Reach;OSNR;Cuts;Variables;Constraints;Paths;Feasible;Infeasible;OnlyOsnr;OnlyReach"
    f.write(line)
    f.close() 
print("Result table created")   

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
print("Execution output folder created")   