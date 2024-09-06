import json
import sys

path = ""

if len(sys.argv) >1:
    path = path +sys.argv[1]
   
# Opening JSON file
f = open(path+"requestOut.json")

# returns JSON object as 
# a dictionary
data = json.load(f)

# Iterating through the json
# list

with open(path+"outAux.csv", "a") as f2:
    for i in data["response"]:
        if "no-path" in i:
            f2.write(i["response-id"] + ";" + str(i["no-path"]["path-properties"]["path-metric"][1]["accumulative-value"]) +"\n")
        else: 
            f2.write(i["response-id"] + ";" + str(i["path-properties"]["path-metric"][1]["accumulative-value"]) +"\n")    
    f2.close()
    # Closing file
f.close()