cmake_minimum_required(VERSION 3.10)
project(jarkov-chain)

# groups ../src/*.cpp and ../main.cpp into one variable called SOURCES, these are the files that will actually be compiled
file(GLOB SOURCES
    ../src/*
    ../main.cpp
)

# can probably be organised better
file(GLOB TESTS
    ../src/*
    ../tests/localtests.cpp
)

file(GLOB SERVER
    ../src/*
    ../runserver.cpp
)

set(HEADERS ../include)

# tells the compiler to look for libraries inside ../include
include_directories(${HEADERS})

add_executable(run-tests ${TESTS})
add_executable(run-server ${SERVER})
add_executable(${PROJECT_NAME} ${SOURCES})

# basicaly just include_directories, but for specific builds, though each one uses basically all of the stuff (may exclude networking from main?)
# target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS})