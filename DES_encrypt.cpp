/**
 * Autores:
 * - Marco Jurado
 * - Cristian Aguirre
 * - Paola de Leon
 */

#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

// Función para cifrar un mensaje con DES
void encryptDES(const std::string& plaintext, const std::string& key, std::string& ciphertext) {
    // Crear un objeto de cifrado DES
    DES::Encryption desEncryption((byte*)key.data());
    // Modo de cifrado ECB (Electronic Codebook)
    ECB_Mode_ExternalCipher::Encryption ecbEncryption(desEncryption);

    // Realizar la operación de cifrado y almacenar el resultado en ciphertext
    StringSource encryptor(plaintext, true,
        new StreamTransformationFilter(ecbEncryption,
            new StringSink(ciphertext)
        )
    );
}

// Función para descifrar un mensaje con DES
void decryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
    // Crear un objeto de descifrado DES
    DES::Decryption desDecryption((byte*)key.data());
    // Modo de descifrado ECB (Electronic Codebook)
    ECB_Mode_ExternalCipher::Decryption ecbDecryption(desDecryption);

    // Realizar la operación de descifrado y almacenar el resultado en plaintext
    StringSource decryptor(ciphertext, true,
        new StreamTransformationFilter(ecbDecryption,
            new StringSink(plaintext)
        )
    );
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <clave>" << std::endl;
        return 1;
    }

    std::string key = argv[1];
    // Usando length()
    std::cout << "\nLongitud de la llave: " << key.length() << std::endl;

    std::string plaintext, ciphertext;

    // Leer el mensaje desde un archivo
    std::ifstream file("texto.txt");
    if (file) {
        plaintext.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    } else {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 1;
    }

    // Cifrar el mensaje
    encryptDES(plaintext, key, ciphertext);
    // Guardar el texto cifrado en un archivo
    std::ofstream outputFile("textoCifrado.txt");
    if (outputFile) {
        outputFile << ciphertext;
        outputFile.close();
        std::cout << "\nTexto cifrado guardado en 'textoCifrado.txt'." << std::endl;
    } else {
        std::cerr << "Error al guardar el archivo cifrado." << std::endl;
        return 1;
    }

    std::string decryptedText;
    // Descifrar el mensaje
    decryptDES(ciphertext, key, decryptedText);
    std::cout << "Texto descifrado: " << decryptedText << std::endl;

    return 0;
}
