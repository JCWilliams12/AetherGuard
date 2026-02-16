# RadioScannerAndSummarizer

-=-=-=-=-=-=-TO RUN-=-=-=-=-=-=-=-=-=-
paste the following in the terminal:
Windows:
    cd server/build
    cmake --build .
    .\Debug\server.exe
Linux:
    brew install cmake
    cd server
    cmake -S . -B build
    cmake --build build
    cd build
    ./server

-=-=-=-=-=-=-For Crow.h Just in case-=-=-=-=-=-=-=-=-=-
wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/crow_all.h -O crow.h
sudo apt-get install -y libboost-all-dev



-=-=-=-=-=-=-Run Front End-=-=-=-=-=-=-=-=-=-
Paste the following into terminal: 
cd radio-scanner
npm install
npm run dev
