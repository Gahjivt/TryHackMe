Scoutanje network infrastrukture
Pri ulasku u mrežu vršimo 
netstat -na i arp -a za pretragu otvorenih portova i mapirannje mac adresa
Ako zelimo saznati još neke podatke o sustavu mozemo koristiti naredbu: 

systeminfo

Users and group management
Enumeracija korisnika i grupa
Get-ADUser -Filter *
Get-ADUSER -Filter 'Name -like "%Admin%"' -Properties * | Format-Table Name,SamAccountName -A
Get-ADUser -Filter * -SearchBase

Host security solutions
Security se dijeli network i host.
Provjera antivirusa:
wmic /namespace:\\root\securitycenter2 path antivirusproduct
Get-CimInstance -Namespace root/SecurityCenter2 -ClassName AntivirusProduct
Get-Service WinDefend #provjera u kojemu je stanju windefend
Get-MpComputerStatus | select RealTimeProtectionEnabled #Provjera je li enablana RTP
Get-NetFirewallProfile | Format-Table Name, Enabled

Ako smo admin mozemo pokusati ugasiti zastitu.
Set-NetFirewallProfile -Profile Domain, Public, Private -Enabled False

Get-NetFirewallRule | select DisplayName, Enabled, Description

Kako bi provjerili stanje porta računala u kojem smo mozemo korisitit naredbu:
Test-NetConnection -ComputerName 127.0.0.1 -Port 22
(New-Object System.Net.Sockets.TcpClient("127.0.0.1", "80")).Connected

Logging
Get-MpThreat
Get-EventLog -list
Get-Process | Where-Object { $_.ProcessName -eq "Sysmon" }
Get-Service | where-object {$_.DisplayName -like "*sysm*"}
Get-CimInstance win32_service -Filter "Description = 'System Monitor service'"
reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WINEVT\Channels\Microsoft-Windows-Sysmon/Operational
findstr /si '<ProcessCreate onmatch="exclude">' C:\tools\* #Provjera da vidimo sto sys administratori nadgledaju

Applications i servisi

U windows sustavu ako zelimo izlistati vecinu instaliranih aplikacija koristimo:

wmic product get name,version,...

Ako zelimo jos detaljnije i preciznije pregledati situaciju koristimo:

PS C:\Users\thm> Get-ChildItem -Hidden -Path C:\Users\kkidd\Desktop\

Kako bi ispisali windows servise koji su pokrenuti koristimo naredbu

net start

Ako zelimo saznati dodatne informacije o servisu koristimo:

wmic service where "name like 'THM Demo'" get Name,PathName
Get-Process -Name thm-demo
netstat -noa |findstr "LISTENING" |findstr "ID procesa"
 
I tako cemo pronaci na kojemu se portu vrti servis i mozemo ga posjetiti.

DNS zone transfer

kako bi to izvršili trebamo pokrenuti:

nslookup.exe
 
i querijamo neki serveru u mreži sa:

> server 10.10.18.184
Default Server:  ip-10-10-18-184.eu-west-1.compute.internal
Address:  10.10.18.184

i na kraju izvršavamo dns zone transfer sa:

ls -d <Domena>

Ako pokusamo napisati ip adresu ili Default-server dobiti cemo ovo:
> ls -d 10.10.18.184
[ip-10-10-18-184.eu-west-1.compute.internal]
*** Can't list domain 10.10.18.184: Non-existent domain
The DNS server refused to transfer the zone 10.10.18.184 to your computer. If this
is incorrect, check the zone transfer security settings for 10.10.18.184 on the DNS
server at IP address 10.10.18.184.

> ls -d ip-10-10-18-184.eu-west-1.compute.internal
[ip-10-10-18-184.eu-west-1.compute.internal]
*** Can't list domain ip-10-10-18-184.eu-west-1.compute.internal: Non-existent domain
The DNS server refused to transfer the zone ip-10-10-18-184.eu-west-1.compute.internal to your computer. If this
is incorrect, check the zone transfer security settings for ip-10-10-18-184.eu-west-1.compute.internal on the DNS
server at IP address 10.10.18.184.

Največi problem je doci do informacije koju domenu korisitit.
Generalno ta informacija se dobiva preko enumeracije (DNS Zone Name Enumeration) i korištenjem alata poput dnsrecon/dig/kerberos,...