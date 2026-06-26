cmake_minimum_required(VERSION 3.15)
project(ig LANGUAGES CXX)

# ----------------- configuración C++ -----------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -DNDEBUG")

# ----------------- rutas de artefactos -----------------
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Carpeta raíz de outputs: ./output/FECHA_HORA/...
string(TIMESTAMP NOW "%H_%M_%S-%Y_%m_%d")
set(OUTPUT_BASE "${CMAKE_SOURCE_DIR}/output")
set(TEST_OUTPUT_ROOT "${OUTPUT_BASE}/${NOW}")
message(STATUS "Tests: los resultados irán a ${TEST_OUTPUT_ROOT}")

# ---------------------- packages -----------------------

find_package(PNG REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(Imath REQUIRED)
# --------------------- librerías -----------------------

# función para crear librerías tipo "geometry", etc.
function(add_project_library NAME)
    add_library(${NAME})
    target_include_directories(${NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    file(GLOB LIB_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/${NAME}/*.hh"
        "${CMAKE_SOURCE_DIR}/src/${NAME}/*.cc")
    target_sources(${NAME} PRIVATE ${LIB_FILES})
    if(ARGN)
        target_link_libraries(${NAME} PUBLIC ${ARGN})
    endif()
endfunction()

# definición de bibliotecas
add_project_library(geometry)
add_project_library(imaging PNG::PNG OpenEXR::OpenEXR Imath::Imath)
add_project_library(sensor geometry imaging Threads::Threads)

# ---------------------- tests -----------------------

include(CTest)
enable_testing()

# función que crea carpeta, ejecuta test y opcionalmente guarda log
function(add_wrapped_test TESTNAME OUT_SUBDIR BIN_PATH)
    set(options LOG)
    set(oneValueArgs)
    set(multiValueArgs BIN_ARGS)
    cmake_parse_arguments(PARSE_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DIR "${TEST_OUTPUT_ROOT}/${OUT_SUBDIR}")
    string(REPLACE ";" " " ARGS_STR "${PARSE_ARGS_BIN_ARGS}")

    if(PARSE_ARGS_LOG)
        add_test(NAME ${TESTNAME}
            COMMAND /bin/sh -c "mkdir -p '${TEST_OUTPUT_ROOT}' '${DIR}' && cd '${DIR}' && ('${BIN_PATH}' ${ARGS_STR}) 2>&1 | tee '${DIR}/${TESTNAME}.log'")
    else()
        add_test(NAME ${TESTNAME}
            COMMAND /bin/sh -c "mkdir -p '${TEST_OUTPUT_ROOT}' '${DIR}' && cd '${DIR}' && '${BIN_PATH}' ${ARGS_STR}")
    endif()
endfunction()

# función para compilar ejecutables de test desde un directorio
function(add_tests_from_dir DIR_NAME)
    file(GLOB TEST_FILES CONFIGURE_DEPENDS "tests/${DIR_NAME}/*.cc")

    foreach(test_file ${TEST_FILES})
        get_filename_component(base ${test_file} NAME_WE)

        # ---------- binario interactivo ----------
        set(int_name ${base}_interactive)
        add_executable(${int_name} ${test_file})
        target_link_libraries(${int_name} PRIVATE ${ARGN})
        set_target_properties(${int_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/interactivo"
            OUTPUT_NAME ${base}
        )

        # ---------- binario de test (ctest) ----------
        set(test_name ${base}_test)
        add_executable(${test_name} ${test_file})
        target_link_libraries(${test_name} PRIVATE ${ARGN})
        target_compile_definitions(${test_name} PRIVATE RUNNING_AS_TEST=1)
        set_target_properties(${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
            OUTPUT_NAME ${base}
        )

    endforeach()
endfunction()

add_tests_from_dir(imaging imaging)

# tests imaging
add_wrapped_test(test_png "imaging/png"
    "${CMAKE_BINARY_DIR}/bin/test/test_png")

add_wrapped_test(test_tonemapping "imaging/tonemapping"
    "${CMAKE_BINARY_DIR}/bin/test/test_tonemapping"
    BIN_ARGS "${CMAKE_SOURCE_DIR}/data/images/example_landscape.exr")

set(EXR_ARG "${CMAKE_SOURCE_DIR}/data/images/example_landscape.exr")
add_wrapped_test(test_exr "imaging/exr"
    "${CMAKE_BINARY_DIR}/bin/test/test_exr" LOG BIN_ARGS "${EXR_ARG}")


# ------------------- otros targets --------------------

add_custom_target(tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(pristine
  COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
  COMMENT "Full cleanup of build directory"
)

add_custom_target(clean-tests
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${TEST_OUTPUT_ROOT}"
  COMMENT "Eliminar outputs de tests (${TEST_OUTPUT_ROOT})"
)
