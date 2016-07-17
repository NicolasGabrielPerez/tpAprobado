RUTA="/home/utnso/tp-2016-1c-Pusheo-luego-existo..."

# Si existen:  las carpetas bin log y test;  los objetos y ejecutables de todos los procesos los elimina
./limpiar.sh

# Crea las carpetas bin logs y test
#crearCarpeta(){
#mkdir ${RUTA}/$1
#}
#crearCarpeta bin
#crearCarpeta logs
#crearCarpeta test

# Make de todos los procesos
./makeTodo.sh

./copiarBin_Cfg.sh

#Si no esta en el .bashrc declarado el exports, ejecuto el exports.sh. TIENE QUE IR AL FINAL SIEMPRE PORQUE EXPORTS.SH REINICIA CONSOLA
if !(grep -q "export LD_LIBRARY_PATH" ~/.bashrc) then
	./exports.sh
fi

# Copia los scripts en la carpeta bin
#cp -f ${RUTA}/scripts/* ${RUTA}/bin/
