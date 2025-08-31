
import jwt

secret_key = "sifra"

header = {
    "typ": "JWT",
    "alg": "HS256",
}

# Payload with the 'admin' role
payload = {
    "sub": "127890",
    "name": "J Doe",
    "admin": True,
}

# Encode the JWT with the specific header
token = jwt.encode(payload, secret_key, algorithm="HS256", headers=header)

# Print the generated token
print(token)


import jwt

secret_key = "sifra"

try:
    decoded = jwt.decode(token, secret_key, algorithms=["HS256"])
    print("Secret is valid! Payload:", decoded)
except jwt.InvalidSignatureError:
    print("Secret is invalid!")
except jwt.ExpiredSignatureError:
    print("Token expired!")