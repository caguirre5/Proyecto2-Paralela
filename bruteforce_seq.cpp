#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

// Función para descifrar un mensaje con DES
bool tryDecryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
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
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <palabra_clave>" << std::endl;
        return 1;
    }

    const std::string keyword = argv[1]; // Obtener la palabra clave desde la línea de comandos

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

    for (int candidate = 0; candidate <= 99999999; candidate++) {
        // Convertir el número de candidato a una cadena de 8 dígitos
        std::string candidateKey = std::to_string(candidate);
        while (candidateKey.length() < 8) {
            candidateKey = "0" + candidateKey;
        }

        if (tryDecryptDES(encryptedText, candidateKey, decryptedText)) {
            // Verificar si la palabra clave está presente en el texto desencriptado
            if (decryptedText.find(keyword) != std::string::npos) {
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
