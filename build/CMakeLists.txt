cmake_minimum_required(VERSION 3.10)
project(jarkov-chain)

# groups ../src/*.cpp and ../main.cpp into one variable called SOURCES, these are the files that will actually be compiled
file(GLOB SOURCES
    ../src/*
    ../main.cpp
)

set(HEADERS ../include)

# tells the compiler to look for libraries inside ../include
include_directories(${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES})
# add_executable(server ${SOURCES})
# add_executable(client ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS})