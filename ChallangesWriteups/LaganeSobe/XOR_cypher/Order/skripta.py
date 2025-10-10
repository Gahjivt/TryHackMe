"""
We intercepted one of Cipher's messages containing their next target. They encrypted their message using a repeating-key XOR cipher. However, they made a critical error—every message always starts with the header:

ORDER:
Can you help void decrypt the message and determine their next target?
Here is the message we intercepted:

1c1c01041963730f31352a3a386e24356b3d32392b6f6b0d323c22243f6373

1a0d0c302d3b2b1a292a3a38282c2f222d2a112d282c31202d2d2e24352e60
"""
import string
from pwn import *
a="1c1c01041963730f31352a3a386e24356b3d32392b6f6b0d323c22243f6373"

b="1a0d0c302d3b2b1a292a3a38282c2f222d2a112d282c31202d2d2e24352e60"

a=bytes.fromhex(a)
b=bytes.fromhex(b)
arraySifri=[a,b]
pocetak_zastave="ORDER:"
pocetak_kljuca=xor(a,pocetak_zastave)[:6]
print(pocetak_kljuca) #kljuc za xoranje
zastava=xor(a+b,pocetak_kljuca)
print(zastava)
