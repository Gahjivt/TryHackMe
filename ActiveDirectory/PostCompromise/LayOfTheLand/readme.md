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

Kako bi provjerili stanje porta računala u kojem smo mozemo korisitit naredbu:
Test-NetConnection -ComputerName 127.0.0.1 -Port 22
(New-Object System.Net.Sockets.TcpClient("127.0.0.1", "80")).Connected

Logging
Get-EventLog -list
Get-Process | Where-Object { $_.ProcessName -eq "Sysmon" }
Get-Service | where-object {$_.DisplayName -like "*sysm*"}
Get-CimInstance win32_service -Filter "Description = 'System Monitor service'"
reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WINEVT\Channels\Microsoft-Windows-Sysmon/Operational
findstr /si '<ProcessCreate onmatch="exclude">' C:\tools\* #Provjera da vidimo sto sys administratori nadgledaju
