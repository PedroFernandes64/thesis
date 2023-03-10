import os
import sys
import time
import tkinter as tk
from tkinter import *
from tkinter import ttk
from tkinter.ttk import *
import tkinter.font as tkFont
from turtle import end_fill
from PIL import Image, ImageTk
from tkinter import filedialog
from tkinter.scrolledtext import ScrolledText
from click import style
from matplotlib import container


class MainWindow(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        #main values
        self.title("IU FlexOptim")
        #self.geometry('1180x700')
        self.configure(background='white', padx=10, pady=10, border=1)
        
        #fonts
        self.fontStyle1 = tkFont.Font(family="Times", size=16, slant=tkFont.ROMAN)
        self.fontStyle2 = tkFont.Font(family="Times", size=14)
        self.fontStyle3 = tkFont.Font(size=11, underline=1)
        self.fontStyle4 = tkFont.Font(family="Times", size=12)

        #image import
        self.imgH = ImageTk.PhotoImage(Image.open("img/help.png"))

        #title
        Label(self, text="User interface for the RSA problem (FlexOptim)", font=self.fontStyle1, background='white', padding=30).grid(column=0, row=0, columnspan=3)

        #logos
        self.img1 = ImageTk.PhotoImage(Image.open("img/isima.png"))
        Label(self, image = self.img1, background='white').grid(row=0, column=4)
        self.img2 = ImageTk.PhotoImage(Image.open("img/limos.png"))
        Label(self, image = self.img2, background='white').grid(row=0, column=5)
        self.img3 = ImageTk.PhotoImage(Image.open("img/anr.png"))
        Label(self, image = self.img3, background='white').grid(row=0, column=6)
        
        #display
        #self.terminalL = Label(self, text='Terminal', background='white').grid(row=1, column=1, columnspan=3)
        #self.text_box = Text(self, height=35, width=80, padx=5, pady=5, background='#959bcf')
        self.text_box = ScrolledText(self, height=42, width=100, padx=5, pady=5, background='#959bcf', font=("consolas", "8", "normal"))
        self.text_box.grid(row=1, column=4, columnspan=3)
        
        #left buttons
        #self.text = StringVar()
        #self.text.set("Next")
        #self.switch_frame_button = tk.Button(self, textvariable=self.text, background='#959bcf', command=self.switch_frame).grid(row=2, column=0, padx=5, pady=5, ipadx=6, ipady=0)
        self.firstframe_button = tk.Button(self, text='1', background='#959bcf', command=lambda: self.show_frame("FirstFrame")).grid(row=2, column=0, padx=5, pady=5, ipadx=16, ipady=0)
        self.secondframe_button = tk.Button(self, text='2', background='#959bcf', command=lambda: self.show_frame("SecondFrame")).grid(row=2, column=1, padx=5, pady=5, ipadx=16, ipady=0)
        self.thirdframe_button = tk.Button(self, text='3', background='#959bcf', command=lambda: self.show_frame("ThirdFrame")).grid(row=2, column=2, padx=5, pady=5, ipadx=16, ipady=0)

        #frames window
        self.container = tk.Frame(self)
        self.container.grid(row=1, column=0, columnspan=3)
        self.frames = {}
        for fr in (FirstFrame, SecondFrame, ThirdFrame):
            page_name = fr.__name__
            frame = fr(parent=self)
            self.frames[page_name] = frame
            frame.grid(row=1, column=0, columnspan=3, sticky='nsew')  # Put all the frames in the same spot.
        self.show_frame("FirstFrame")

        #right buttons
        #tk.Button(self, text="RL", background='#959bcf', command=self.redirect_logging).grid(row=3, column=1, padx=5, pady=5, ipadx=8, ipady=0)
        #tk.Button(self, text="Generate Parameters", background='#959bcf', command=self.gen_param).grid(row=2, column=4, padx=5, pady=5, ipadx=8, ipady=0)
        tk.Button(self, text="Run", background='#959bcf', command=self.run).grid(row=2, column=5, padx=5, pady=5, ipadx=10  , ipady=0)
        tk.Button(self, text="Close", command=self.destroy, background='#959bcf').grid(row=2, column=6, padx=5, pady=5, ipadx=5, ipady=0)

        # Declarations for input values 
        self.paramFileFolder = StringVar()
        self.topologyFile = StringVar()
        self.initialMappingDemandFile = StringVar()
        self.initialMappingAssignmentFile = StringVar()
        self.demandToBeRoutedFolder = StringVar()
        self.GNPY_activation = StringVar()
        self.GNPY_topologyFile = StringVar()
        self.GNPY_equipmentFile = StringVar()
        self.nbDemandsAtOnce = StringVar()
        self.formulation = StringVar()
        self.userCuts = StringVar()
        self.obj = StringVar()
        self.allowBlocking = StringVar()
        self.hopPenalty = StringVar()
        self.partitionPolicy = StringVar()
        self.partitionLoad = StringVar()
        self.partitionSlice = StringVar()
        self.solver = StringVar()
        self.method = StringVar()
        self.preprocessingLevel = StringVar()
        self.linearRelaxation = StringVar()
        self.relaxMethod = StringVar()
        self.lagrangianRelaxation = StringVar()
        self.outputPath = StringVar()
        self.outputLevel = StringVar()
        self.nbSlicesInOutputFile = StringVar()
        self.globalTimeLimit = StringVar()
        self.timeLimit = StringVar()
        
        self.lagrangianMultiplier_zero = StringVar()
        self.lagrangianLambda_zero = StringVar()
        self.nbIterationsWithoutImprovement = StringVar()
        self.maxNbIterations = StringVar()
        
        self.lagFormulation = StringVar()
        self.heuristic = StringVar()

        self.directionMethod = StringVar()
        self.crowderParam = StringVar()
        self.carmeriniParam = StringVar()
        
        self.projection = StringVar()
        self.alternativeStop = StringVar()
        self.warmstart = StringVar()

        self.lagOutputPath = StringVar()
    
    #show a frame for the given page name
    def show_frame(self, page_name):
        
        frame = self.frames[page_name]
        frame.tkraise()
        #self.change_bcolor(page_name)

    #change color of clicked button #### not implemented
    def change_bcolor(self, page_name):
        if page_name == "FirstFrame":
            self.firstframe_button.configure(background='white')
            self.secondframe_button.configure(background='#959bcf')
            self.thirdframe_button.configure(background='#959bcf')
        elif page_name == "SecondFrame":
            self.firstframe_button.configure(background='#959bcf')
            self.secondframe_button.configure(background='white')
            self.thirdframe_button.configure(background='#959bcf')
        elif page_name == "ThirdFrame":
            self.firstframe_button.configure(background='#959bcf')
            self.secondframe_button.configure(background='#959bcf')
            self.thirdframe_button.configure(background='white')

    #create parameters file
    def create_param(self):
        file = open('onlineParameters.txt', 'w')

        file.write('******* Input File Paths *******\n')
        
        if self.topologyFile.get() != '':
            topologyFilePath = StringVar()
            os.system("wsl wslpath -a " + self.topologyFile.get() + " > topologyFile.txt")
            File1 = open('topologyFile.txt', 'r').readline()
            topologyFilePath.set(File1.strip('\n'))
            File1.write('topologyFile='+topologyFilePath.get()+'\n')
        else:
            self.text_box.insert("end", "Empty Field: topologyFile\n")
            file.write('topologyFile=../Parameters/Instances/LagrangianTests/Spain/30demands/Link.csv\n')
        
        if self.initialMappingDemandFile.get() != '':
            file.write('initialMappingDemandFile='+self.initialMappingDemandFile.get()+'\n')
        else:
            self.text_box.insert("end", "Empty Field: initialMappingDemandFile\n")
            file.write('initialMappingDemandFile=\n')
        
        if self.initialMappingAssignmentFile.get() != '':
            file.write('initialMappingAssignmentFile='+self.initialMappingAssignmentFile.get()+'\n')
        else:
            self.text_box.insert("end", "Empty Field: initialMappingAssignmentFile\n")
            file.write('initialMappingAssignmentFile=\n')
        
        if self.demandToBeRoutedFolder.get() != '':
            file.write('demandToBeRoutedFolder='+self.demandToBeRoutedFolder.get()+'\n')
        else:
            self.text_box.insert("end", "Empty Field: demandToBeRoutedFolder\n")
            file.write('demandToBeRoutedFolder=../Parameters/Instances/LagrangianTests/Spain/30demands/Demands1\n\n')

        file.write('******* GNPY parameters *******\n')
        
        if self.GNPY_activation.get() == "Deactivated":
            file.write('GNPY_activation=0\n')
        elif self.GNPY_activation.get() == "Activated":
            file.write('GNPY_activation=1\n')
            
            if self.GNPY_topologyFile.get() != '':
                file.write('GNPY_topologyFile='+self.GNPY_topologyFile.get()+'\n')
            else:
                self.text_box.insert("end", "Empty Field: GNPY_topologyFile\n")
                file.write('GNPY_topologyFile=\n')

            if self.GNPY_equipmentFile.get() != '':
                file.write('GNPY_equipmentFile='+self.GNPY_equipmentFile.get()+'\n\n')
            else:
                self.text_box.insert("end", "Empty Field: GNPY_equipmentFile\n")
                file.write('GNPY_equipmentFile=\n')
        else:
            self.text_box.insert("end", "Empty Field: GNPY_activation : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('GNPY_activation=0\n')
            file.write('GNPY_topologyFile=\n')
            file.write('GNPY_equipmentFile=\n\n')

        file.write('******* Formulation parameters *******\n')
        
        if self.nbDemandsAtOnce.get() != "":
            file.write('nbDemandsAtOnce=' + self.nbDemandsAtOnce.get() + '\n')
        else:
            self.text_box.insert("end", "Empty Field: nbDemandsAtOnce : ")
            self.text_box.insert("end", "Considering 30\n")
            file.write('nbDemandsAtOnce=30\n')

        if self.formulation.get() == "0":
            file.write('formulation=0\n')
        elif self.formulation.get() == "1":
            file.write('formulation=1\n')
        else:
            self.text_box.insert("end", "Empty Field: formulation : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('formulation=0\n')

        if self.userCuts.get() == "Deactivated":
            file.write('userCuts=0\n')
        elif self.userCuts.get() == "Activated":
            file.write('userCuts=1\n')
        else:
            self.text_box.insert("end", "Empty Field: userCuts : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('userCuts=0\n')
            
        if self.obj.get() == "1":
            file.write('obj=1\n')
        elif self.obj.get() == "1p":
            file.write('obj=1p\n')
        elif self.obj.get() == "2":
            file.write('obj=2\n')
        elif self.obj.get() == "2p":
            file.write('obj=2p\n')
        elif self.obj.get() == "4":
            file.write('obj=4\n')
        elif self.obj.get() == "8":
            file.write('obj=8\n')
        else:
            self.text_box.insert("end", "Empty Field: obj : ")
            self.text_box.insert("end", "Considering 2p\n")
            file.write('obj=2p\n')

        if self.allowBlocking.get() == "Deactivated":
            file.write('allowBlocking=0\n')
        elif self.allowBlocking.get() == "Activated":
            file.write('allowBlocking=1\n')
        else:
            self.text_box.insert("end", "Empty Field: allowBlocking : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('allowBlocking=0\n')

        if self.hopPenalty.get() == "Deactivated":
            file.write('hopPenalty=0\n')
        elif self.hopPenalty.get() == "Activated":
            file.write('hopPenalty=1\n')
        else:
            self.text_box.insert("end", "Empty Field: hopPenalty : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('hopPenalty=0\n')

        if self.partitionPolicy.get() == "0":
            file.write('partitionPolicy=0\n')
        elif self.partitionPolicy.get() == "1":
            file.write('partitionPolicy=1\n')
        else:
            self.text_box.insert("end", "Empty Field: partitionPolicy : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('partitionPolicy=0\n')

        if self.partitionLoad.get() != "":
            file.write('partitionLoad=' + self.partitionLoad.get() + '\n')
        else:
            self.text_box.insert("end", "Empty Field: partitionLoad : ")
            self.text_box.insert("end", "Considering 4\n")
            file.write('partitionLoad=4\n')

        if self.partitionSlice.get() != "":
            file.write('partitionSlice=' + self.partitionSlice.get() + '\n\n')
        else:
            self.text_box.insert("end", "Empty Field: partitionSlice : ")
            self.text_box.insert("end", "Considering 15\n")
            file.write('partitionSlice=15\n\n')

        file.write('******* Optimization parameters *******\n')

        if self.solver.get() == "0":
            file.write('solver=0\n')
        elif self.solver.get() == "1":
            file.write('solver=1\n')
        else:
            self.text_box.insert("end", "Empty Field: solver : ")
            self.text_box.insert("end", "Considering 1\n")
            file.write('solver=1\n')
            
        if self.method.get() == "0":
            file.write('method=0\n')
        elif self.method.get() == "1":
            file.write('method=1\n')
        elif self.method.get() == "2":
            file.write('method=2\n')
        else:
            self.text_box.insert("end", "Empty Field: method : ")
            self.text_box.insert("end", "Considering 2\n")
            file.write('method=2\n')

        if self.preprocessingLevel.get() == "0":
            file.write('preprocessingLevel=0\n')
        elif self.preprocessingLevel.get() == "1":
            file.write('preprocessingLevel=1\n')
        elif self.preprocessingLevel.get() == "2":
            file.write('preprocessingLevel=2\n')
        else:
            self.text_box.insert("end", "Empty Field: preprocessingLevel : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('preprocessingLevel=0\n')
            
        if self.linearRelaxation.get() == "Deactivated":
            file.write('linearRelaxation=0\n')
        elif self.linearRelaxation.get() == "Activated":
            file.write('linearRelaxation=1\n')
        else:
            self.text_box.insert("end", "Empty Field: linearRelaxation : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('linearRelaxation=0\n')
            
        if self.relaxMethod.get() == "Deactivated":
            file.write('relaxMethod=0\n')
        elif self.relaxMethod.get() == "Activated":
            file.write('relaxMethod=1\n')
        else:
            self.text_box.insert("end", "Empty Field: relaxMethod : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('relaxMethod=0\n')
        
        if self.lagrangianRelaxation.get() == "Deactivated":
            file.write('lagrangianRelaxation=0\n\n')
        elif self.lagrangianRelaxation.get() == "Activated":
            file.write('lagrangianRelaxation=1\n\n')
        else:
            self.text_box.insert("end", "Empty Field: lagrangianRelaxation : ")
            self.text_box.insert("end", "Considering 0\n")
            file.write('lagrangianRelaxation=0\n\n')
            
        file.write('******* Execution parameters *******\n')
        
        if self.outputPath.get() != '':
            file.write('outputPath='+self.outputPath.get()+'\n')
        else:
            self.text_box.insert("end", "Empty Field: outputPath\n")
            file.write('outputPath=../Parameters/Instances/Spain_N5/InitialMappingMet1Slice30/SimpleTest/Output\n')
        
        if self.outputLevel.get() == "0":
            file.write('outputLevel=0\n')
        elif self.outputLevel.get() == "1":
            file.write('outputLevel=1\n')
        elif self.outputLevel.get() == "2":
            file.write('outputLevel=2\n')
        else:
            self.text_box.insert("end", "Empty Field: outputLevel : ")
            self.text_box.insert("end", "Considering 2\n")
            file.write('outputLevel=2\n')

        if self.nbSlicesInOutputFile.get() == "0":
            file.write('nbSlicesInOutputFile=0\n')
        elif self.nbSlicesInOutputFile.get() == "1":
            file.write('nbSlicesInOutputFile=1\n')
        elif self.nbSlicesInOutputFile.get() == "2":
            file.write('nbSlicesInOutputFile=2\n')
        elif self.nbSlicesInOutputFile.get() == "3":
            file.write('nbSlicesInOutputFile=3\n')
        elif self.nbSlicesInOutputFile.get() == "4":
            file.write('nbSlicesInOutputFile=4\n')
        elif self.nbSlicesInOutputFile.get() == "5":
            file.write('nbSlicesInOutputFile=5\n')
        elif self.nbSlicesInOutputFile.get() == "6":
            file.write('nbSlicesInOutputFile=6\n')
        elif self.nbSlicesInOutputFile.get() == "7":
            file.write('nbSlicesInOutputFile=7\n')
        elif self.nbSlicesInOutputFile.get() == "8":
            file.write('nbSlicesInOutputFile=8\n')
        else:
            self.text_box.insert("end", "Empty Field: nbSlicesInOutputFile : ")
            self.text_box.insert("end", "Considering 320\n")
            file.write('nbSlicesInOutputFile=320\n')

        if self.globalTimeLimit.get() == '':
            self.text_box.insert("end", "Empty Field: globalTimeLimit : ")
            self.text_box.insert("end", "Considering 72000\n")
            file.write('globalTimeLimit=72000\n')
        else:
            file.write('globalTimeLimit='+self.globalTimeLimit.get()+'\n')
        
        if  self.timeLimit.get() == '':
            self.text_box.insert("end", "Empty Field: timeLimit : ")
            self.text_box.insert("end", "Considering 7200\n\n")
            file.write('timeLimit=7200\n\n')
        else:
            file.write('timeLimit='+self.timeLimit.get()+'\n\n')
        
        file.write('******* Fields below are reserved for team LIMOS *******\n')

        if self.lagrangianMultiplier_zero.get() == "0":
            file.write('lagrangianMultiplier_zero=0\n')
        elif self.lagrangianMultiplier_zero.get() == "1":
            file.write('lagrangianMultiplier_zero=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: lagrangianMultiplier_zero : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('lagrangianMultiplier_zero=0\n')

        if self.lagrangianLambda_zero.get() == "0":
            file.write('lagrangianLambda_zero=0\n')
        elif self.lagrangianLambda_zero.get() == "1":
            file.write('lagrangianLambda_zero=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: lagrangianLambda_zero : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('lagrangianLambda_zero=0\n')

        if self.nbIterationsWithoutImprovement.get() == "0":
            file.write('nbIterationsWithoutImprovement=0\n')
        elif self.nbIterationsWithoutImprovement.get() == "1":
            file.write('nbIterationsWithoutImprovement=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: nbIterationsWithoutImprovement : ")
            #self.text_box.insert("end", "Considering 30\n")
            file.write('nbIterationsWithoutImprovement=30\n')

        if self.maxNbIterations.get() == "0":
            file.write('maxNbIterations=0\n')
        elif self.maxNbIterations.get() == "1":
            file.write('maxNbIterations=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: maxNbIterations : ")
            #self.text_box.insert("end", "Considering 5000\n")
            file.write('maxNbIterations=5000\n')

        if self.lagFormulation.get() == "0":
            file.write('lagFormulation=0\n')
        elif self.lagFormulation.get() == "1":
            file.write('lagFormulation=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: lagFormulation : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('lagFormulation=0\n')

        if self.heuristic.get() == "0":
            file.write('heuristic=0\n')
        elif self.heuristic.get() == "1":
            file.write('heuristic=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: heuristic : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('heuristic=0\n')

        if self.directionMethod.get() == "0":
            file.write('directionMethod=0\n')
        elif self.directionMethod.get() == "1":
            file.write('directionMethod=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: directionMethod : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('directionMethod=0\n')

        if self.crowderParam.get() == "0":
            file.write('crowderParam=0\n')
        elif self.crowderParam.get() == "1":
            file.write('crowderParam=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: crowderParam : ")
            #self.text_box.insert("end", "Considering 0.0\n")
            file.write('crowderParam=0.0\n')

        if self.carmeriniParam.get() == "0":
            file.write('carmeriniParam=0\n')
        elif self.carmeriniParam.get() == "1":
            file.write('carmeriniParam=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: carmeriniParam : ")
            #self.text_box.insert("end", "Considering 0.0\n")
            file.write('carmeriniParam=0.0\n')

        if self.projection.get() == "0":
            file.write('projection=0\n')
        elif self.projection.get() == "1":
            file.write('projection=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: projection : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('projection=0\n')

        if self.alternativeStop.get() == "0":
            file.write('alternativeStop=0\n')
        elif self.alternativeStop.get() == "1":
            file.write('alternativeStop=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: alternativeStop : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('alternativeStop=0\n')

        if self.warmstart.get() == "0":
            file.write('warmstart=0\n')
        elif self.warmstart.get() == "1":
            file.write('warmstart=1\n')
        else:
            #self.text_box.insert("end", "Empty Field: warmstart : ")
            #self.text_box.insert("end", "Considering 0\n")
            file.write('warmstart=0\n')

        if self.lagOutputPath.get() == "0":
            file.write('lagOutputPath=0\n\n')
        elif self.lagOutputPath.get() == "1":
            file.write('lagOutputPath=1\n\n')
        else:
            #self.text_box.insert("end", "Empty Field: lagOutputPath : ")
            #self.text_box.insert("end", "Considering 0\n\n")
            file.write('lagOutputPath=outputs/')

        file.close()

    #read parameters
    def read_param(self, fileName):
        txt = ''
        try:
            file = open(fileName, 'r')
            txt = txt + file.read()
            file.close()
        except OSError:
            self.text_box.delete(1.0, 'end')
            self.text_box.insert("end", "Incorrect format or non-existent file onlineParameters.txt")
        return txt

    #read output.txt
    def read_exec(self):
        txt = ''
        try:
            file = open('output.txt', 'r')
            txt = txt + file.read()
            file.close()
        except OSError:
            self.text_box.delete(1.0, 'end')
            self.text_box.insert("end", "Incorrect format or non-existent file onlineParameters.txt")
        return txt

    #import parameters
    def imp_param(self):
        self.text_box.delete(1.0, 'end')
        self.text_box.insert("end", self.read_param(self.paramFileFolder.get()))
        os.system("wsl dos2unix " + self.paramFileFolder.get())

    #generate parameters #convert .txt linux in .txt windows
    def gen_param(self):
        self.text_box.delete(1.0, 'end')
        self.text_box.insert("end", "Making file onlineParameters.txt ...\n\n")
        self.create_param()
        self.text_box.insert("end", self.read_param("onlineParameters.txt"))
        os.system("wsl dos2unix onlineParameters.txt")
        self.paramFileFolder.set('')

    #run #generate output
    def run(self):
        if self.paramFileFolder.get() != '':
            paramFilePath = StringVar()
            os.system("wsl wslpath -a " + self.paramFileFolder.get() + " > paramFolderPath.txt")
            file = open('paramFolderPath.txt', 'r').readline()
            paramFilePath.set(file.strip('\n'))
            print("wsl ./exec " + paramFilePath.get() + " > output.txt")
            os.system("wsl ./exec " + paramFilePath.get() + " > output.txt")
            self.text_box.delete(1.0, 'end')
            self.text_box.insert("end", self.read_exec())
        else:
            os.system("wsl ./exec onlineParameters.txt > output.txt")
            self.text_box.delete(1.0, 'end')
            self.text_box.insert("end", self.read_exec())


class FirstFrame(tk.Frame):
    def __init__(self, parent):
        #main values
        tk.Frame.__init__(self, parent, background='white', relief = RIDGE, borderwidth = '3') # relief = RIDGE, borderwidth = '8'
        
        #declarations
        self.demandFilePath = StringVar()
        self.linkFilePath = StringVar()
        self.status = StringVar()

        #title #help1
        Label(self, text='Main Files', font=parent.fontStyle2, background='white').pack(padx=0, pady=20)
        tk.Button(self, background='white', command=self.open_help1, image = parent.imgH, relief=FLAT).pack(ipadx=0, ipady=0)

        #link file choice
        Label(self, text='Choose your Link file', background='white', font=parent.fontStyle4).pack(padx=0, pady=25)
        tk.Button(self, text='Choose file', background='#959bcf', command=lambda:self.open_link_file(parent)).pack(padx=0, pady=0)
        tk.Entry(self, textvariable=self.linkFilePath, background='#959bcf', borderwidth=3).pack(pady=10, ipadx=120)
        
        #demand folder choice
        Label(self, text='Choose your Demand folder', background='white', font=parent.fontStyle4).pack(padx=0, pady=25)
        tk.Button(self, text='Choose file', background='#959bcf', command=lambda:self.open_demand_file(parent)).pack(padx=0, pady=0)
        tk.Entry(self, textvariable=self.demandFilePath, background='#959bcf', borderwidth=3).pack(pady=10, ipadx=120)
        
        #label #confirmation text
        Label(self, text='Accepted formats: .csv , .txt', font=parent.fontStyle4, background='white').pack(padx=0, pady=10)
        Label(self, textvariable=self.status, foreground='green', background='white').pack()

    #open help1
    def open_help1(self):
        top= Toplevel(self)
        top.config(background='white', relief = RIDGE, borderwidth = '8', padx=50, pady=50)
        top.title("Help Main Files")
        Label(top, text="This is a user interface for the FlexOptim project - RSA Problem\n\nFurther information can be found at the following link: https://github.com/rafael-colares/FlexOptim", wraplength=300, background='white').pack()

    #open demand folder* (fix name)
    def open_demand_file(self, parent):
        self.demandFile = tk.filedialog.askdirectory()
        if self.demandFile != '':
            self.demandFilePath.set(self.demandFile)
            parent.demandToBeRoutedFolder.set(self.demandFile)
            self.status.set('Demand file imported successfully')
    
    #open link file
    def open_link_file(self, parent):
        self.linkFile = tk.filedialog.askopenfilename()
        if self.linkFile != '':
            self.linkFilePath.set(self.linkFile)
            parent.topologyFile.set(self.linkFile)
            self.status.set('Link file imported successfully')


class SecondFrame(tk.Frame):
    def __init__(self, parent):
        #main values
        tk.Frame.__init__(self, parent, background='white', relief = RIDGE, borderwidth = '3') # relief = RIDGE, borderwidth = '8'
        
        #declarations
        self.paramFilePath = StringVar()
        self.status = StringVar()
        self.status0 = StringVar()

        #label #help2
        Label(self, text='Parameters', font=parent.fontStyle2, background='white').pack(padx=0, pady=20)
        tk.Button(self, background='white', command=self.open_help2, image = parent.imgH, relief=FLAT).pack(ipadx=0, ipady=0)

        #paramters file
        Label(self, text='Import a Parameters file', font=parent.fontStyle4, background='white').pack(padx=0, pady=25)
        tk.Button(self, text='Choose file', background='#959bcf', command=lambda:self.open_param_file(parent)).pack(padx=0, pady=0)
        tk.Entry(self, textvariable=self.paramFilePath, background='#959bcf', borderwidth=3).pack(pady=10, ipadx=120)
        
        #label
        Label(self, text='Accepted format: .txt', font=parent.fontStyle4, background='white').pack(padx=0, pady=10)
        
        #import parameters
        tk.Button(self, text="Import Parameters", background='#959bcf', borderwidth=3, command=lambda:self.update_param(parent)).pack(padx=5, pady=50, ipadx=10, ipady=10)
        
        #verification text
        Label(self, textvariable=self.status, foreground='green', background='white').pack()
        Label(self, textvariable=self.status0, foreground='red', background='white').pack()
        
    #open help2    
    def open_help2(self):
        top= Toplevel(self)
        top.config(background='white', relief = RIDGE, borderwidth = '8', padx=50, pady=50)
        top.title("Help Parameters")
        Label(top, text="This is a user interface for the FlexOptim project - RSA Problem\n\nFurther information can be found at the following link: https://github.com/rafael-colares/FlexOptim", wraplength=300, background='white').pack()

    #...
    def open_param_file(self, parent):
        self.paramFile = tk.filedialog.askopenfilename()
        if self.paramFile != '':
            self.paramFilePath.set(self.paramFile)
            parent.paramFileFolder.set(self.paramFile)
    
    #...
    def update_param(self, parent):
        if self.paramFilePath.get() != '':
            parent.imp_param()
            self.status.set('Parameters file imported successfully')
            self.status0.set('')
        else:
            self.status.set('')
            self.status0.set('Choose a parameters file')


class ThirdFrame(tk.Frame):
    def __init__(self, parent):    
        #main values
        tk.Frame.__init__(self, parent, background='white', relief = RIDGE, borderwidth = '3')
        
        #declarations
        self.status = StringVar()
        
        #label #help3
        Label(self, text='Modify your parameters', font=parent.fontStyle2, background='white').pack(padx=0, pady=20)
        tk.Button(self, background='white', command=self.open_help3, image = parent.imgH, relief=FLAT).pack(ipadx=0, ipady=0)
        
        #label #main parameters
        Label(self, text='Main Parameters', font=parent.fontStyle4, background='white').pack(padx=0, pady=25)
        tk.Button(self, text="Change Formulation", background='#959bcf', borderwidth=3, command=lambda:self.open_f(parent)).pack(padx=0, pady=0, ipadx=10, ipady=5)
        tk.Button(self, text="Change Objective", background='#959bcf', borderwidth=3, command=lambda:self.open_o(parent)).pack(padx=0, pady=0, ipadx=17, ipady=5)

        #label #paramters
        Label(self, text='Others Parameters', font=parent.fontStyle4, background='white').pack(padx=0, pady=25)
        tk.Button(self, text="Change Formulation Parameters", background='#959bcf', borderwidth=3, command=lambda:self.open_fparam(parent)).pack(padx=0, pady=0, ipadx=6, ipady=0)
        tk.Button(self, text="Change Optimisation Parameters", background='#959bcf', borderwidth=3, command=lambda:self.open_oparam(parent)).pack(padx=0, pady=0, ipadx=4, ipady=0)
        tk.Button(self, text="Change Execution Parameters", background='#959bcf', borderwidth=3, command=lambda:self.open_eparam(parent)).pack(padx=0, pady=0, ipadx=12.4, ipady=0)

        #update button #confirmation text
        tk.Button(self, text="Update Parameters", background='#959bcf', borderwidth=3, command=lambda:self.update_param(parent)).pack(padx=0, pady=23, ipadx=10, ipady=10)
        Label(self, textvariable=self.status, foreground='green', background='white').pack()

    #...
    def open_help3(self):
        top= Toplevel(self)
        top.config(background='white', relief = RIDGE, borderwidth = '8', padx=50, pady=50)
        top.title("Help Modify Parameters")
        Label(top, text="This is a user interface for the FlexOptim project - RSA Problem\n\nFurther information can be found at the following link: https://github.com/rafael-colares/FlexOptim", wraplength=300, background='white').pack()

    #....
    def update_param(self, parent):
        parent.gen_param()
        self.status.set('Parameters updated successfully')
    
    #open formulation window
    def open_f(self, parent):
        self.top = Toplevel(self)
        #self.top.geometry("800x600")
        self.top.title("Change Formulation")
        self.top.configure(background='white', padx=20, pady=20, border=1, borderwidth=1, relief=RIDGE)
        
        #Label(self.top, text="Formulation Parameters", font=parent.fontStyle2, background='white', padding=5).grid(row=0, column=0, pady=10)

        omS = ttk.Style()
        omS.configure("TMenubutton", background='#959bcf')

        Label(self.top, text="Formulation :", background='white', font=parent.fontStyle3).grid(row=3, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.formulation, "Choose", "0", "1").grid(row=4, column=1, padx=6, pady=2)
        Label(self.top, text= "Choice of the formulation to be used. 0 for Flow Formulation. 1 for Edge-Node.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=4, column=0, padx=6, pady=2)

        tk.Button(self.top, text="Done", background='#959bcf', borderwidth=3, command=self.top.destroy).grid(row=19, column=0, pady=10) #lambda:self.setFparam(parent)

    #open objective window
    def open_o(self, parent):
        self.top = Toplevel(self)
        #self.top.geometry("800x600")
        self.top.title("Change Objective")
        self.top.configure(background='white', padx=20, pady=20, border=1, borderwidth=1, relief=RIDGE)
        
        #Label(self.top, text="Formulation Parameters", font=parent.fontStyle2, background='white', padding=5).grid(row=0, column=0, pady=10)

        omS = ttk.Style()
        omS.configure("TMenubutton", background='#959bcf')

        Label(self.top, text="Objective :", background='white', font=parent.fontStyle3).grid(row=7, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.obj, "Choose", "1", "1p", "2", "2p", "4", "8").grid(row=8, column=1, padx=6, pady=2)
        Label(self.top, text= "Which objective to be optimized. 1 for minimize last slot used per demand. 1p for minimize last slot used per edge. 2 for minimize number of hops. 2p for minimize used slices. 4 minimize path lenght. 8 minimize last slot used overall.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=8, column=0, padx=6, pady=2)

        tk.Button(self.top, text="Done", background='#959bcf', borderwidth=3, command=self.top.destroy).grid(row=19, column=0, pady=10) #lambda:self.setFparam(parent)

    #open formulation paramters window
    def open_fparam(self, parent):
        self.top = Toplevel(self)
        #self.top.geometry("800x600")
        self.top.title("Formulation Parameters")
        self.top.configure(background='white', padx=20, pady=20, border=1, borderwidth=1, relief=RIDGE)
        
        #Formulation Parameters
        Label(self.top, text="Formulation Parameters", font=parent.fontStyle2, background='white', padding=5).grid(row=0, column=0, pady=10)

        omS = ttk.Style()
        omS.configure("TMenubutton", background='#959bcf') # wraplength=55 , relief=tk.RAISED
        
        Label(self.top, text="n. Demands at Once :", background='white', font=parent.fontStyle3).grid(row=1, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.nbDemandsAtOnce, "Choose", "0", "1", "2", "3", "4", "5").grid(row=2, column=1, padx=6, pady=2)
        tk.Entry(self.top, textvariable=parent.nbDemandsAtOnce, background='#959bcf').grid(row=2, column=1, padx=6, pady=2)
        Label(self.top, text= "States how many demands are treated in one optimization step. Max = 30", font=parent.fontStyle4, wraplength=580, background='white').grid(row=2, column=0, padx=6, pady=2)

        #Label(self.top, text="Formulation :", background='white', font=parent.fontStyle3).grid(row=3, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.formulation, "Choose", "0", "1").grid(row=4, column=1, padx=6, pady=2)
        #Label(self.top, text= "Choice of the formulation to be used. 0 for Flow Formulation. 1 for Edge-Node.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=4, column=0, padx=6, pady=2)

        Label(self.top, text="User Cuts :", background='white', font=parent.fontStyle3).grid(row=5, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.userCuts, "Choose", "Deactivated", "Activated").grid(row=6, column=1, padx=6, pady=2)
        Label(self.top, text= "Boolean parameter indicating whether user cuts should be applied.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=6, column=0, padx=6, pady=2)

        #Label(self.top, text="Objective :", background='white', font=parent.fontStyle3).grid(row=7, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.obj, "Choose", "1", "1p", "2", "2p", "4", "8").grid(row=8, column=1, padx=6, pady=2)
        #Label(self.top, text= "Which objective to be optimized. 1 for minimize last slot used per demand. 1p for minimize last slot used per edge. 2 for minimize number of hops. 2p for minimize used slices. 4 minimize path lenght. 8 minimize last slot used overall.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=8, column=0, padx=6, pady=2)

        Label(self.top, text="Allow Blocking :", background='white', font=parent.fontStyle3).grid(row=9, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.allowBlocking, "Choose", "Deactivated", "Activated").grid(row=10, column=1, padx=6, pady=2)
        Label(self.top, text= "Boolean parameter indicating whether the optimization can continue after a demand is blocked.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=10, column=0, padx=6, pady=2)

        Label(self.top, text="Hop Penalty :", background='white', font=parent.fontStyle3).grid(row=11, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.hopPenalty, "Choose", "Deactivated", "Activated").grid(row=12, column=1, padx=6, pady=2)
        Label(self.top, text= "Demand's reach penalty applied on each hop.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=12, column=0, padx=6, pady=2)

        Label(self.top, text="Partition Policy :", background='white', font=parent.fontStyle3).grid(row=13, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.partitionPolicy, "Choose", "0", "1").grid(row=14, column=1, padx=6, pady=2)
        Label(self.top, text= "States whether the spectrum is partitioned. 0 for no partition at all. 1 for pushing bigger demands to the right and smaller to the left.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=14, column=0, padx=6, pady=2)

        Label(self.top, text="Partition Load :", background='white', font=parent.fontStyle3).grid(row=15, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.partitionLoad, "Choose", "Deactivated", "Activated").grid(row=16, column=1, padx=6, pady=2)
        tk.Entry(self.top, textvariable=parent.partitionLoad, background='#959bcf').grid(row=16, column=1, padx=6, pady=2)
        Label(self.top, text= "States the highest load that should be on the left partition.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=16, column=0, padx=6, pady=2)
        
        Label(self.top, text="Partition Slice :", background='white', font=parent.fontStyle3).grid(row=17, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.partitionSlice, "Choose", "Deactivated", "Activated").grid(row=18, column=1, padx=6, pady=2)
        tk.Entry(self.top, textvariable=parent.partitionSlice, background='#959bcf').grid(row=18, column=1, padx=6, pady=2)
        Label(self.top, text= "If partition policy = 2, states where the spectrum is divided.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=18, column=0, padx=6, pady=2)

        tk.Button(self.top, text="Done", background='#959bcf', borderwidth=3, command=self.top.destroy).grid(row=19, column=0, pady=10) #lambda:self.setFparam(parent)

    #open optimization paramters window
    def open_oparam(self, parent):
        self.top = Toplevel(self)
        #self.top.geometry("800x600")
        self.top.title("Optimization Parameters")
        self.top.configure(background='white', padx=20, pady=20, border=1, borderwidth=1, relief=RIDGE)
        
        #Optimization Parameters
        Label(self.top, text="Optimization Parameters", font=parent.fontStyle2, background='white', padding=5).grid(row=0, column=0, pady=10)

        omS = ttk.Style()
        omS.configure("TMenubutton", background='#959bcf') # wraplength=55 , relief=tk.RAISED
        
        Label(self.top, text="Solver :", background='white', font=parent.fontStyle3).grid(row=1, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.solver, "Choose", "0", "1").grid(row=2, column=1, padx=6, pady=2)
        Label(self.top, text= "Which solver to be used. 0 for CPLEX. 1 for CBC.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=2, column=0, padx=6, pady=2)

        Label(self.top, text="Method :", background='white', font=parent.fontStyle3).grid(row=3, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.method, "Choose", "0", "1").grid(row=4, column=1, padx=6, pady=2)
        Label(self.top, text= "Possible methods to be applied at each node of the enumeration tree. 0 for linear relaxation.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=4, column=0, padx=6, pady=2)

        Label(self.top, text="Preprocessing Level :", background='white', font=parent.fontStyle3).grid(row=5, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.preprocessingLevel, "Choose", "0", "1", "2").grid(row=6, column=1, padx=6, pady=2)
        Label(self.top, text= "Possible levels of preprocessing to be applied for eliminating variables before optimization is called. 0 to only remove arcs that do not fit the demand load. 1 to look for arcs that would induce length violation and arcs whose neighboors cannot forward the demand. 2 to apply level 1 recursevely until no arc can be removed.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=6, column=0, padx=6, pady=2)

        Label(self.top, text="Linear Relaxation :", background='white', font=parent.fontStyle3).grid(row=7, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.linearRelaxation, "Choose", "Deactivated", "Activated").grid(row=8, column=1, padx=6, pady=2)
        Label(self.top, text= "Boolean parameter used to turn integer variables into linear ones.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=8, column=0, padx=6, pady=2)

        tk.Button(self.top, text="Done", background='#959bcf', borderwidth=3, command=self.top.destroy).grid(row=9, column=0, pady=10) #lambda:self.setFparam(parent)

    #open execution paramters window
    def open_eparam(self, parent):
        self.top = Toplevel(self)
        #self.top.geometry("800x600")
        self.top.title("Execution Parameters")
        self.top.configure(background='white', padx=20, pady=20, border=1, borderwidth=1, relief=RIDGE)
        
        #Execution Parameters
        Label(self.top, text="Execution Parameters", font=parent.fontStyle2, background='white', padding=5).grid(row=0, column=0, pady=10)

        omS = ttk.Style()
        omS.configure("TMenubutton", background='#959bcf') # wraplength=55 , relief=tk.RAISED
        
        Label(self.top, text="Output File Folder :", background='white', font=parent.fontStyle3).grid(row=1, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.outputPath, "Choose", "0", "1").grid(row=2, column=1, padx=6, pady=2)
        tk.Button(self.top, text='Choose file', background='#959bcf', command=lambda:self.open_output_file(parent)).grid(row=2, column=1, padx=6, pady=2)     
        Label(self.top, textvariable=self.status, foreground='green', background='white').grid(row=3, column=1, padx=6, pady=2)
        Label(self.top, text= "Refers to the folder address where the output files will be sent by the end of the optimization procedure.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=2, column=0, padx=6, pady=2)

        Label(self.top, text="Output Level :", background='white', font=parent.fontStyle3).grid(row=3, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.outputLevel, "Choose", "0", "1", "2").grid(row=4, column=1, padx=6, pady=2)
        Label(self.top, text= "Possible output policies to be used. 0 for not creating any output file. 1 for generating output files corresponding to the last mapping. 2 for generating output files after every optimization iteration.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=4, column=0, padx=6, pady=2)

        Label(self.top, text="n Slices In Output File :", background='white', font=parent.fontStyle3).grid(row=5, column=0, padx=6, pady=2)
        OptionMenu(self.top, parent.nbSlicesInOutputFile, "Choose", "0", "1", "2", "3", "4", "5", "6", "7", "8").grid(row=6, column=1, padx=6, pady=2)
        Label(self.top, text= "States how many slices will be displayed in the output file", font=parent.fontStyle4, wraplength=580, background='white').grid(row=6, column=0, padx=6, pady=2)

        Label(self.top, text="Global Time Limit (in secs) :", background='white', font=parent.fontStyle3).grid(row=7, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.globalTimeLimit, "Choose", "Deactivated", "Activated").grid(row=8, column=1, padx=6, pady=2)
        tk.Entry(self.top, textvariable=parent.globalTimeLimit, background='#959bcf').grid(row=8, column=1, padx=6, pady=2)
        Label(self.top, text= "Refers to how much time (in seconds) can be spent during the whole optmization.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=8, column=0, padx=6, pady=2)

        Label(self.top, text="Time Limit (in secs) :", background='white', font=parent.fontStyle3).grid(row=9, column=0, padx=6, pady=2)
        #OptionMenu(self.top, parent.timeLimit, "Choose", "Deactivated", "Activated").grid(row=10, column=1, padx=6, pady=2)
        tk.Entry(self.top, textvariable=parent.timeLimit, background='#959bcf').grid(row=10, column=1, padx=6, pady=2)
        Label(self.top, text= "Refers to how much time (in seconds) can be spent during the one iteration of the optimization.", font=parent.fontStyle4, wraplength=580, background='white').grid(row=10, column=0, padx=6, pady=2)

        tk.Button(self.top, text="Done", background='#959bcf', borderwidth=3, command=self.top.destroy).grid(row=11, column=0, pady=10) #lambda:self.setFparam(parent)

    #...
    def open_output_file(self, parent):
        self.output = tk.filedialog.askopenfilename()
        if self.output != '':
            parent.outputPath.set(self.output)
            self.status.set('Output file folder imported successfully')

#root
root = MainWindow()
root.mainloop()