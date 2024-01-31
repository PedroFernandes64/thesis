#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-39             # création d'un tableau de 40 jobs indicés de 0 à 39
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=1        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=8192      # 8 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_i6nodes9links_6_9_d15_of2p_f2_s0_gn1_cu0 /oP_iEuropean_18_40_d48_of4_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d91_of4_f2_s0_gn1_cu1 /oP_iNSF_9_13_d50_of2p_f2_s0_gn1_cu1 /oP_iNSF_9_13_d50_of4_f2_s0_gn1_cu0 /oP_iGerman_17_26_d45_of2p_f2_s0_gn1_cu0 /oP_iEuropean_18_40_d48_of2p_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d50_of2p_f2_s0_gn1_cu1 /oP_iSpain_21_35_d70_of2p_f2_s0_gn1_cu0 /oP_iSpain_21_35_d30_of4_f2_s0_gn1_cu0 /oP_iEU_18_40_d48_of2p_f2_s0_gn1_cu1 /oP_iGerman_17_26_d45_of2p_f2_s0_gn1_cu1 /oP_iGerman_17_26_d45_of4_f2_s0_gn1_cu0 /oP_i6nodes9links_6_9_d15_of4_f2_s0_gn1_cu0 /oP_iSpain_21_35_d30_of2p_f2_s0_gn1_cu1 /oP_iEuropean_18_40_d48_of4_f2_s0_gn1_cu1 /oP_i6nodes9links_6_9_d50_of2p_f2_s0_gn1_cu1 /oP_iSpain_21_35_d30_of2p_f2_s0_gn1_cu0 /oP_iEuropean_18_40_d48_of2p_f2_s0_gn1_cu1 /oP_i6nodes9links_6_9_d50_of2p_f2_s0_gn1_cu0 /oP_iSpain_21_35_d30_of4_f2_s0_gn1_cu1 /oP_i6nodes9links_6_9_d50_of4_f2_s0_gn1_cu0 /oP_iSpain_21_35_d70_of2p_f2_s0_gn1_cu1 /oP_iNsfnet_14_21_d91_of4_f2_s0_gn1_cu0 /oP_iSpain_21_35_d70_of4_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d91_of2p_f2_s0_gn1_cu1 /oP_iEU_18_40_d48_of4_f2_s0_gn1_cu1 /oP_i6nodes9links_6_9_d15_of2p_f2_s0_gn1_cu1 /oP_iSpain_21_35_d70_of4_f2_s0_gn1_cu1 /oP_iNSF_9_13_d50_of2p_f2_s0_gn1_cu0 /oP_iNSF_9_13_d50_of4_f2_s0_gn1_cu1 /oP_iGerman_17_26_d45_of4_f2_s0_gn1_cu1 /oP_iNsfnet_14_21_d50_of2p_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d91_of2p_f2_s0_gn1_cu0 /oP_iEU_18_40_d48_of2p_f2_s0_gn1_cu0 /oP_i6nodes9links_6_9_d50_of4_f2_s0_gn1_cu1 /oP_iEU_18_40_d48_of4_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d50_of4_f2_s0_gn1_cu0 /oP_iNsfnet_14_21_d50_of4_f2_s0_gn1_cu1 /oP_i6nodes9links_6_9_d15_of4_f2_s0_gn1_cu1 )
echo parametersSet${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt