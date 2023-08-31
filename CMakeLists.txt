# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0.0)

# 项目信息
project (Json_Parser)

#头文件搜索目录
include_directories(${PROJECT_SOURCE_DIR}/src)

#src目录下的源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)
#test目录下文件
aux_source_directory(${PROJECT_SOURCE_DIR}/test TEST_SRCS)

#启用测试
include(CTest)
enable_testing()
#寻找GTest和Threads库
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# add_executable(test_token ${TEST_SRCS})
# target_link_libraries(test_token gtest ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_executable(test_parser ${TEST_SRCS} ${DIR_SRCS})
# link_directories(${DIR_SRCS})
target_link_libraries(test_parser gtest ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
gtest_discover_tests(test_parser)
# add_test(NAME test_add COMMAND test_add)

# 指定生成目标
# add_executable(test ${DIR_SRCS} ${TEST_SRCS})
