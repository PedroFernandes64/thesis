#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-27             # création d'un tableau de 28 jobs indicés de 0 à 27
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L1.5x_Tran_iGerman_17_26_d15_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d20_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d20-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d40-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d30_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d10-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d35-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d40-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d15_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d30_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d10_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d25-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d30-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d40_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d25-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d30-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d15-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d25_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d35_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d35-b_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d25_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d10-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d20-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d40_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d10_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d35_ofNLUS_f0_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d20_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 /oP_L1.5x_Tran_iGerman_17_26_d15-b_ofNLUS_f3_cd0_os0_gn0_b1_r0_cu0_p2 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt