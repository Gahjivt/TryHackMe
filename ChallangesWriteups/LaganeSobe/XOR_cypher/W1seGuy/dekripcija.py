#kljuc dolazi u stilu
# 123c2836367715092332030c110c323240062625071a177e272a381c251334001c7d33340c2a3f3b
#Po kodu vidimo da prov trebamo dekodirati iz hexa, te onda xortai da se vrati na isto
import string
from pwn import *
a = "232905191f4600240c1b32193c231b03552b090c360f3a510e1b2d310a3a051531521a0519071012"

"""

def setup(server, key):
    flag = 'THM{thisisafakeflag}' 
    xored = ""

    for i in range(0,len(flag)):
        xored += chr(ord(flag[i]) ^ ord(key[i%len(key)]))

    hex_encoded = xored.encode().hex()
    return hex_encoded

"""

a=bytes.fromhex(a)
print(a)
#sada vise nije hex i trebamo pronaci key
#Znamo da key pocinje sa TH{ i zavrsava sa }
#Zbog toga cemo pokusati pronaci koji key se koristio za xoranje
#Po kodu zadatka znamo da je xor key jednak 5

pocetak_zastave="THM{"
pocetak_kljuca=xor(a,pocetak_zastave)[:5]
print(pocetak_kljuca) #znamo prvih 4 bita kljuca, fali nam 5, sada postoje 2 mogucnosti, ili cemo rucno ici u cyberchefu mjenjati taj znak sa svim znakvoima abecede i brojevima, ili napraviti dodatnu provjeru sa zadnjim elementom iz kljuca }
for c in string.ascii_letters+string.digits:
    kljuc=pocetak_kljuca[:4]+c.encode()
    zastava=xor(a,kljuc).decode()
    if zastava[-1]=="}":
        print("Zastava: ", zastava, " -> Kljuc: ",kljuc.decode("utf-8"))