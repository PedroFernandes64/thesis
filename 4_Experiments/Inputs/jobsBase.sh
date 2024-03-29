#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-14             # création d'un tableau de 15 jobs indicés de 0 à 14
#SBATCH --partition=normal       # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=2        # une task nécessite 2 CPU
#SBATCH --mem-per-cpu=8192       # 8 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================
