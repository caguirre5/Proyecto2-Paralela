#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <typeinfo>

using namespace CryptoPP;

// Función para descifrar un mensaje con DES
bool tryDecryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
    // std::cout << "La llave se encontro." << key << "." << std::endl;
    try {
        DES::Decryption desDecryption((byte*)key.data());
        ECB_Mode_ExternalCipher::Decryption ecbDecryption(desDecryption);

        StringSource decryptor(ciphertext, true,
            new StreamTransformationFilter(ecbDecryption,
                new StringSink(plaintext)
            )
        );

        return true;
    } catch (CryptoPP::Exception& e) {
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <palabra_clave> <num_caracteres_llave>" << std::endl;
        return 1;
    }

    const std::string keyword = argv[1]; // Obtener la palabra clave desde la línea de comandos
    int numCaracteresLlave = std::stoi(argv[2]); // Convertir el número de caracteres de la llave en entero

    std::string decryptedText;

    // Leer el texto cifrado desde un archivo
    std::ifstream encryptedFile("textoCifrado.txt");
    if (!encryptedFile) {
        std::cerr << "Error al abrir el archivo cifrado para descifrar." << std::endl;
        return 1;
    }

    std::string encryptedText((std::istreambuf_iterator<char>(encryptedFile)), std::istreambuf_iterator<char>());
    encryptedFile.close();

    bool keyFound = false;
    std::string foundKey;
    

    for (int candidate = 0; candidate <= std::pow(10, numCaracteresLlave) - 1; candidate++) {
        // Convertir el número de candidato a una cadena de longitud numCaracteresLlave rellenada con ceros
        std::string candidateKey = std::to_string(candidate);
        while (candidateKey.length() < numCaracteresLlave) {
            candidateKey = "0" + candidateKey;
        }

        if (tryDecryptDES(encryptedText, "" + candidateKey + "", decryptedText)) {
            // Verificar si la palabra clave está presente en el texto desencriptado
            std::cout << "Funciono con la llave: " << decryptedText.find(keyword) << std::endl;
            if (decryptedText.find(keyword) != std::string::npos) {
                std::cout << "La llave se encontro." << std::endl;
                keyFound = true;
                foundKey = candidateKey;
                break;
            }
        }

        // Restablecer decryptedText para el próximo intento
        decryptedText.clear();
    }

    if (keyFound) {
        std::cout << "Llave encontrada: " << foundKey << std::endl;
        std::cout << "Texto descifrado: " << decryptedText << std::endl;
    } else {
        std::cout << "La llave no se encontró o la palabra clave no está en el texto descifrado." << std::endl;
    }

    return 0;
}
