set(CMAKE_CXX_STANDARD 11)

add_executable(nssg_toggkmc_test nssg_toggkmc_test.cpp)
target_link_libraries(nssg_toggkmc_test ${PROJECT_NAME})

add_executable(test_ssg_optimized_search test_ssg_optimized_search.cpp)
target_link_libraries(test_ssg_optimized_search ${PROJECT_NAME})

