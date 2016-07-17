RUTA="/home/utnso/tp-2016-1c-Pusheo-luego-existo..."

(cd /home/utnso/tp-2016-1c-Pusheo-luego-existo.../commons && sudo make install ) > make.dat
rm -f make.dat

(cd /home/utnso/tp-2016-1c-Pusheo-luego-existo.../parser && sudo make install ) > make.dat
rm -f make.dat

# Compila todos los procesos
compila(){
(cd ${RUTA}/$1/Debug && make all) > make.dat
}

compila consola
compila cpu
compila nucleo
compila swap
compila umc
