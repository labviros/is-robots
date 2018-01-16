Aria Installation
====================

```bash
# Drivers (ARIA)
wget http://robots.mobilerobots.com/ARIA/download/current/ARIA-src-2.9.1.tar.gz
tar -xf ARIA-src-2.9.1.tar.gz
mv Aria-src-2.9.1 Aria # Necessary to install the simulator 
cd Aria
make allLibs
sudo cp lib/* /usr/local/lib
sudo mkdir /usr/local/include/Aria
sudo cp include/* /usr/local/include/Aria
cd ..

# Simulator (MobileSim) 
# Apparently this needs to build libaria.a so you need to have aria source located on ../Aria 
wget http://robots.mobilerobots.com/MobileSim/download/archives/MobileSim-src-0.7.3.tgz
tar -xf MobileSim-src-0.7.3.tgz
cd MobileSim-src-0.7.3
make MobileSim 
sudo make install
```