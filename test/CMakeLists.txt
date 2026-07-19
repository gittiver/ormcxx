cmake_minimum_required(VERSION 3.21)
project(ormcxx_test)

set(CMAKE_POLICY_DEFAULT_CMP0077 new)
set(CMAKE_POLICY_WARNING_CMP0126 new)
  
enable_testing()

set(TEST_SRCS
        sql_config_test.cpp
        db_test.cpp
)

add_executable(unittest ${TEST_SRCS})
target_link_libraries(unittest ormcxx ormcxx_pq Catch2::Catch2WithMain)

add_test(NAME unittest COMMAND unittest)
