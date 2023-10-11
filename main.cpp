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

// Función para descifrar un mensaje con DES usando fuerza bruta
void bruteForceDecryptDES(const std::string& ciphertext) {
    std::string key(8, ' ');  // DES utiliza una llave de 8 bytes
    std::string plaintext;

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            // ... (continúa con bucles anidados para cubrir todos los bytes de la llave)
            key[0] = i;
            key[1] = j;
            // ...

            decryptDES(ciphertext, key, plaintext);

            // Aquí puedes verificar si el texto descifrado tiene sentido o usar alguna otra heurística
            // Por simplicidad, solo mostraré el texto descifrado para cada llave
            std::cout << "Key: " << key << ", Plaintext: " << plaintext << std::endl;
        }
    }
}

int main() {
    std::string key = "12345678";  // Asegúrate de que la llave tenga 8 caracteres
    std::string plaintext, ciphertext;

    // Leer el mensaje desde un archivo
    std::ifstream file("message.txt");
    if (file) {
        plaintext.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    } else {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 1;
    }

    encryptDES(plaintext, key, ciphertext);
    std::cout << "Texto cifrado: " << ciphertext << std::endl;

    std::string decryptedText;
    decryptDES(ciphertext, key, decryptedText);
    std::cout << "Texto descifrado: " << decryptedText << std::endl;

    // Para demostrar la fuerza bruta (esto puede llevar MUCHO tiempo)
    // bruteForceDecryptDES(ciphertext);

    return 0;
}
