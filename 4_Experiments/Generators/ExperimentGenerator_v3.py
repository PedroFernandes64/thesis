import os
import shutil
import csv

class TestUnit:
    def __init__(self,linkStrategy,transponderStrategy,topology,demandCode):
        self.topology = topology
        self.linkStrategy = linkStrategy
        self.transponderStrategy = transponderStrategy
        self.demandCode = demandCode

def testUnitVerifier(testSet):
    for test in testSet:
        print(test.linkStrategy + " "+ test.transponderStrategy +" "+test.topology+" "+test.demandCode)
        
testSet = []

linkStrategies = [f.name for f in os.scandir("../Outputs/Instances") if f.is_dir()]
for linkStrategy in linkStrategies:
    transponderStrategies = [f.name for f in os.scandir("../Outputs/Instances/" + linkStrategy) if f.is_dir()]
    for transponderStrategy in transponderStrategies:
        topologies = [f.name for f in os.scandir("../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy) if f.is_dir()]
        for topology in topologies:
            demands = [f.name for f in os.scandir("../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology + "/Demands") if f.is_dir()]
            for demand in demands:
                test = TestUnit(linkStrategy,transponderStrategy,topology,demand)
                testSet.append(test)

#testUnitVerifier(testSet)
gnpyActivation = ["0"]
CDSet=["0","1"]
osnrSet=["0","1"]

bands=["1"]
TFlowSet=["0"]
reinforcements=["0"]

formulationSet = ["0"]
userCuts = ["0"]
objSet = ["TUS","NLUS"]

preprocessingSet= ["0","2"]

with open('../Inputs/onlineParametersBase.txt', "r") as f:
    lines = f.readlines()
    f.close() 

auxOutputFolder = [f.name for f in os.scandir("../Outputs") if f.is_dir()]
if "parametersSet" not in auxOutputFolder:
    os.mkdir("../Outputs/parametersSet")
else:
    shutil.rmtree("../Outputs/parametersSet")
    os.mkdir("../Outputs/parametersSet")

if "QoTSet" not in auxOutputFolder:
    os.mkdir("../Outputs/QoTSet")
else:
    shutil.rmtree("../Outputs/QoTSet")
    os.mkdir("../Outputs/QoTSet")

auxParameterFolder = [f.name for f in os.scandir("../Outputs/parametersSet/") if f.is_dir()]
auxQoTFolder = [f.name for f in os.scandir("../Outputs/parametersSet/") if f.is_dir()]

batchs = 0
counter = 0
print(len(testSet))

with open("../Outputs/experimentList.csv", "w") as list:
    line = "LinkS;TranspS;Instance;Demands;Formulation;CD;OSNR;GNpy;Bands;Reinforcements;Cuts;Prepro"
    list.write(line)
    print("Test list table created")  
    counb = 0 
    for experiment in testSet:
        topology = experiment.topology
        linkStrategy = experiment.linkStrategy
        transponderStrategy = experiment.transponderStrategy
        demandCode = experiment.demandCode
        
        auxLinkStrategy = "_L" + linkStrategy
        auxTransponderStrategy = "_T" + transponderStrategy[:3]

        stringLinks = "topologyFile=Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Links/" + demandCode + "/Link.csv" "\n" 
        stringDemands = "demandToBeRoutedFolder=Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Demands/" + demandCode + "\n"
        demandCode = demandCode.replace("_demands","")

        lines[1] = stringLinks
        lines[4] = stringDemands

        for obj in objSet:
            stringObj = "obj=" + obj + "\n"
            lines[21] = stringObj

            for form in formulationSet:
                stringForm = "formulation=" + form + "\n"
                lines[19] = stringForm

                for cd in CDSet:
                    stringCD = "ChromaticDispersion_activation=" + cd + "\n"
                    lines[9] = stringCD

                    for osnr in osnrSet:
                        stringOSNR = "OSNR_activation=" + osnr + "\n"
                        lines[10] = stringOSNR

                        for gnpy in gnpyActivation:
                            stringGN = "GNPY_activation=" + gnpy + "\n"
                            lines[7] = stringGN

                            for band in bands:
                                stringBand = "Bands=" + band + "\n"
                                lines[13] = stringBand

                                for cut in userCuts:
                                    stringCut = "userCuts=" + cut + "\n"
                                    lines[20] = stringCut

                                    for reinforcement in reinforcements:
                                        stringReinfo = "Reinforcements=" + reinforcement + "\n"
                                        lines[15] = stringReinfo

                                        for prepro in preprocessingSet:
                                            stringPrepro = "preprocessingLevel=" + prepro + "\n"
                                            lines[31] = stringPrepro

                                            #verifying if tflow
                                            if form == '2':
                                                for tflow in TFlowSet:
                                                    stringTflow = "TFlow_Policy=" + tflow  + "\n"
                                                    lines[14] = stringTflow

                                                    #FOLDER MANAGEMENT
                                                    if (counter % 200 == 0) or (counter == 0):
                                                        batchs = batchs + 1
                                                        currentBatch = "batch_" + str(batchs)
                                                        currentBatchFolder = "../Outputs/parametersSet/" + currentBatch
                                                        if currentBatch not in auxParameterFolder:
                                                                os.mkdir(currentBatchFolder)
                                                        else:
                                                            shutil.rmtree(currentBatchFolder)
                                                            os.mkdir(currentBatchFolder)
                                                        print("Batch folder created")

                                                    qotFolder = "../Outputs/QoTSet" + "/oP" +auxLinkStrategy+ auxTransponderStrategy+ "_i" + topology + "_d" + demandCode + "_of" + obj + "_f" + form + "_tf" + tflow+ "_cd" + cd + "_os" + osnr + "_gn" + gnpy+ "_b" + band + "_r" + reinforcement + "_cu" + cut+ "_p" + prepro
                                                    #os.mkdir(qotFolder)
                                                    #print(qotFolder)
                                                    shutil.copytree("../Inputs/QoT_ReferenceFolder", qotFolder)  
                                                    thisQotFolderDemands = [f.name for f in os.scandir(qotFolder+"/demands") if f.is_file()]
                                                    for element in thisQotFolderDemands:
                                                        os.remove(qotFolder+"/demands/"+element)
                                                    thisQotFolderTopology = [f.name for f in os.scandir(qotFolder+"/topology") if f.is_file()]
                                                    for element in thisQotFolderTopology:
                                                        os.remove(qotFolder+"/topology/"+element)
                                                    #os.remove(qotFolder+"/paths.csv")
                                                    nodeFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Nodes/" + demandCode+"_demands" + "/Nodes.csv"
                                                    linkFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Links/" + demandCode+"_demands" + "/Link.csv"
                                                    demandFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Demands/" + demandCode+"_demands" + "/demands_1.csv"
                                                    
                                                    shutil.copyfile(nodeFile, qotFolder+"/topology/Node.csv")
                                                    shutil.copyfile(linkFile, qotFolder+"/topology/Link.csv")
                                                    shutil.copyfile(demandFile, qotFolder+"/demands/demand_1.csv")

                                                    lines[8] = "QoTFolder="+qotFolder[11:]+ "\n"
                                                    parametersName = currentBatchFolder + "/oP"+auxLinkStrategy+ auxTransponderStrategy+ "_i" + topology + "_d" + demandCode + "_of" + obj + "_f" + form + "_tf" + tflow+ "_cd" + cd + "_os" + osnr + "_gn" + gnpy+ "_b" + band + "_r" + reinforcement + "_cu" + cut+ "_p" + prepro+ ".txt"
                                                    listLine = "\n" + linkStrategy+";"+ transponderStrategy+";"+topology + ";" + demandCode + ";" + obj + ";" + form + tflow +";" + cd + ";" + osnr + ";"+ gnpy + ";"+ band + ";"+ reinforcement + ";"+ cut + ";"+prepro+ ";"
                                                    list.write(listLine)
                                                    with open(parametersName, "w") as f:
                                                        for line in lines:
                                                            f.write(line)
                                                        f.close() 
                                                    counter = counter + 1
                                                    print(parametersName)
                                            else:
                                                stringTflow = "TFlow_Policy=0" + "\n"
                                                lines[14] = stringTflow

                                                #FOLDER MANAGEMENT
                                                if (counter % 200 == 0) or (counter == 0):
                                                    batchs = batchs + 1
                                                    currentBatch = "batch_" + str(batchs)
                                                    currentBatchFolder = "../Outputs/parametersSet/" + currentBatch
                                                    if currentBatch not in auxParameterFolder:
                                                            os.mkdir(currentBatchFolder)
                                                    else:
                                                        shutil.rmtree(currentBatchFolder)
                                                        os.mkdir(currentBatchFolder)
                                                    print("Batch folder created")


                                                qotFolder = "../Outputs/QoTSet" + "/oP" +auxLinkStrategy+ auxTransponderStrategy+ "_i" + topology + "_d" + demandCode + "_of" + obj + "_f" + form + "_cd" + cd + "_os" + osnr + "_gn" + gnpy+ "_b" + band + "_r" + reinforcement + "_cu" + cut+ "_p" + prepro
                                                #os.mkdir(qotFolder)
                                                #print(qotFolder)
                                                shutil.copytree("../Inputs/QoT_ReferenceFolder", qotFolder)  
                                                thisQotFolderDemands = [f.name for f in os.scandir(qotFolder+"/demands") if f.is_file()]
                                                for element in thisQotFolderDemands:
                                                    os.remove(qotFolder+"/demands/"+element)
                                                thisQotFolderTopology = [f.name for f in os.scandir(qotFolder+"/topology") if f.is_file()]
                                                for element in thisQotFolderTopology:
                                                    os.remove(qotFolder+"/topology/"+element)
                                                #os.remove(qotFolder+"/paths.csv")
                                                nodeFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Nodes/" + demandCode+"_demands" + "/Nodes.csv"
                                                linkFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Links/" + demandCode+"_demands" + "/Link.csv"
                                                demandFile = "../Outputs/Instances/" + linkStrategy + "/" + transponderStrategy+ "/" + topology  + "/Demands/" + demandCode+"_demands" + "/demands_1.csv"
                                                
                                                shutil.copyfile(nodeFile, qotFolder+"/topology/Node.csv")
                                                shutil.copyfile(linkFile, qotFolder+"/topology/Link.csv")
                                                shutil.copyfile(demandFile, qotFolder+"/demands/demand_1.csv")

                                                lines[8] = "QoTFolder="+qotFolder[11:]+ "\n"
                                                parametersName = currentBatchFolder + "/oP"+auxLinkStrategy+ auxTransponderStrategy+ "_i" + topology + "_d" + demandCode + "_of" + obj + "_f" + form + "_cd" + cd + "_os" + osnr + "_gn" + gnpy+ "_b" + band + "_r" + reinforcement + "_cu" + cut+ "_p" + prepro+ ".txt"
                                                listLine = "\n" + linkStrategy+";"+ transponderStrategy+";"+topology + ";" + demandCode + ";" + obj + ";" + form  +";" + cd + ";" + osnr + ";"+ gnpy + ";"+ band + ";"+ reinforcement + ";"+ cut + ";" +prepro + ";"
                                                list.write(listLine)
                                                with open(parametersName, "w") as f:
                                                    for line in lines:
                                                        f.write(line)
                                                    f.close() 
                                                counter = counter + 1
                                        
list.close() 

print("Parameter Set created")
print(str(counter) + " Experiments")

batchsList = os.listdir("../Outputs/parametersSet")
toClean = os.listdir("../Outputs")
for element in toClean:
    if element[0] == "j":
        os.remove("../Outputs/"+element)

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
                if counter == 9:
                    line = "#SBATCH --output="+batch+"_%a      # modifie le nom du fichier de sortie par défaut"
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
        stringLine2 = "echo " + "jobs" + batch + ".sh "+ ">> okBatchs.txt\n"
        f.write(stringLine2)
f.close() 
print("Experiments script created")

with open("../Outputs/results.csv", "w") as f:
    line = "LinkS;TranspS;Instance;Demands;UB;LB;GAP;Time;OF;Formulation;CD;OSNR;GNpy;Bands;Reinforcements;Cuts;Prepro;PreproTime;Variables;Constraints;v0;preproC;Paths;FeasibleC;InfeasibleC;OnlyOsnrC;OnlyReachC;FeasibleL;InfeasibleL;OnlyOsnrL;OnlyReachL;ads;dcb;llb;nlus;slus;suld;trl;tus;tase"
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
