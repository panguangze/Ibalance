cmake_minimum_required(VERSION 3.17)
project(Ibalance)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXX_FLAGS} -O0 -Wall -ggdb -fkeep-inline-functions")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 ")

aux_source_directory(./src  LOCAL_HAP_SRC)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(Ibalance main.cpp ${LOCAL_HAP_SRC})
target_link_libraries(Ibalance -lCbcSolver -lCbc -lOsi -lOsiCbc -lCgl -lOsiClp -lClp -lCoinUtils -llapack -lbz2 -lz)
install (TARGETS Ibalance DESTINATION bin)
