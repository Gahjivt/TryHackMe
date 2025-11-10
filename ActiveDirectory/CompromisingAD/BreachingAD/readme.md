Breachign Active Directory Tryhackme

Prva metoda breachanja AD je preko NTMM i netNTLM
Trenutno u ovome modulu se prikazuje iskljucivo netNTML u web stranici i mozemo ju crackirati kroisteci broute force.
Za crackiranje potrebno nam je 4 stvari, url, fqdn (fully qualified domain name) i sifre/prezimena.
Mozemo koristiti hydru ili napisati vlastiti kod.

LDAP bind credentials
LDAP se koristi kako bi aplikacija mogla automatski verificirati korisnicke podatke.
Ako dobijemo pristup mrezi i pronađemo uređaj poput printera ili projektora koji je spojen  na LDAP sustav.
Način na koji mozemo exploitati ovo je da napravimo vlastiti ldap server/sustav koji ce oponasati tocan i preko cega cemo moci dobiti sifru koju koristi uređaj.
Kada dobijem informacije mogu sa tim korisnikom querijati AD bazu za podatke i imati authentificirani AD pristup.

Authentikacijski relay
U AD mrezama, kada korisnik zeli pristupiti nekom djeljenom direktoriju, ili sadrzaj na serveru, server mu salje NTLM challange koji haker prosljeđuje korisniku. 
Kada se korisnik autentificira, on salje serveru odgovoru na challange sa enkriptiranim informacijama (cesto sifre korisnika). Kada korisnik dobije te podatke može sam ubuduce koristiti server sa informacijom od tog korisnika.
Od alata se prvenstveno koristi responder.

Microsoft Deployment toolkit




Configuration files

