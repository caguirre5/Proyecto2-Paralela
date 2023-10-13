#include <iostream>
#include <fstream>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <mpi.h>

using namespace CryptoPP;

// Función para descifrar un mensaje con DES
bool tryDecryptDES(const std::string& ciphertext, const std::string& key, std::string& plaintext) {
    // Implementa la función tryDecryptDES aquí
    // ...
    return false;  // Reemplaza esto con la implementación real
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <palabra_clave>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    const std::string keyword = argv[1]; // Obtener la palabra clave desde la línea de comandos
    const int total_keys = 99999999;

    // Calcular el rango de llaves para este nodo
    const int keys_per_node = total_keys / size;
    const int start_key = rank * keys_per_node;
    const int end_key = (rank == size - 1) ? total_keys : (rank + 1) * keys_per_node;

    std::string decryptedText;

    // Leer el texto cifrado desde un archivo (solo el nodo 0 lo hace)
    std::string encryptedText;
    if (rank == 0) {
        std::ifstream encryptedFile("textoCifrado.txt");
        if (!encryptedFile) {
            std::cerr << "Error al abrir el archivo cifrado para descifrar." << std::endl;
            MPI_Finalize();
            return 1;
        }
        encryptedText.assign((std::istreambuf_iterator<char>(encryptedFile)), std::istreambuf_iterator<char>());
    }

    bool keyFound = false;
    std::string foundKey;

    for (int candidate = start_key; candidate < end_key && !keyFound; candidate++) {
        std::string candidateKey = std::to_string(candidate);
        while (candidateKey.length() < 8) {
            candidateKey = "0" + candidateKey;
        }

        if (tryDecryptDES(encryptedText, candidateKey, decryptedText)) {
            if (decryptedText.find(keyword) != std::string::npos) {
                keyFound = true;
                foundKey = candidateKey;
            }
        }
    }

    // Comunicación entre nodos para detener la búsqueda si se encuentra la clave
    MPI_Bcast(&keyFound, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    if (keyFound) {
        if (rank == 0) {
            std::cout << "Llave encontrada: " << foundKey << std::endl;
            std::cout << "Texto descifrado: " << decryptedText << std::endl;
        }
    } else {
        if (rank == 0) {
            std::cout << "La llave no se encontró o la palabra clave no está en el texto descifrado." << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
