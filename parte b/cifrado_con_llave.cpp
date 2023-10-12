/**
 * authors:
 *  - Marco Jurado
 *  - Cristian Aguirre
 *  - Paola de Leon
 * cifrado_con_llave.cpp
 * Programa para cifrar un mensaje de texto utilizando el algoritmo DES.
 * La clave privada arbitraria se pasa como argumento junto con el nombre del archivo de entrada.
 *
 * uso:
 *   - ./cifrado_con_llave <clave> <archivo de entrada> <modo (cifrar o descifrar)>

*/

#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

// Cifrar un mensaje con DES usando una llave (por ejemplo 42)
void encriptado(const std::string &plaintext, const std::string &key, std::string &ciphertext)
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
    if (argc != 4)
    {
        std::cerr << "Uso: " << argv[0] << " <clave> <archivo de entrada> <modo (cifrar o descifrar)>" << std::endl;
        return 1;
    }

    // Parametros
    std::string key = argv[1];           // La clave se pasa como argumento para la encripcion o descencripcion
    std::string inputFileName = argv[2]; // El nombre del archivo ahora es un parametro
    std::string mode = argv[3];          // El modo a utilizar

    // Definiciones
    std::string plaintext, ciphertext;

    // Leer con stream
    std::ifstream file(inputFileName);
    if (file)
    {
        // Si existe el archivo
        plaintext.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    }
    else
    {
        // Si no existe el archivo
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 1;
    }

    if (mode == "cifrar")
    {
        // Cifrar el txt
        encriptado(text, key, processedText); // Encriptar
        std::cout << "Texto cifrado: " << processedText << std::endl;
        // Guardar el resultado en un archivo de salida
        std::ofstream outFile("resultado_cifrado.txt");
        outFile << ciphertext;
        outFile.close();
    }
    else if (mode == "descifrar")
    {
        // Descifrar el txt
        descencriptado(text, key, processedText); // Desencriptar
        std::cout << "Texto descifrado: " << processedText << std::endl;
        // Guardar el resultado en un archivo de salida
        std::ofstream outFile("resultado_descifrado.txt");
        outFile << ciphertext;
        outFile.close();
    }
    else
    {
        // Ingreso algo que no es valido
        std::cerr << "Modo no válido. Solamente puede ser 'cifrar' o 'descifrar'." << std::endl;
        return 1; // retorno error
    }

    return 0;
}
