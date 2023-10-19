/**
 *
 * authors:
 *  - Marco Jurado
 *  - Cristian Aguirre
 *  - Paola de Leon

*/

#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <typeinfo>
#include <chrono>
#include <mpi.h>

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

bool binarySearch(const std::string& ciphertext, const std::string& keyword, int numCaracteresLlave, long long startCandidate, long long endCandidate, std::string& foundKey, int processId) {    std::string decryptedText;
    int keysEvaluated = 0;  // Inicializa un contador de claves evaluadas a 0

    for (long long candidate = startCandidate; candidate < endCandidate; candidate++) {
        std::string candidateKey = std::to_string(candidate);
        while (candidateKey.length() < numCaracteresLlave) {
            candidateKey = "0" + candidateKey;
        }  
        
        if (tryDecryptDES(ciphertext, candidateKey, decryptedText)) {
            keysEvaluated++;  // Incrementa el contador de claves evaluadas
            if (decryptedText.find(keyword) != std::string::npos) {
                foundKey = candidateKey;
                return true;
            }
        }

        decryptedText.clear();
    }

    std::cout << "Proceso " << processId << " evaluó " << keysEvaluated << " claves." << std::endl;  // Imprime el número de claves evaluadas en este proceso
    return false;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <palabra_clave> <num_caracteres_llave>" << std::endl;
        return 1;
    }

    const std::string keyword = argv[1];
    int numCaracteresLlave = std::stoi(argv[2]);

    std::string decryptedText;

    std::ifstream encryptedFile("textoCifrado_alt1.txt");
    if (!encryptedFile) {
        std::cerr << "Error al abrir el archivo cifrado para descifrar." << std::endl;
        return 1;
    }

    std::string encryptedText((std::istreambuf_iterator<char>(encryptedFile)), std::istreambuf_iterator<char>());
    encryptedFile.close();

    MPI_Init(&argc, &argv);

    int numProcesses;
    int processId;

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    long long int totalCandidates = std::pow(10, numCaracteresLlave);
    long long int candidatesPerProcess = totalCandidates / numProcesses;

    long long int startCandidate = processId * candidatesPerProcess;
    long long int endCandidate = (processId + 1) * candidatesPerProcess;

    bool keyFound = false;
    std::string foundKey;

    auto startTime = std::chrono::high_resolution_clock::now();

    if (binarySearch(encryptedText, keyword, numCaracteresLlave, startCandidate, endCandidate, foundKey, processId)) {
        keyFound = true;
    }


    auto endTime = std::chrono::high_resolution_clock::now();
    

    if (keyFound) {
        std::cout << "Proceso " << processId << " muestra la llave: " << foundKey << std::endl;
        std::chrono::duration<double> duration = endTime - startTime;
        std::cout << "\nTiempo tomado para encontrar la llave: " << duration.count() << " segundos" << std::endl;
        MPI_Send(foundKey.c_str(), foundKey.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(nullptr, 0, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    if (processId == 0) {
        std::string finalFoundKey;
        bool anyKeyFound = false;

        for (int i = 0; i < numProcesses; i++) {
            char receivedKey[256];
            MPI_Recv(receivedKey, sizeof(receivedKey), MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (receivedKey[0] != '\0') {
                finalFoundKey = std::string(receivedKey);
                anyKeyFound = true;
            }
        }

        if (anyKeyFound) {
            std::cout << "\nLlave encontrada: " << finalFoundKey << std::endl;
        } else {
            std::cout << "\nLa llave no se encontró o la palabra clave no está en el texto descifrado." << std::endl;
        }

        std::chrono::duration<double> duration = endTime - startTime;
        std::cout << "\nTiempo tomado para encontrar la llave: " << duration.count() << " segundos" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
