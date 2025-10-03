
from Crypto.Util.number import *
import sympy

n = 340282366920938460843936948965011886881
e = 65537

factors = sympy.factorint(n)
p, q = factors.keys()

baza = (p - 1) * (q - 1)

d = inverse(e, baza)

print(f"Dekcripcija:\n{d}")