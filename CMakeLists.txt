cmake_minimum_required(VERSION 3.12)

project(Wren VERSION 0.4.0 LANGUAGES C)
set(C_STANDARD 99)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

option(BUILD_OPTIONAL_META "Build the optional meta library" ON)
option(BUILD_OPTIONAL_RAND "Build the optional rand library" ON)
option(BUILD_WREN_NAN_TAGGING "Build the VM with NaN tagging" OFF)
option(BUILD_WREN_COMPUTED_GOTO "Build the VM with computed GOTO" ON)

set(SRC_DIR src)
set(INC_SRC_DIR ${SRC_DIR}/include/)
set(VM_SRC_DIR ${SRC_DIR}/vm)
set(VM_SRC_FILENAME ${VM_SRC_DIR}/wren_)
set(OPT_SRC_DIR ${SRC_DIR}/optional)
set(OPT_SRC_FILENAME ${OPT_SRC_DIR}/wren_opt_)

add_library(WrenVM STATIC)
target_include_directories(WrenVM PUBLIC ${INC_SRC_DIR})
target_sources(WrenVM PUBLIC
        ${INC_SRC_DIR}wren.h
        ${VM_SRC_FILENAME}common.h
        )
target_include_directories(WrenVM PRIVATE ${VM_SRC_DIR})
target_sources(WrenVM PRIVATE
        ${VM_SRC_FILENAME}compiler.h
        ${VM_SRC_FILENAME}compiler.c
        ${VM_SRC_FILENAME}core.h
        ${VM_SRC_FILENAME}core.c
        ${VM_SRC_FILENAME}debug.h
        ${VM_SRC_FILENAME}debug.c
        ${VM_SRC_FILENAME}math.h
        ${VM_SRC_FILENAME}opcodes.h
        ${VM_SRC_FILENAME}primitive.h
        ${VM_SRC_FILENAME}primitive.c
        ${VM_SRC_FILENAME}utils.h
        ${VM_SRC_FILENAME}utils.c
        ${VM_SRC_FILENAME}value.h
        ${VM_SRC_FILENAME}value.c
        ${VM_SRC_FILENAME}vm.h
        ${VM_SRC_FILENAME}vm.c)

if (BUILD_OPTIONAL_META OR BUILD_OPTIONAL_RAND)
    target_include_directories(WrenVM PRIVATE ${OPT_SRC_DIR})
endif ()

add_compile_definitions(WREN_OPT_META=${BUILD_OPTIONAL_META})
if (BUILD_OPTIONAL_META)
        target_sources(WrenVM PUBLIC
                ${OPT_SRC_FILENAME}meta.h
        )
        target_sources(WrenVM PRIVATE
                ${OPT_SRC_FILENAME}meta.c
        )
endif ()
add_compile_definitions(WREN_OPT_RANDOM=${BUILD_OPTIONAL_RAND})
if (BUILD_OPTIONAL_RAND)                  
        target_sources(WrenVM PUBLIC
                ${OPT_SRC_FILENAME}random.h
        )
    target_sources(WrenVM PRIVATE
                ${OPT_SRC_FILENAME}random.c
        )
endif ()
add_compile_definitions(WREN_NAN_TAGGING=${BUILD_WREN_NAN_TAGGING})
add_compile_definitions(WREN_COMPUTED_GOTO=${BUILD_WREN_COMPUTED_GOTO})
target_link_libraries(WrenVM m)
