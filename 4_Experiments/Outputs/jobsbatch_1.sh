#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-8             # création d'un tableau de 9 jobs indicés de 0 à 8
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L1.5x_Teff_iEuropean_18_40_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iGerman_17_26_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iGerman_17_26_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iGerman_17_26_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iNsfnet_14_21_d30_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iNsfnet_14_21_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iEuropean_18_40_d40_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iEuropean_18_40_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 /oP_L1.5x_Teff_iNsfnet_14_21_d50_ofNLUS_f0_cd1_os1_gn0_b1_r0_cu0_p2 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt