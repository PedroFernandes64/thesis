#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-19             # création d'un tableau de 20 jobs indicés de 0 à 19
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L2x_Tran_iEuropean_18_40_d10_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iEuropean_18_40_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iGerman_17_26_d20_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iNsfnet_14_21_d10_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iEuropean_18_40_d20_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iNsfnet_14_21_d20_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iEuropean_18_40_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_i6nodes9links_6_9_d20_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_i6nodes9links_6_9_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iNsfnet_14_21_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iEuropean_18_40_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iGerman_17_26_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_i6nodes9links_6_9_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_i6nodes9links_6_9_d10_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iGerman_17_26_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_i6nodes9links_6_9_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iNsfnet_14_21_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iGerman_17_26_d10_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iGerman_17_26_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 /oP_L2x_Tran_iNsfnet_14_21_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt