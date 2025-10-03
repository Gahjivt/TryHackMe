
import jwt

secret_key = "sifra"

header = {
    "typ": "JWT",
    "alg": "HS256",
}

payload = {
    "sub": "127890",
    "name": "J Doe",
    "admin": True,
}

token = jwt.encode(payload, secret_key, algorithm="HS256", headers=header)

print(token)




try:
    decoded = jwt.decode(token, secret_key, algorithms=["HS256"])
    print("Secret is valid! Payload:", decoded)
except jwt.InvalidSignatureError:
    print("Secret is invalid!")
except jwt.ExpiredSignatureError:
    print("Token expired!")