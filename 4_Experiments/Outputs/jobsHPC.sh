#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-13             # création d'un tableau de 14 jobs indicés de 0 à 13
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=1        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=8192      # 8 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_iNSF_d91_of4_s3_gn0.txt /oP_iNSF_d91_of1_s3_gn0.txt /oP_iNSF_d91_of2_s3_gn0.txt /oP_iNSF_d91_of4p_s0_gn0.txt /oP_iNSF_d91_of2_s0_gn0.txt /oP_iNSF_d91_of4p_s3_gn0.txt /oP_iNSF_d91_of8_s0_gn0.txt /oP_iNSF_d91_of2p_s0_gn0.txt /oP_iNSF_d91_of1_s0_gn0.txt /oP_iNSF_d91_of8_s3_gn0.txt /oP_iNSF_d91_of1p_s3_gn0.txt /oP_iNSF_d91_of2p_s3_gn0.txt /oP_iNSF_d91_of4_s0_gn0.txt /oP_iNSF_d91_of1p_s0_gn0.txt )
tab2=(/oP_iNSF_d91_of4_s3_gn0 /oP_iNSF_d91_of1_s3_gn0 /oP_iNSF_d91_of2_s3_gn0 /oP_iNSF_d91_of4p_s0_gn0 /oP_iNSF_d91_of2_s0_gn0 /oP_iNSF_d91_of4p_s3_gn0 /oP_iNSF_d91_of8_s0_gn0 /oP_iNSF_d91_of2p_s0_gn0 /oP_iNSF_d91_of1_s0_gn0 /oP_iNSF_d91_of8_s3_gn0 /oP_iNSF_d91_of1p_s3_gn0 /oP_iNSF_d91_of2p_s3_gn0 /oP_iNSF_d91_of4_s0_gn0 /oP_iNSF_d91_of1p_s0_gn0 )
echo parametersSet${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet${tab1[$SLURM_ARRAY_TASK_ID]} >> executionOutputs${tab2[$SLURM_ARRAY_TASK_ID]}.txt