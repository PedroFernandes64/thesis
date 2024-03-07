sh ../src/makeHPC.sh
echo compiled
sbatch jobsbatch_5.sh
sbatch jobsbatch_2.sh
sbatch jobsbatch_3.sh
sbatch jobsbatch_1.sh
sbatch jobsbatch_4.sh
sbatch jobsbatch_6.sh
