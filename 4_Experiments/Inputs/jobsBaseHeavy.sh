#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-33             # création d'un tableau de 24 jobs indicés de 0 à 23
#SBATCH --partition=normal       # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=65536      # 64 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================
