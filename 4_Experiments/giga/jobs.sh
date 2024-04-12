#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-33             # création d'un tableau de 24 jobs indicés de 0 à 23
#SBATCH --partition=normal       # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=65536      # 64 Go de RAM par CPU
#SBATCH --output=nouveau-nom_%a  # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_iSpain_21_35_d30_of8_f2_mr1_os1_cu0_tf3 /oP_iSpain_21_35_d30_of8_f0_mr1_os1_cu0 /oP_iGerman_17_26_d50_of2p_f2_mr1_os1_cu0_tf3 /oP_iGerman_17_26_d50_of2p_f0_mr1_os1_cu0 /oP_iGerman_17_26_d30_of8_f2_mr1_os1_cu0_tf3 /oP_iGerman_17_26_d30_of8_f0_mr1_os1_cu0 /oP_iEuropean_18_40_d50_of8_f2_mr1_os1_cu0_tf3 /oP_iEuropean_18_40_d50_of8_f0_mr1_os1_cu0 /oP_iEuropean_18_40_d48_of8_f2_mr1_os1_cu0_tf3 /oP_iEuropean_18_40_d48_of8_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of1010_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d500_of8_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d690_of1010_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d240_of1010_f0_mr1_os1_cu0 /oP_iSpain_30_56_d690_of8_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d240_of1010_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d240_of2p_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d690_of2_f0_mr1_os1_cu0 /oP_iSpain_30_56_d690_of2p_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d240_of8_f0_mr1_os1_cu0 /oP_iSpain_30_56_d240_of2p_f0_mr1_os1_cu0 /oP_iSpain_30_56_d240_of2_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d690_of1010_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of1010_f0_mr1_os1_cu0 /oP_iSpain_30_56_d690_of8_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of2_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d690_of2_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d240_of8_f2_mr1_os1_cu0_tf3 /oP_iSpain_30_56_d690_of2p_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of8_f0_mr1_os1_cu0 /oP_iSpain_30_56_d240_of2_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of2p_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of2_f0_mr1_os1_cu0 /oP_iSpain_30_56_d500_of2p_f2_mr1_os1_cu0_tf3 )
echo parametersSet/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt