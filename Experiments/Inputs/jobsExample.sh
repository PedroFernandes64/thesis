#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-14              # création d'un tableau de 15 jobs indicés de 0 à 14
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=1        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384       # 2 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================
 
tab1=(/10demands /20demands /30demands /10demands /20demands /30demands /10demands /20demands /30demands /10demands /20demands /30demands /10demands /20demands /30demands)
tab2=(1 1 1 2 2 2 3 3 3 4 4 4 5 5 5)
./exe Spain${tab1[$SLURM_ARRAY_TASK_ID]} Spain${tab1[$SLURM_ARRAY_TASK_ID]}_K${tab2[$SLURM_ARRAY_TASK_ID]}.txt ${tab2[$SLURM_ARRAY_TASK_ID]}