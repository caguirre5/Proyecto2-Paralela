/**
 * authors:
 *  - Marco Jurado
 *  - Cristian Aguirre
 *  - Paola de Leon
 * cifrado_con_llave.cpp
 * Programa para cifrar un mensaje de texto utilizando el algoritmo DES.
 * La clave privada arbitraria se pasa como argumento junto con el nombre del archivo de entrada.
 * Hace uso de paralelizacion con OpenMPI
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
    MPI_Init(&argc, &argv); // Init de MPI

    if (argc != 4)
    {
        std::cerr << "Uso: " << argv[0] << " <clave> <archivo de entrada> <modo (cifrar o descifrar)>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Parametros de MPI
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        std::cerr << "Este programa debe ejecutarse con al menos 2 procesos MPI." << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Parametros
    std::string key = argv[1];           // La clave se pasa como argumento para la encripcion o descencripcion
    std::string inputFileName = argv[2]; // El nombre del archivo ahora es un parametro
    std::string mode = argv[3];          // El modo a utilizar

    if (rank == 0)
    {
        // Si estamos en el rank 0

        // Definiciones
        std::string text, ciphertext;

        // Leer con stream
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

        /**
         * Ahora vamos a verificar antes si estamos recibiendo
         * cifrar o descifrar para ahorrar el uso de recursos
         * con el resto de la ejecucion
         */
        if (mode != "cifrar" && mode != "descifrar")
        {
            // Ingreso algo que no es valido
            std::cerr << "Modo no válido. Solamente puede ser 'cifrar' o 'descifrar'." << std::endl;
            MPI_Finalize();
            return 1; // retorno error
        }

        int textLength = text.length();    // Size del texto a trabajar
        int blockSize = textLength / size; // Lo dividimos en los hilos de MPI
        int remainder = textLength % size; // Observamos el residuo de la division

        // Enviamos la respectiva informacion a cada hilo del MPI
        for (int i = 1; i < size; i++)
        {
            int sendSize = (i < remainder) ? (blockSize + 1) : blockSize; // Size de lo que se enviara

            // Instrucciones para enviar con MPI
            MPI_Send(&sendSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(text.c_str() + i * blockSize + std::min(i, remainder), sendSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        std::string myBlock((rank < remainder) ? (blockSize + 1) : blockSize, '\0');
        myBlock = text.substr(rank * blockSize + std::min(rank, remainder), myBlock.size()); // Bloque definido por el size que el hilo obtiene

        if (mode == "cifrar")
        {
            encryptDES(myBlock, key, ciphertext);
        }
        else
        {
            decryptDES(myBlock, key, ciphertext);
        }

        // Recibimos la info de los hilos ya procesada
        for (int i = 1; i < size; i++)
        {
            int recvSize;
            MPI_Recv(&recvSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::string receivedBlock(recvSize, '\0');
            MPI_Recv(&receivedBlock[0], recvSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ciphertext += receivedBlock;
        }

        // Para saber que hace cada proceso :)
        std::cout << "Texto procesado por el proceso " << rank << ": " << ciphertext << std::endl;
    }
    else
    {
        // Ocurre el proceso de encriptado dentro de los otros hilos

        int mySize;
        MPI_Recv(&mySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibe el texto a trabajar
        std::string myBlock(mySize, '\0');
        MPI_Recv(&myBlock[0], mySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::string myCiphertext; // Define el texto

        if (mode == "cifrar")
        {
            encryptDES(myBlock, key, myCiphertext);
        }
        else
        {
            decryptDES(myBlock, key, myCiphertext);
        }

        // Luego envia al rank 0 la informacion procesada
        MPI_Send(&mySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&myCiphertext[0], mySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); // Fin MPI
    return 0;
}
