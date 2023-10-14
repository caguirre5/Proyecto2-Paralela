/**
 *
 * authors:
 *  - Marco Jurado
 *  - Cristian Aguirre
 *  - Paola de Leon

*/


#include <iostream>
#include <fstream>
#include <cryptopp/des.h>       //DEpendencia para aplicar DES
#include <cryptopp/modes.h>     //DEpendencia para aplicar DES
#include <cryptopp/filters.h>   //DEpendencia para aplicar DES
#include <cryptopp/hex.h>       //DEpendencia para aplicar DES
#include <typeinfo>
#include <chrono> //Dependencia para tomar tiempo

using namespace CryptoPP;

// Función para descifrar un mensaje con DES
bool tryDecryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
    
    try {
        // Configurar el objeto de descifrado DES
        DES::Decryption desDecryption((byte*)key.data());
        ECB_Mode_ExternalCipher::Decryption ecbDecryption(desDecryption);

        // Realizar la operación de descifrado y almacenar el resultado en plaintext
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
    
    // Iniciar el temporizador
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int candidate = 0; candidate <= std::pow(10, numCaracteresLlave) - 1; candidate++) {
        // Convertir el número de candidato a una cadena de longitud numCaracteresLlave rellenada con ceros
        std::string candidateKey = std::to_string(candidate);
        while (candidateKey.length() < numCaracteresLlave) {
            candidateKey = "0" + candidateKey;
        }

        if (tryDecryptDES(encryptedText, "" + candidateKey + "", decryptedText)) {
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

    // Detener el temporizador
    auto endTime = std::chrono::high_resolution_clock::now();

    if (keyFound) {
        std::cout << "\nLlave encontrada: " << foundKey << std::endl;
        std::cout << "Texto descifrado: " << decryptedText << std::endl;
    } else {
        std::cout << "\nLa llave no se encontró o la palabra clave no está en el texto descifrado." << std::endl;
    }
    
    // Calcular la duración y mostrarla
    std::chrono::duration<double> duration = endTime - startTime;
    std::cout << "\nTiempo tomado para encontrar la llave: " << duration.count() << " segundos" << std::endl;

    return 0;
}
