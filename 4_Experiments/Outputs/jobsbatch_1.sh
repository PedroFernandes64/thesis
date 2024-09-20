#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-0             # création d'un tableau de 1 jobs indicés de 0 à 0
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_1_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L2x_Teff_iNsfnet_14_21_d91_ofTUS_f0_cd1_os1_gn0_b1_r0_cu0_p0 )
echo parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_1/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt