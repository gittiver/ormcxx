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
option(ORMCXX_ENABLE_POSTGRES "enable postgres driver" ON)

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

if(${ORMCXX_ENABLE_POSTGRES})
    if (APPLE)
        set(PostgreSQL_LIBRARY /opt/local/lib/postgresql17/libpq.dylib)
        set(PostgreSQL_INCLUDE_DIR /opt/local/include/postgresql17/)
    endif ()
    find_package(PostgreSQL REQUIRED)
    add_library(${PROJECT_NAME}_pq
            src/lib/ormcxx_pq.hpp src/lib/ormcxx_pq.cpp
            )
    target_include_directories(
            ${PROJECT_NAME}_pq
            PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/include)
    target_link_libraries(${PROJECT_NAME}_pq
            PUBLIC tl::expected
            PRIVATE PostgreSQL::PostgreSQL)
endif()

# lib
add_library(${PROJECT_NAME}
        src/include/ormcxx/ormcxx_sql_config.hpp src/lib/ormcxx_sql_config.cpp
        src/lib/ormcxx_table.cpp src/include/ormcxx/ormcxx_table.hpp
        src/lib/ormcxx_sqlite.hpp src/lib/ormcxx_sqlite.cpp
        src/lib/ormcxx_db.cpp
        src/include/ormcxx/data_source.hpp)

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
    VERSION 3.10.0
    GITHUB_REPOSITORY catchorg/Catch2
    OPTIONS
    "CATCH_INSTALL_DOCS Off"
    "CATCH_INSTALL_EXTRAS Off"
    "CATCH_ENABLE_REPRODUCIBLE_BUILD Off"
  )
  add_subdirectory(test)

endif()
