cmake_minimum_required (VERSION 3.25)

project ("ExampleApp")

set(APP_NAME "ExampleApp")
set(DEVELOPER_NAME "DevName")
set(RAM "500" CACHE STRING "Ram requirement (in kb)")
set(IMSI "9123412341234" CACHE STRING "Imsi of first SIM-card")
set(API "File SIM card ProMng" CACHE STRING "Api for app")
option(BACKGROUND "App can work background" OFF)

set(MRE_SDK "$ENV{MRE_SDK}" CACHE STRING "MRE SDK dir")
set(TinyMRESDK "$ENV{TinyMRESDK}" CACHE STRING "TinyMRESDK dir")
set(VXP_DEV_SIGN "$ENV{VXP_DEV_SIGN}" CACHE STRING "VXP Dev Sign executable")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BACKGROUND)
    add_definitions(-DSUPPORT_BG)
    set(TB 1)
else()
    set(TB 0)
endif()

# === Compliation Targets ===

if(${CMAKE_SYSTEM_NAME} STREQUAL Generic)  # Phone target
    add_executable(${APP_NAME}
        platform/gccmain.c
        platform/syscalls.c
        src/main.cpp
    )

    # Link scatter file (scat.ld) for correct memory layout
    target_link_options(${APP_NAME} PRIVATE "-T" "${CMAKE_SOURCE_DIR}/cmake/scat.ld")

    set_target_properties(${APP_NAME} PROPERTIES SUFFIX ".axf")

    # === MRE API Linking ===
    target_link_libraries(${APP_NAME} -L"${MRE_SDK}/lib/MRE30/armgcc/" 
        -l:percommon.a -l:pertcp.a -l:persensor.a -l:persmsper.a -l:perbitstream.a
        -l:percontact.a -l:permms.a -l:persmsmng.a -l:peraudio.a -l:percallout.a
        -l:percam.a -l:perfile.a -l:perhttp.a -l:persim.a -l:perrecord.a
        -l:persmssp.a -l:perlbs.a -l:perpro.a -l:pervideo.a -l:perxml.a
        -l:persec.a -l:persysstorage.a -l:perpayment.a -l:perbt.a -l:perpn.a
        -l:perudp.a -l:persysfile.a -l:persso.a
    )

    set(VXP_SUFFIX  ".vxp")
elseif(WIN32)  # MoDis target (Windows)
    add_library(${APP_NAME} SHARED
        src/main.cpp
    )

    # Use dll.def for exported symbols
    set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS "/DEF:${CMAKE_SOURCE_DIR}/cmake/dll.def")

    # Link MRE SDK
    target_link_libraries(${APP_NAME} "${MRE_SDK}/lib/mrewin32.lib")

    set(VXP_SUFFIX ".vc.vxp")
else()
    message(FATAL_ERROR "Unsupported target system")
endif()

target_include_directories(${APP_NAME} PRIVATE "${MRE_SDK}/include")

# === Resource Handling ===
file(GLOB RESOURCE_FILES resources/*)

# Resource pack target
add_custom_target(resources ALL
    COMMAND "${TinyMRESDK}/bin/PackRes" 
        -o "${CMAKE_BINARY_DIR}/${APP_NAME}.res"
        -f ${RESOURCE_FILES}
        --empty-logo
    DEPENDS ${RESOURCE_FILES}
)

# === VXP Packaging ===
add_custom_target(vxp ALL
    COMMAND "${TinyMRESDK}/bin/PackApp"
        -a "$<TARGET_FILE:${APP_NAME}>"
        -r "${CMAKE_BINARY_DIR}/${APP_NAME}.res"
        -o "${CMAKE_BINARY_DIR}/${APP_NAME}${VXP_SUFFIX}"
        -tr "${RAM}" 
        -tn "${APP_NAME}"
        -tdn "${DEVELOPER_NAME}"
        -tb "${TB}"
        -ti "${IMSI}"
        -tapi "${API}"
    DEPENDS ${APP_NAME} resources
)


# === Automatic VXP Signing (Only if target is Phone) ===
if(${CMAKE_SYSTEM_NAME} STREQUAL Generic)
    add_custom_command(TARGET vxp POST_BUILD
        COMMAND "${VXP_DEV_SIGN}" 
                "${CMAKE_BINARY_DIR}/${APP_NAME}.vxp" "${IMSI}" "-"
        COMMENT "Signing VXP file in-place with IMSI: ${IMSI}"
    )
endif()

# === MoDis Execution (Windows only) ===
if (WIN32)
    set(MODIS_PATH "${MRE_SDK}/models/Model01_QVGA_MRE3.0/")

    add_custom_target(run_in_MoDis ALL
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${APP_NAME}.vc.vxp" 
            "${MODIS_PATH}/WIN32FS/DRIVE_E/mre/${APP_NAME}.vc.vxp"
        COMMAND echo "${MODIS_PATH}/MoDIS/Release/MoDIS.exe" > "${CMAKE_BINARY_DIR}/run_in_MoDis.bat"
        DEPENDS vxp
    )
endif()
