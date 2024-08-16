rm network.json
echo generating network file
python3 translator.py >> network.json
rm request.json
echo generating request file
python3 requestWriter.py >> request.json
rm requestOut.json
echo executing gnpy
gnpy-path-request -o requestOut.json network.json request.json -e equipments.json 
echo generating output file
python3 requestOutputReader.py >> outAux.json