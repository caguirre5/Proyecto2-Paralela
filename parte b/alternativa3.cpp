/**
 * authors:
 *  - Marco Jurado
 *  - Cristian Aguirre
 *  - Paola de Leon
 * cifrado_con_llave.cpp
 * Programa para cifrar un mensaje de texto utilizando el algoritmo DES.
 * La clave privada arbitraria se pasa como argumento junto con el nombre del archivo de entrada.
 * Hace uso de paralelizacion con OpenMPI y esta es una alternativa que toma y divide el texto en
 * bloques del mismo size.
 *
 * uso:
 *   - ./cifrado_con_llave <clave> <archivo de entrada> <modo (cifrar o descifrar)>

*/

#include <iostream>
#include <fstream>
#include <mpi.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

// Cifrar un mensaje con DES usando una llave (por ejemplo 42)
void encriptado(const std::string &plaintext, const stdstring &key, std::string &ciphertext)
{
    DES::Encryption desEncryption((byte *)key.data(), DES::DEFAULT_KEYLENGTH);
    ECB_Mode_ExternalCipher::Encryption ecbEncryption(desEncryption);

    StringSource encryptor(plaintext, true, new StreamTransformationFilter(ecbEncryption, new StringSink(ciphertext)));
}

// Descifrar un mensaje con DES usando una llave (por ejemplo 42)
void descencriptado(const std::string &ciphertext, const std::string &key, std::string &plaintext)
{
    DES::Decryption desDecryption((byte *)key.data(), DES::DEFAULT_KEYLENGTH);
    ECB_Mode_ExternalCipher::Decryption ecbDecryption(desDecryption);

    StringSource decryptor(ciphertext, true, new StreamTransformationFilter(ecbDecryption, new StringSink(plaintext)));
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv); // Inicialización de MPI

    if (argc != 4)
    {
        std::cerr << "Uso: " << argv[0] << " <clave> <archivo de entrada> <modo (cifrar o descifrar)>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Parámetros de MPI
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        std::cerr << "Este programa debe ejecutarse con al menos 2 procesos MPI." << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Parámetros
    std::string key = argv[1];           // La clave se pasa como argumento para la encripción o desencripción
    std::string inputFileName = argv[2]; // El nombre del archivo ahora es un parámetro
    std::string mode = argv[3];          // El modo a utilizar

    if (rank == 0)
    {
        // Si estamos en el rango 0

        // Definiciones
        std::string text, ciphertext;

        // Leer el archivo de entrada
        std::ifstream file(inputFileName);
        if (file)
        {
            text.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
        }
        else
        {
            std::cerr << "Error al abrir el archivo." << std::endl;
            MPI_Finalize();
            return 1;
        }

        // Verificar el modo (cifrar o descifrar)
        if (mode != "cifrar" && mode != "descifrar")
        {
            // Modo no válido
            std::cerr << "Modo no válido. Solamente puede ser 'cifrar' o 'descifrar'." << std::endl;
            MPI_Finalize();
            return 1; // Retorno de error
        }

        int textLength = text.length();    // Tamaño del texto a trabajar
        int blockSize = textLength / size; // Dividimos el texto en bloques de igual tamaño
        int remainder = textLength % size; // Observamos el residuo de la división

        int myBlockSize = blockSize; // Tamaño del bloque para el proceso actual
        if (rank < remainder)
        {
            // Si el rango es menor que el residuo, se le asigna un bloque adicional
            myBlockSize++;
        }

        // Calcular el desplazamiento
        int displacement = 0;
        for (int i = 0; i < rank; i++)
        {
            displacement += (i < remainder) ? (blockSize + 1) : blockSize;
        }

        // Crear un tipo de dato MPI para describir un bloque
        MPI_Datatype blockType;
        MPI_Type_contiguous(myBlockSize, MPI_CHAR, &blockType);
        MPI_Type_commit(&blockType);

        // Dividir el texto en bloques de igual tamaño
        char *myBlock = new char[myBlockSize];

        // Distribuir los bloques a cada proceso
        MPI_Scatterv(text.c_str(), &myBlockSize, &displacement, blockType, myBlock, myBlockSize, MPI_CHAR, 0, MPI_COMM_WORLD);

        std::string myText(myBlock, myBlock + myBlockSize);

        // Realizar cifrado o descifrado
        std::string myCiphertext;

        if (mode == "cifrar")
        {
            encriptado(myText, key, myCiphertext);
        }
        else
        {
            descencriptado(myText, key, myCiphertext);
        }

        // Reunir los resultados en el proceso 0
        MPI_Gatherv(myCiphertext.c_str(), myCiphertext.length(), MPI_CHAR, ciphertext.data(), nullptr, nullptr, MPI_CHAR, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            // Mostrar el texto cifrado o descifrado
            std::cout << "Texto procesado por el proceso " << rank << ": " << ciphertext << std::endl;

            // Guardar el resultado en un archivo de salida
            std::ofstream outFile("resultado_alternativa3.txt");
            outFile << ciphertext;
            outFile.close();
        }

        delete[] myBlock;
    }
    else
    {
        // Procesos secundarios (rango > 0) realizan el cifrado/descifrado

        // Resto del código...

    }

    MPI_Finalize(); // Fin MPI
    return 0;
}