# Programa de Cifrado y Descifrado DES

# **DES_encrypt.cpp**

Este programa de ejemplo está diseñado para cifrar y descifrar un mensaje utilizando el algoritmo de cifrado DES (Data Encryption Standard) de Crypto++.

### Descripcion

Este programa define dos funciones principales
*encryptDES: encripta un texto*
*decryptDES: desencripta un texto*

El programa abre el archivo *texto.txt* y extrae el texto en el. Esto es encriptado y el resultado (el texto encriptado) es guardado en el archivo *textoCifrado.txt*

### Requisitos

Antes de utilizar este programa, asegúrate de tener instalada la biblioteca Crypto++ en tu sistema. Puedes encontrar más información sobre cómo instalarla en [el sitio web oficial de Crypto++](https://www.cryptopp.com/wiki/Installation).

**sudo apt-get install libcrypto++-dev**

### Uso

Para utilizar el programa, sigue estos pasos:

1. Compila el programa: Abre una terminal en el directorio donde se encuentra el código fuente y ejecuta el siguiente comando para compilar el programa:

g++ -o <nombre_ejecutable> <nombre_archivo>.cpp -lcryptopp


2. Correr el programa: 

./programa <clave_secreta>

*NOTA: La clave debe ser numerica*


# **bruteforce_seq.cpp**

Este programa es una herramienta diseñada para descifrar un texto cifrado utilizando el algoritmo de cifrado DES (Data Encryption Standard) de Crypto++. El programa se enfoca en encontrar la clave utilizada para cifrar el texto encriptado y luego utilizar esa clave para descifrar el mensaje.

### Descripción

El programa define una función principal llamada tryDecryptDES, que se encarga de probar diferentes claves y desencriptar un texto cifrado. La clave utilizada para descifrar se determina al buscar una palabra o frase clave proporcionada por el usuario.

El programa intentará descifrar el mensaje cifrado utilizando diferentes claves en un rango numérico (de 0 a 99999999) y verificará si la palabra o frase clave deseada se encuentra en el texto descifrado.

### Requisitos

Antes de utilizar este programa, asegúrate de tener instalada la biblioteca Crypto++ en tu sistema. Puedes encontrar más información sobre cómo instalarla en el sitio web oficial de Crypto++.

**sudo apt-get install libcrypto++-dev**

### Uso

Compila el programa: Abre una terminal en el directorio donde se encuentra el código fuente y ejecuta el siguiente comando para compilar el programa:

**g++ -o programa programa.cpp -lcryptopp**

Ejecuta el programa:

**./programa '<palabra_clave>' <numero de caracteres de la llave>**

*NOTA: el parametro debe estar entre comillas ya que es un string que puede ser una palabra o bien una frase*

### Resultado

El programa mostrará la clave encontrada y el texto descifrado si la palabra clave se encuentra en el mensaje descifrado. En caso contrario, informará que la clave no se encontró o que la palabra clave no está en el texto descifrado.

# **bruteforce_mpi.cpp**

Compila el programa: Abre una terminal en el directorio donde se encuentra el código fuente y ejecuta el siguiente comando para compilar el programa:

**mpicxx -o decrypt_mpi bruteforce_mpi.cpp -lcryptopp**

Ejecuta el programa:

**mpirun -np 4 ./decrypt_mpi '<palabra_clave>' <numero de caracteres de la llave>**
