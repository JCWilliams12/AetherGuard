# RadioScannerAndSummarizer

-=-=-=-=-=-=-=DATABASE=-=-=-=-=-=-=-
make sure SQLite is installed on the machine
Instalation:
1) install SQLite at https://www.tutorialspoint.com/sqlite/sqlite_installation.htm 
2) Download sqlite-tools-win-x64-3510100.zip and sqlite-dll-win-x64-3510100.zip
3) Make sqlite folder in C:   (C:\sqlite)
4) move contents of .zips into the sqlite folder
5) Add C:\sqlite in your PATH environment variable

-=-=-=-=-=-=-TO RUN-=-=-=-=-=-=-=-=-=-
paste the following in the terminal:
Windows:
    mingw32-make
    ./main.exe
Linux:
    make
    ./main

-=-=-=-=-=-=-For Crow.h Just in case-=-=-=-=-=-=-=-=-=-
wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/crow_all.h -O crow.h
sudo apt-get install -y libboost-all-dev



-=-=-=-=-=-=-Run Front End-=-=-=-=-=-=-=-=-=-
Paste the following into terminal: 
cd radio-scanner
npm install
npm run dev
