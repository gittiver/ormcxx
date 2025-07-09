cmake_minimum_required(VERSION 3.21)

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif ()

if (POLICY CMP0126)
    cmake_policy(SET CMP0126 NEW)
endif ()

project(ormcxx VERSION 0.0.1 LANGUAGES CXX)

enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

option(ORMCXX_ENABLE_TESTS "enable testing" ON)

# add tartan lamas expected library
CPMAddPackage(expected
  VERSION 1.1.0
  GITHUB_REPOSITORY TartanLlama/expected
  OPTIONS
  "EXPECTED_BUILD_TESTS Off"
)

CPMAddPackage(sqlite3
GIT_TAG master
GITHUB_REPOSITORY gittiver/sqlite3-amalgamation
)

# lib
add_library(${PROJECT_NAME}
        src/include/ormcxx/ormcxx_sql_config.hpp src/lib/ormcxx_sql_config.cpp
        src/lib/ormcxx_table.cpp src/include/ormcxx/ormcxx_table.hpp
        src/lib/ormcxx_sqlite.hpp src/lib/ormcxx_sqlite.cpp
        src/lib/ormcxx_db.cpp)
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/include)
target_link_libraries(${PROJECT_NAME} PUBLIC tl::expected sqlite3)

# main executable
add_executable(${PROJECT_NAME}_main src/main.cpp)
target_link_libraries(${PROJECT_NAME}_main
        PRIVATE
        ${PROJECT_NAME}
)
target_include_directories(${PROJECT_NAME}_main PRIVATE ${expected_SOURCE_DIR}/include)

add_executable(db_access src/examples/db_access.cpp)
target_link_libraries(db_access
        PRIVATE
        ${PROJECT_NAME}
)

include(CTest)
if(${ORMCXX_ENABLE_TESTS})

  enable_testing()

  CPMAddPackage(Catch2
    VERSION 3.8.1
    GITHUB_REPOSITORY catchorg/Catch2
    OPTIONS
    "CATCH_INSTALL_DOCS Off"
    "CATCH_INSTALL_EXTRAS Off"
    "CATCH_ENABLE_REPRODUCIBLE_BUILD Off"
  )
  add_subdirectory(test)

endif()
