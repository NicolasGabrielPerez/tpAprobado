# Edita el archivo .bashrc para ejecutar desde la consola
echo "" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~tp-2016-1c-Pusheo-luego-existo.../push-library/Debug" >> ~/.bashrc
exec bash --login #Reinicia la consola para volver a leer el .bashrc y cargar la variable de librerias
