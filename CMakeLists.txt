cmake_minimum_required(VERSION 3.30)
project(hola)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hola main.cpp
        BufferPool.cpp
        CommandParser.cpp
        Frame.cpp
        ResultDisplayer.cpp
        RequestResults.h
        ResultDisplayer.h
        Frame.h
        DiskStorageConfig.h
        CommandParser.h)
