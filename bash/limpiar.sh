RUTA="/home/utnso/tp-2016-1c-Pusheo-luego-existo..."

# Elimina las carpetas bin logs
#eliminaCarpeta(){
#rm -r -f ${RUTA}/$1
#}
#eliminaCarpeta bin
#eliminaCarpeta logs

# Elimina todos los binarios objetos y dependencias
elimina(){
cd ${RUTA}/$1/Debug
rm -f ./$1
rm -f ./src/*.o
rm -f ./src/*.d
}

elimina consola
elimina cpu
elimina nucleo
elimina swap
elimina umc

# Elimina objetos y ejecutables de push librery
cd /home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library/Debug
rm -f ./libpush-library.so
rm -f ./push-library/*.o
rm -f ./push-library/*.d

# Elimina objetos y ejecutables de commons
sudo rm -rf /home/utnso/tp-2016-1c-Pusheo-luego-existo.../commonsso-commons-library/build
sudo rm -rf /home/utnso/tp-2016-1c-Pusheo-luego-existo.../parseransisop-parser/build

# Elimina swap.data
#rm -f ${RUTA}/swap/Debug/swap.data
#rm -f ${RUTA}/bash/swap.data
