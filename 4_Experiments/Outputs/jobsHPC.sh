#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-31             # création d'un tableau de 32 jobs indicés de 0 à 31
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=1        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=8192      # 8 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_iGerman_17_25_d45_of2p_s0_gn0.txt /oP_iEuropean_18_40_d48_of2p_s3_gn0.txt /oP_iEuropean_18_40_d48_of8_s0_gn0.txt /oP_iGerman_17_25_d45_of8_s3_gn0.txt /oP_i6nodes9links_6_9_d15_of2p_s0_gn0.txt /oP_iNsfnet_14_21_d91_of2p_s3_gn0.txt /oP_iNsfnet_14_21_d91_of4_s3_gn0.txt /oP_i6nodes9links_6_9_d15_of4_s3_gn0.txt /oP_i6nodes9links_6_9_d15_of8_s0_gn0.txt /oP_iGerman_17_25_d45_of4_s3_gn0.txt /oP_i6nodes9links_6_9_d15_of2p_s3_gn0.txt /oP_i6nodes9links_6_9_d15_of1_s0_gn0.txt /oP_iNsfnet_14_21_d91_of8_s3_gn0.txt /oP_iNsfnet_14_21_d91_of8_s0_gn0.txt /oP_iGerman_17_25_d45_of1_s0_gn0.txt /oP_iEuropean_18_40_d48_of2p_s0_gn0.txt /oP_iEuropean_18_40_d48_of8_s3_gn0.txt /oP_iGerman_17_25_d45_of1_s3_gn0.txt /oP_iNsfnet_14_21_d91_of2p_s0_gn0.txt /oP_iGerman_17_25_d45_of8_s0_gn0.txt /oP_iNsfnet_14_21_d91_of4_s0_gn0.txt /oP_i6nodes9links_6_9_d15_of1_s3_gn0.txt /oP_iNsfnet_14_21_d91_of1_s0_gn0.txt /oP_iGerman_17_25_d45_of2p_s3_gn0.txt /oP_iNsfnet_14_21_d91_of1_s3_gn0.txt /oP_iEuropean_18_40_d48_of1_s0_gn0.txt /oP_i6nodes9links_6_9_d15_of4_s0_gn0.txt /oP_iEuropean_18_40_d48_of4_s0_gn0.txt /oP_iEuropean_18_40_d48_of4_s3_gn0.txt /oP_iGerman_17_25_d45_of4_s0_gn0.txt /oP_i6nodes9links_6_9_d15_of8_s3_gn0.txt /oP_iEuropean_18_40_d48_of1_s3_gn0.txt )
tab2=(/oP_iGerman_17_25_d45_of2p_s0_gn0 /oP_iEuropean_18_40_d48_of2p_s3_gn0 /oP_iEuropean_18_40_d48_of8_s0_gn0 /oP_iGerman_17_25_d45_of8_s3_gn0 /oP_i6nodes9links_6_9_d15_of2p_s0_gn0 /oP_iNsfnet_14_21_d91_of2p_s3_gn0 /oP_iNsfnet_14_21_d91_of4_s3_gn0 /oP_i6nodes9links_6_9_d15_of4_s3_gn0 /oP_i6nodes9links_6_9_d15_of8_s0_gn0 /oP_iGerman_17_25_d45_of4_s3_gn0 /oP_i6nodes9links_6_9_d15_of2p_s3_gn0 /oP_i6nodes9links_6_9_d15_of1_s0_gn0 /oP_iNsfnet_14_21_d91_of8_s3_gn0 /oP_iNsfnet_14_21_d91_of8_s0_gn0 /oP_iGerman_17_25_d45_of1_s0_gn0 /oP_iEuropean_18_40_d48_of2p_s0_gn0 /oP_iEuropean_18_40_d48_of8_s3_gn0 /oP_iGerman_17_25_d45_of1_s3_gn0 /oP_iNsfnet_14_21_d91_of2p_s0_gn0 /oP_iGerman_17_25_d45_of8_s0_gn0 /oP_iNsfnet_14_21_d91_of4_s0_gn0 /oP_i6nodes9links_6_9_d15_of1_s3_gn0 /oP_iNsfnet_14_21_d91_of1_s0_gn0 /oP_iGerman_17_25_d45_of2p_s3_gn0 /oP_iNsfnet_14_21_d91_of1_s3_gn0 /oP_iEuropean_18_40_d48_of1_s0_gn0 /oP_i6nodes9links_6_9_d15_of4_s0_gn0 /oP_iEuropean_18_40_d48_of4_s0_gn0 /oP_iEuropean_18_40_d48_of4_s3_gn0 /oP_iGerman_17_25_d45_of4_s0_gn0 /oP_i6nodes9links_6_9_d15_of8_s3_gn0 /oP_iEuropean_18_40_d48_of1_s3_gn0 )
echo parametersSet${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet${tab1[$SLURM_ARRAY_TASK_ID]} >> executionOutputs${tab2[$SLURM_ARRAY_TASK_ID]}.txt