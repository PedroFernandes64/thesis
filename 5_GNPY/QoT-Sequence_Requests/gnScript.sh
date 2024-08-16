rm network.json
echo generating network file
python3 translator.py >> network.json
echo generating request files and computing
python3 requestWriter.py
