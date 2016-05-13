cmake_minimum_required(VERSION 2.6)
project(LibParIO)

#set(LIBPARIO_INSTALL "" CACHE STRING "MemTool installatoin directory")

#set(LIBPARIO_INCLUDE "${LIBPARIO_INSTALL}/include")
#set(LIBPARIO_LIB "${LIBPARIO_INSTALL}/lib")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCODE_MP -DOMPI_SKIP_MPICXX")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -DQDEBUG")
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNQDEBUG -DNDEBUG -qarch=qp -qtune=qp")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNQDEBUG -DNDEBUG")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Set a default build type if none was specified
#f(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
# message(STATUS "Setting build type to 'Release' as none was specified.")
# set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE#
# set(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
#   "MinSizeRel" "RelWithDebInfo")
#ndif()

set(LIBPARIO_SOURCES 
        buffer.cpp
        libpario.cpp
)

#set(LIBPARIO_API
#	api/DataPool.cpp
#        api/MLM_DataPool.cpp
#)

include_directories(${CMAKE_SOURCE_DIR})

#SET_TARGET_PROPERTIES(LibParIO -lpthread)

#link_directories(${..._LIB})

add_library(libpario ${LIBPARIO_SOURCES})
#set(/bgsys/drivers/ppcfloor/spi/lib/ libSPI_l1p)
#target_link_libraries(MemTool l1plib)

#add_executable(memtool_prop_alloc test/prop_alloc/main.cpp ${LIBPARIO_SOURCES})
#add_executable(memtool_data_access test/data_access/main.cpp ${LIBPARIO_SOURCES})
add_executable(LibParIO main.cpp ${LIBPARIO_SOURCES})
#target_link_libraries(memtool_data_access /bgsys/drivers/ppcfloor/spi/lib/libSPI_l1p.a)

#install(TARGETS MemTool LIBRARY DESTINATION ${LIBPARIO_LIB} ARCHIVE DESTINATION ${LIBPARIO_LIB})
#install(FILES src/include/section.h DESTINATION ${LIBPARIO_INCLUDE})

