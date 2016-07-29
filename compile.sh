#!/bin/bash
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make all
sudo make install
cd ..

cd ansisop-parser
cd parser
sudo make uninstall
sudo make clean
sudo make all
sudo make install
cd ..
cd ..

cd push-library
cd Debug
sudo make clean
sudo make all
cd ..
cd ..

cd cpu
cd Debug
sudo make clean
sudo make all
cd ..
cd ..

cd nucleo
cd Debug
sudo make clean
sudo make all
cd ..
cd ..

cd consola
cd Debug
sudo make clean
sudo make all
sudo cp ansisop /usr/bin/ansisop
cd ..
cd ..

cd swap
cd Debug
sudo make clean
sudo make all
cd ..
cd ..

cd umc
cd Debug
sudo make clean
sudo make all
cd ..
cd ..

cd scripts
chmod +x completo.ansisop
chmod +x consumidor.ansisop
chmod +x deref-prioridad.ansisop
chmod +x facil.ansisop
chmod +x fibo.ansisop
chmod +x for.ansisop
chmod +x forES.ansisop
chmod +x mcd.ansisop
chmod +x pesado.ansisop
chmod +x productor.ansisop
chmod +x segfault.ansisop
chmod +x stackoverflow.ansisop
chmod +x vector.ansisop
cd ..
