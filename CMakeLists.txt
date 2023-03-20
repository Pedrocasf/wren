cmake_minimum_required(VERSION 3.12)

project(Wren VERSION 0.4.0 LANGUAGES C)
set(C_STANDARD 99)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

option(BUILD_OPTIONAL_META "Build the optional meta library" ON)
option(BUILD_OPTIONAL_RAND "Build the optional rand library" OFF)

set(SRC_FOLDER src)
set(INC_SRC_FOLDER ${SRC_FOLDER}/include/)
set(VM_SRC_FOLDER ${SRC_FOLDER}/vm)
set(VM_SRC_FILENAME ${VM_SRC_FOLDER}/wren_)
set(OPT_SRC_FOLDER ${SRC_FOLDER}/optional)
set(OPT_SRC_FILENAME ${OPT_SRC_FOLDER}/wren_opt_)

add_library(WrenVM STATIC)
target_include_directories(WrenVM PUBLIC ${INC_SRC_FOLDER})
target_sources(WrenVM PUBLIC
        ${INC_SRC_FOLDER}wren.h
        ${VM_SRC_FILENAME}common.h
        )
target_include_directories(WrenVM PUBLIC ${VM_SRC_FOLDER})
target_sources(WrenVM PRIVATE
        ${VM_SRC_FILENAME}common.h
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
    target_include_directories(WrenVM PUBLIC ${OPT_SRC_FOLDER})
endif ()

if (BUILD_OPTIONAL_META)
        add_compile_definitions(WREN_OPT_META=1)
        target_sources(WrenVM PUBLIC
                ${OPT_SRC_FILENAME}meta.h
        )
        target_sources(WrenVM PRIVATE
                ${OPT_SRC_FILENAME}meta.c
        )
else()
        add_compile_definitions(WREN_OPT_META=0) 
endif ()
if (BUILD_OPTIONAL_RAND)
        add_compile_definitions(WREN_OPT_RAND=1)                   
        target_sources(WrenVM PUBLIC
                ${OPT_SRC_FILENAME}random.h
        )
    target_sources(WrenVM PRIVATE
                ${OPT_SRC_FILENAME}random.c
        )
else()
        add_compile_definitions(WREN_OPT_RAND=0) 
endif ()

target_link_libraries(WrenVM m)
