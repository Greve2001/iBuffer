# iBuffer
LAN Buffer Writer &amp; Reader  
This software is made as a part of a course on DTU and is made without security as a focus point. Therefore, this should no be viewed as software for secure communication. The software uses plain text communication.

iBuffer is a small program that can host a buffer on one host and then others can connect through a pass phrase which consist of approximately 3 words. The connection of the client is made using a udp broadcast on the network, which after the server and client have found one and another is converter to a tcp connection. When the connection is up the server and host are able to exchange text over the network.  

The program is designed to work on local area networks and with the ease of use in mind.  

![Image of running program](https://github.com/Greve2001/iBuffer/blob/main/images/screenshot.png)

## Dependencies
 - gcc
 - glibc
 - ncurses
 
## How to install
### Ubuntu
```
sudo apt install libncurses5-dev libncursesw5-dev
git clone https://github.com/Greve2001/iBuffer.git
cd iBuffer && make
```
### Arch
```
sudo pacman -S ncurses
git clone https://github.com/Greve2001/iBuffer.git
cd iBuffer && make
```

## How to run
```
./iBuffer
```
