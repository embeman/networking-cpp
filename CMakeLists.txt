cmake_minimum_required(VERSION 3.5)
project(networking)

add_executable(server server/server.cpp server/clientThread.cpp)
add_executable(client client/client.cpp client/comThread.cpp)

set_property(TARGET server PROPERTY CXX_STANDARD 14)
set_property(TARGET client PROPERTY CXX_STANDARD 14)

find_package(Threads)

target_link_libraries(server Threads::Threads)
target_link_libraries(client Threads::Threads)

target_include_directories(server PUBLIC include/)
target_include_directories(client PUBLIC include/)

install(TARGETS server DESTINATION /usr/bin)
install(TARGETS client DESTINATION /usr/bin)

