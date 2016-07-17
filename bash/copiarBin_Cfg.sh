RUTA="/home/utnso/tp-2016-1c-Pusheo-luego-existo..."

# Copiar los binarios en ./bin
copiarBin(){
cp ${RUTA}/$1/Debug/$1 ${RUTA}/bin/$1
}

copiarBin consola
copiarBin cpu
copiarBin nucleo
copiarBin swap
copiarBin umc

# Copia los config en ./bin
copiarCfg(){
cp ${RUTA}/config/$1 ${RUTA}/bin/$1
}

copiarCfg consola.config
copiarCfg cpu.config
copiarCfg nucleo.config
copiarCfg swap.config
copiarCfg umc.config
