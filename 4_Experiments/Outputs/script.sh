sbatch jobsbatch_2.sh
echo jobsbatch_2.sh >> okBatchs.txt
sbatch jobsbatch_1.sh
echo jobsbatch_1.sh >> okBatchs.txt
