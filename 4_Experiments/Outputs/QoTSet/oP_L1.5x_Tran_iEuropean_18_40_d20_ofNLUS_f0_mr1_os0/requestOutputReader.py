import json

# Opening JSON file
f = open("requestOut.json")

# returns JSON object as 
# a dictionary
data = json.load(f)

# Iterating through the json
# list
for i in data["response"]:
    

    if "no-path" in i:
        print("Request " + i["response-id"] + " OSNR: " + str(i["no-path"]["path-properties"]["path-metric"][1]["accumulative-value"]) )
        print("Request " + i["response-id"] + " refused")
    else:
        print("Request " + i["response-id"] + " OSNR: " + str(i["path-properties"]["path-metric"][1]["accumulative-value"]) )
        print("Request " + i["response-id"] + " accepted")

# Closing file
f.close()