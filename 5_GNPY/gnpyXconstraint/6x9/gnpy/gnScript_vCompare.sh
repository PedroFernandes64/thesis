rm network.json
echo generating network file
python3 translator.py
echo generating request files and computing
python3 requestWriter_vCompare.py
