find_package(OpenSSL CONFIG REQUIRED)
find_package(azure-storage-blobs-cpp CONFIG REQUIRED)
find_package(azure-identity-cpp CONFIG REQUIRED)

add_executable(Main main.cpp)

target_link_libraries(Main PRIVATE OpenSSL::Crypto)
target_link_libraries(Main PRIVATE Azure::azure-storage-blobs)
target_link_libraries(Main PRIVATE Azure::azure-identity)