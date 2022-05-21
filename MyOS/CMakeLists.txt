cmake_minimum_required(VERSION 3.11)

project(Disk)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(. DISK_SOURCE_FILES)
aux_source_directory(file_manage DISK_SOURCE_FILES)

add_executable(diskManagement ${DISK_SOURCE_FILES})
