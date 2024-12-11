#include <iostream>
#include <azure/core.hpp>
#include <azure/identity/default_azure_credential.hpp>
#include <azure/storage/blobs.hpp>
#include <string>
#include <openssl/aes.h>
#include <openssl/core_names.h>
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <openssl/thread.h>

void decode_connection_string(std::string& connection_string) {
    auto find = connection_string.find('%');
    while (find != std::string::npos) {
        connection_string[find] = ';';
        find = connection_string.find('%', find);
    }
}

void hash_password(const std::string& password) {
    uint32_t lanes = 2, threads = 2, memcost = 65536;
    size_t outlen = 256;
    unsigned char result[outlen];

    unsigned char pwd[password.size()], salt[] = "G{a@{d0VLjiy96;RVc]f";
    password.copy((char*)pwd, password.size() + 1);

    OSSL_PARAM params[6], *p = params;
    EVP_KDF *kdf = NULL;
    EVP_KDF_CTX *kctx = NULL;

    if (OSSL_set_max_threads(NULL, threads) != 1)
        goto fail;

    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threads);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES, &lanes);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST, &memcost);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, salt, strlen((const char *)salt));
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, pwd, strlen((const char *)pwd));
    *p++ = OSSL_PARAM_construct_end();


    if ((kdf = EVP_KDF_fetch(NULL, "ARGON2D", NULL)) == NULL)
        goto fail;
    if ((kctx = EVP_KDF_CTX_new(kdf)) == NULL)
        goto fail;
    if (EVP_KDF_derive(kctx, &result[0], outlen, params) != 1)
        goto fail;
    std::cout << "Output: " << OPENSSL_buf2hexstr(result, outlen) << std::endl;

fail:
    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(kctx);
    OSSL_set_max_threads(NULL, 0);
    std::cout << "FUUUUUUUUUUCCCCCCKKKKKK" << std::endl;
}

int main() {
    using namespace Azure::Storage::Blobs;

    std::string input;
    while (std::cin >> input) {
        hash_password(input);
    }

    return EXIT_SUCCESS;

    // Get connection string.
    std::string connection_string { CONNECTION_STRING_ENCODED };
    decode_connection_string(connection_string);
    
    auto blob_service_client {BlobServiceClient::CreateFromConnectionString(connection_string)};
    auto container_client {blob_service_client.GetBlobContainerClient("testblobcontainer")};
    container_client.CreateIfNotExists();

    std::string name {"testfile.txt"};
    uint8_t content[] {"Hello, World!"}; 
    auto blob_client {container_client.GetBlockBlobClient(name)};
    auto response = blob_client.UploadFrom(content, sizeof(content));
    std::cout << "finished" << std::endl;


    

    return EXIT_SUCCESS;
}