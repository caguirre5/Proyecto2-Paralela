#include <iostream>
#include <cstring>
#include <mpi.h>
#include <cryptopp/des.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>

void decrypt(const CryptoPP::byte *key, const CryptoPP::byte *ciphertext, int len, CryptoPP::byte *plaintext) {
    CryptoPP::DES::Decryption des;
    des.SetKey(key, 8); // Clave de 8 bytes
    CryptoPP::ECB_Mode_ExternalCipher::Decryption ecbDecryption(des);
    ecbDecryption.ProcessData(plaintext, ciphertext, len);
}

void encrypt(const CryptoPP::byte *key, const CryptoPP::byte *plaintext, int len, CryptoPP::byte *ciphertext) {
    CryptoPP::DES::Encryption des;
    des.SetKey(key, 8); // Clave de 8 bytes
    CryptoPP::ECB_Mode_ExternalCipher::Encryption ecbEncryption(des);
    ecbEncryption.ProcessData(ciphertext, plaintext, len);
}

const char *search = "hello";

int tryKey(const CryptoPP::byte *key, const CryptoPP::byte *ciphertext, int len) {
    CryptoPP::byte temp[len];
    memcpy(temp, ciphertext, len);
    decrypt(key, temp, len, temp);
    return strstr(reinterpret_cast<char *>(temp), search) != nullptr;
}

int main(int argc, char *argv[]) {
    int N, id;
    uint64_t upper = 0x100; // Upper bound para claves de 8 bytes
    uint64_t mylower, myupper;
    MPI_Status st;
    MPI_Request req;
    int flag;

    CryptoPP::byte ciphertext[] = "hello from the ubuntu";
    int ciphlen = strlen((char *)ciphertext);

    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &N);
    MPI_Comm_rank(comm, &id);
    int range_per_node = upper / N;
    mylower = range_per_node * id;
    myupper = range_per_node * (id + 1) - 1;
    if (id == N - 1) {
        // Compensar por el residuo
        myupper = upper;
    }

    uint64_t found = 0;
    MPI_Irecv(&found, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);
    
    for (uint64_t i = mylower; i <= myupper && (found == 0); ++i) {
        if (tryKey(reinterpret_cast<const CryptoPP::byte *>(&i), ciphertext, ciphlen)) {
            found = i;
            for (int node = 0; node < N; node++) {
                MPI_Send(&found, 1, MPI_LONG_LONG, node, 0, MPI_COMM_WORLD);
            }
            break;
        }
    }
    
    if (id == 0) {
        MPI_Wait(&req, &st);
        CryptoPP::byte plaintext[ciphlen];
        decrypt(reinterpret_cast<const CryptoPP::byte *>(&found), ciphertext, ciphlen, plaintext);
        std::cout << found << " " << plaintext << std::endl;
    }
    
    MPI_Finalize();
}
