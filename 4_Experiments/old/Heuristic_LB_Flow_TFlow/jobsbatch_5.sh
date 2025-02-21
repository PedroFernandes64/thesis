#!/bin/bash
 
#================= OPTIONS (s'applique à chaque job du tableau) =========================
#SBATCH --array=0-9             # création d'un tableau de 10 jobs indicés de 0 à 9
#SBATCH --partition=court        # choix de la partition
#SBATCH --ntasks=1               # chaque job possède une seule task
#SBATCH --cpus-per-task=4        # une task nécessite 4 CPU
#SBATCH --mem-per-cpu=16384      # 16 Go de RAM par CPU
#SBATCH --output=batch_5_%a      # modifie le nom du fichier de sortie par défaut
 
#========================== TASKS ================================

tab1=(/oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf2_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK10_eK20_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf3_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf1_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf3_cd1_os1_gn0_b1_r0_cu0_p2_g0_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf2_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf0_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK10_eK20_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf0_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf1_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK10_eK20_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf2_cd1_os1_gn0_b1_r0_cu0_p2_g0_it50_p500_c100_m200_cK5_eK10_lb1 /oP_L1.5x_Tran_iEuropean_18_40_d40_ofNLUS_f2_tf3_cd1_os1_gn0_b1_r0_cu0_p2_g1_it50_p500_c100_m200_cK10_eK20_lb1 )
echo parametersSet/batch_5/${tab1[$SLURM_ARRAY_TASK_ID]}
./exec parametersSet/batch_5/${tab1[$SLURM_ARRAY_TASK_ID]}.txt >> executionOutputs${tab1[$SLURM_ARRAY_TASK_ID]}.txt