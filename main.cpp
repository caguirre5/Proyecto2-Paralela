#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

// Función para cifrar un mensaje con DES
void encryptDES(const std::string& plaintext, const std::string& key, std::string& ciphertext) {
    DES::Encryption desEncryption((byte*)key.data());
    ECB_Mode_ExternalCipher::Encryption ecbEncryption(desEncryption);

    StringSource encryptor(plaintext, true,
        new StreamTransformationFilter(ecbEncryption,
            new StringSink(ciphertext)
        )
    );
}

// Función para descifrar un mensaje con DES
void decryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
    DES::Decryption desDecryption((byte*)key.data());
    ECB_Mode_ExternalCipher::Decryption ecbDecryption(desDecryption);

    StringSource decryptor(ciphertext, true,
        new StreamTransformationFilter(ecbDecryption,
            new StringSink(plaintext)
        )
    );
}


int main() {
    std::string key = "12345678";  // Asegúrate de que la llave tenga 8 caracteres
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

    encryptDES(plaintext, key, ciphertext);
    std::cout << "Texto cifrado: " << ciphertext << std::endl;

    // Guardar el texto cifrado en un archivo
    std::ofstream outputFile("textoCifrado.txt");
    if (outputFile) {
        outputFile << ciphertext;
        outputFile.close();
        std::cout << "Texto cifrado guardado en 'textoCifrado.txt'." << std::endl;
    } else {
        std::cerr << "Error al guardar el archivo cifrado." << std::endl;
        return 1;
    }

    std::string decryptedText;
    decryptDES(ciphertext, key, decryptedText);
    std::cout << "Texto descifrado: " << decryptedText << std::endl;


    return 0;
}
