import requests

results = [
    requests.get("localhost:6678/model=test&prompt=Beep boop"),
    requests.get("localhost:6678/model=test"),
    requests.get()
]
expected = [ 
    "hi"
]
for res, index in enumerate([res.json() for res in results]):
    if res != expected[index]:
        exit -1

