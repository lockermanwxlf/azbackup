connection_string="$(terraform output storage_account_connection_string)"
connection_string=${connection_string//;/%}
cmake --preset=vcpkg -Dconnection_string=$connection_string
cd build/
ninja