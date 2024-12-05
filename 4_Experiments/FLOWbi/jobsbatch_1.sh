#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-6             # création d'un tableau de 7 jobs indicés de 0 à 6
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=32768      # 32 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L1x_Tran_iGerman_17_26_d60_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d40_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d20_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d50_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d30_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d80_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d70_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt