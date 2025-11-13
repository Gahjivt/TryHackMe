Windows

System

systeminfo
wmic qfe get Caption,Description #instalirana azuriranja
net start #instaliran software
wmic product get name, version, vendor #instalirani programi

Users

whoami
whoami /priv
whoami /groups
net user
net localgroup
net localgroup administrators

Networking

ipconfig
netstat -abno
arp -a


Linux

System:

ls /etc/*-release
hostname
cat /etc/passwd
cat /etc/shadow
cat /etc/group

ls -lh /var/mail

Instalirane aplikacije:

ls -lh /usr/bin/
ls -lh /sbin/

Instalirani paketi:

dpkg -l ILI rpm -qa (debian, rpm)

Korisnici

who
whoami
w
id
sudo -l
last
lsof

Network:

ip a s
cat /etc/resolv.conf
netstat -plt

lsof :25 //list of porta koje slusaju i njihove konekcije

Servisi:

pa axf
ps -ef |grep ime
