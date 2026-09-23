cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "VitaSDK toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your VitaSDK installation.")
    endif()
endif()

project(VitaFlightRadar LANGUAGES C)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Vita Flight Radar")
set(VITA_TITLEID "VFLR00001")
set(VITA_VERSION "01.00")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-O2 -Wall -Wextra -Wno-unused-parameter)
add_link_options(-Wl,-q)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.c
    src/app_config.c
    src/flight.c
    src/net.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)


target_link_libraries(${PROJECT_NAME}
    vita2d
    jansson
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    freetype
    png
    jpeg
    z
    m
    c
    SceDisplay_stub
    SceGxm_stub
    SceSysmodule_stub
    SceCtrl_stub
    SceTouch_stub
    ScePgf_stub
    ScePvf_stub
    SceCommonDialog_stub
    SceAppMgr_stub
    SceNet_stub
    SceNetCtl_stub
    SceHttp_stub
    SceSsl_stub
    SceIofilemgr_stub
    SceLibKernel_stub
    SceProcessmgr_stub
    SceKernelThreadMgr_stub
)

vita_create_self(eboot.bin ${PROJECT_NAME})

vita_create_vpk(VitaFlightRadar.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE sce_sys/icon0.png sce_sys/icon0.png
    FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
    FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
