string(TOLOWER ${TLS_LIB} use_tls_lib)

if (${use_tls_lib} STREQUAL "wolfssl")
    find_package(PkgConfig)
    pkg_check_modules(WOLFSSL wolfssl)
    if (WOLFSSL_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${WOLFSSL_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${WOLFSSL_LIBRARIES})
    endif()
elseif (NOT ${use_tls_lib} STREQUAL "no")
    find_package(OpenSSL QUIET)
    if (OPENSSL_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
    elseif (APPLE)
        target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/opt/openssl/include)
        target_link_libraries(${PROJECT_NAME} -lssl -lcrypto)
    endif()
endif()

