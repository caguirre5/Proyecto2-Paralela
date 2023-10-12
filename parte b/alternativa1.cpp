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

std::string EncryptAES(const std::string &plainText, const std::string &key)
{
    std::string cipherText;

    CryptoPP::AES::Encryption aesEncryption((byte *)key.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, (byte *)key.c_str());

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipherText));

    stfEncryptor.Put(reinterpret_cast<const unsigned char *>(plainText.c_str()), plainText.length());
    stfEncryptor.MessageEnd();

    return cipherText;
}

std::string DecryptAES(const std::string &cipherText, const std::string &key)
{
    std::string decryptedText;

    CryptoPP::AES::Decryption aesDecryption((byte *)key.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, (byte *)key.c_str());

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));

    stfDecryptor.Put(reinterpret_cast<const unsigned char *>(cipherText.c_str()), cipherText.length());
    stfDecryptor.MessageEnd();

    return decryptedText;
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

        for (int i = 1; i < size; i++)
        {
            int sendSize = (i <= remainder) ? (blockSize + 1) : blockSize;
            std::string block = plaintext.substr((i - 1) * blockSize, sendSize);

            // Envía cada bloque al proceso correspondiente
            MPI_Send(&sendSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(block.c_str(), sendSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < size; i++)
        {
            int recvSize;
            MPI_Recv(&recvSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::string receivedBlock(recvSize, '\0');
            MPI_Recv(&receivedBlock[0], recvSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ciphertext += receivedBlock;
        }

        // Mostrar el texto cifrado
        std::cout << "Texto cifrado: " << ciphertext << std::endl;

        // Mostrar el texto cifrado o descifrado
        if (mode == "cifrar")
        {
            std::cout << "Texto cifrado: " << ciphertext << std::endl;

            // Guardar el resultado cifrado en un archivo de salida
            std::ofstream outFile("cifrado_alternativa1.txt");
            outFile << ciphertext;
            outFile.close();
        }
        else if (mode == "descifrar")
        {
            std::cout << "Texto descifrado: " << ciphertext << std::endl;

            // Guardar el resultado descifrado en un archivo de salida
            std::ofstream outFile("descifrado_alternativa1.txt");
            outFile << ciphertext;
            outFile.close();
        }
    }
    else
    {
        // Procesos secundarios (rango > 0) realizan el cifrado/descifrado y envían el resultado al proceso principal.

        int mySize;
        MPI_Recv(&mySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::string myBlock(mySize, '\0');
        MPI_Recv(&myBlock[0], mySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::string myResult;

        if (mode == "cifrar")
        {
            myResult = EncryptAES(myBlock, key);
        }
        else if (mode == "descifrar")
        {
            myResult = DecryptAES(myBlock, key);
        }

        MPI_Send(&mySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&myResult[0], mySize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
