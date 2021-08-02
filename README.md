# Raylib Game Server
 a rough hack I put together to demostrate how to use ENet with raylib to create an online gaming experience
 
 ### HOW TO BUILD:
 there are 2 dependencies: ENet, and raylib. Both are included in the include/ folder but have to be compiled first. Instructions:
- [Raylib](http://enet.bespin.org/Installation.html)
- [ENet](http://enet.bespin.org/Installation.html)
- *If you already have them installed on your system there is a makefile in the build/ folder. You will need to change the ENET_INSTALL_PATH and RAYLIB_INSTALL_PATH variables for make to work.* 

After you have that sorted you should be able to go into the build/ folder and run ```make udp_client``` and ```make udp_server```. The server and client have to be compiled seperately. These applications should appear in the project directory and then can be ran directly.
