#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-13             # création d'un tableau de 14 jobs indicés de 0 à 13
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L1x_Tran_iGerman_17_26_d15_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d40_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d20_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d20-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d35-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d15-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d25_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d35_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d10_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d40-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d10-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d30-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d25-b_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 /oP_L1x_Tran_iGerman_17_26_d30_ofLLB_f0_cd0_os0_gn0_b2_r0_cu0_p2 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt