string(TOLOWER ${TLS_LIB} use_tls_lib)

if (${use_tls_lib} STREQUAL "wolfssl")
    add_compile_options(-D CO_USE_WOLFSSL)
elseif (${use_tls_lib} STREQUAL "openssl")
    add_compile_options(-D CO_USE_OPENSSL)
elseif (${use_tls_lib} STREQUAL "no")
    add_compile_options(-D CO_NO_TLS)
endif()

