
def enc(plaintext):
    return "".join(
        chr((ord(c) - (base := ord('A') if c.isupper() else ord('a')) + i) % 26 + base) 
        if c.isalpha() else c
        for i, c in enumerate(plaintext)
    )
#i index, c char u for loop
#ako je c broj onda ga prepisuje samo, ako slovo vriti enrkipciju
#prvo pronade njegovu unicode vrijednost -> oduzima sa bazom
#Baza
#ako je slovo veliko, onda je baza jednaka ord('A'), ako ne baza prima vrijednost ord('a')
#na sumu ord(c) i baze dodaje se index iteracije for petlje (u nasem slucaju slovo koje je u recenici-1)
#na kraju se modularno podjeli sa 26 i doda baza

#ovo je prikaz cezarove skripte, jedina razlika je sto se ne koristi staticni kljuc, nego se pomice preko vrijednosti i
def decript(enc_text):
    for i,c in enumerate(enc_text):
        if c.isupper():
            base=ord('A')
        elif c.islower():
            base=ord('a')
        else:
            print(c,end="")
            continue
        print(chr((ord(c)-base-i)%26+base),end="")

with open("message.txt", "w") as f:
    f.write(enc("a_sm4ll_crypt0_message_to_st4rt_with_THM_cracks"))
decript("a_up4qr_kaiaf0_bujktaz_qm_su4ux_cpbq_ETZ_rhrudm")



