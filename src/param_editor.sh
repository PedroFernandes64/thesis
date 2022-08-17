#! /bin/bash
nbd_online=$1
nbd_offline=$2
time=$3
blocking=$4
echo "removing all spain_n5 output  files"
#(./remove_results_spain.sh)
#echo "copying new param files"
#(rm -rf ./Parameters/Instances/spain_n5/*)
#(cp -rf ../../flexoptim/Parameters/Instances/spain_n5/* ../Parameters/Instances/spain_n5/)

 
echo "modifying online perm1"
######Perm1#######
file=onlineParameters.txt
#sed -i '6s/offline/online/' $file
sed -i 's/nbDemandsAtOnce=.*/nbDemandsAtOnce='$nbd_online' /' $file
sed -i 's/globalTimeLimit=.*/globalTimeLimit='$time' /' $file
#sed -i '/^obj=.*/a timeLimit=\nglobalTimeLimit='$time'\nallowBlocking=1\n******* Partition Parameters *******\npartitionPolicy=0\npartitionLoad=4\npartitionSlice=15' $file
#sed -i 's/allowBlocking=.*/allowBlocking='$blocking'/' $file
#sed '$d' $file
#sed  -i  's/maxNbIterations=100.*/maxNbIterations=100\n'/ $file
echo "END OF EDITING\n"
cat $file