# Install script for directory: /private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/private/tmp/.espressif/tools/xtensa-esp-elf/esp-15.2.0_20251204/xtensa-esp-elf/bin/xtensa-esp32-elf-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_adjust_auto_enabled.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_adjust_config_dependencies.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_adjust_config_derived.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_adjust_config_key_pair_types.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_adjust_config_synonyms.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_builtin_composites.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_builtin_key_derivation.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_builtin_primitives.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_compat.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_config.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_driver_common.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_composites.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_key_derivation.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_primitives.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_driver_random.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_extra.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_platform.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_sizes.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_struct.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_types.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/psa/crypto_values.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tf-psa-crypto" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/tf-psa-crypto/build_info.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/tf-psa-crypto/version.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/../drivers/builtin/include/mbedtls/config_adjust_legacy_crypto.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/../drivers/builtin/include/mbedtls/private_access.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/asn1.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/asn1write.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/base64.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/compat-3-crypto.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/constant_time.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/lms.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/md.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/memory_buffer_alloc.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/nist_kw.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/pem.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/pk.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/platform.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/platform_time.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/platform_util.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/psa_util.h"
    "/private/tmp/esp-idf-v6.0/components/mbedtls/mbedtls/tf-psa-crypto/include/mbedtls/threading.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/avi/Documents/work/quantum-silicon-photonics/multi-channel-voltage-source-fw/build-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
