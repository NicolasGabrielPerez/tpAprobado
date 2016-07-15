#!/bin/bash
cd so-commons-library
sudo make install
cd ..

cd ansisop-parser
cd parser
make all
sudo make install
cd ..
cd ..

cd push-library
make all
sudo make install
cd ..

cd test_scripts
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
