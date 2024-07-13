cmake_minimum_required(VERSION 3.16)
project(obelisk)
set(VERSION 1.0.0)
include(CMakePackageConfigHelpers)
file(GLOB_RECURSE CPP_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE CPP_SOURCES source/*.cpp source/*.cc)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS url thread json fiber thread system)
find_package(fmt CONFIG REQUIRED)
find_package(sahara REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME} STATIC ${CPP_HEADERS} ${CPP_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE sahara::sahara Boost::json)

configure_package_config_file(cmake/template/obelisk.in.cmake
        ${PROJECT_NAME}-config.cmake
        INSTALL_DESTINATION ./)
write_basic_package_version_file(
        ${PROJECT_NAME}-config-version.cmake
        VERSION ${VERSION}
        COMPATIBILITY ExactVersion)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION share/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(EXPORT ${PROJECT_NAME}-targets NAMESPACE ${PROJECT_NAME}:: DESTINATION share/${PROJECT_NAME})
#target_link_libraries(obelisk PRIVATE )
#
#file(GLOB_RECURSE TEST_HEADERS test/*.h test/*.hpp)
#file(GLOB_RECURSE TEST_SOURCES test/*.cpp test/*.cc)

#
#find_package(CURL REQUIRED)
#find_package(OpenSSL REQUIRED)
#
#file(GLOB_RECURSE TEST_SOURCES test/*.h test/*.hpp test/*.cpp test/*.cc)
#add_executable(${PROJECT_NAME}_test ${TEST_SOURCES} )
#find_package(sahara REQUIRED)
#target_include_directories(${PROJECT_NAME}_test PRIVATE "C:/Users/Hackman.Lo/Downloads/Compressed/aliyun-oss-cpp-sdk-master/sdk/include")
#target_link_directories(${PROJECT_NAME}_test PRIVATE "C:/Users/Hackman.Lo/Downloads/Compressed/aliyun-oss-cpp-sdk-master/cmake-build-debug/lib")
#target_link_libraries(${PROJECT_NAME}_test PRIVATE alibabacloud-oss-cpp-sdk CURL::libcurl OpenSSL::SSL  OpenSSL::Crypto)
#target_include_directories(${PROJECT_NAME}_test PRIVATE include)
#target_link_libraries(${PROJECT_NAME}_test PRIVATE $<$<BOOL:${WIN32}>:ws2_32> $<$<BOOL:${WIN32}>:Mswsock> obelisk Boost::json Boost::locale Boost::cobalt Boost::thread Boost::system sahara::sahara)
#target_link_libraries(${PROJECT_NAME}_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
#target_link_libraries(${PROJECT_NAME}_test PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
#target_link_libraries(${PROJECT_NAME}_test PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

