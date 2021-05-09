set(CMAKE_CXX_STANDARD 11)

add_executable(nsg_toggkmc_test nsg_toggkmc_test.cpp)
target_link_libraries(nsg_toggkmc_test ${PROJECT_NAME} -ltcmalloc)

add_executable(test_nsg_search test_nsg_search.cpp)
target_link_libraries(test_nsg_search ${PROJECT_NAME})

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_link_libraries(test_nsg_optimized_search ${PROJECT_NAME} -ltcmalloc)

