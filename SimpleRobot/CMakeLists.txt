cmake_minimum_required(VERSION 2.8.3)
project(simple_robot)

include_directories(
  include include
)

add_library(common STATIC
    include/common.h
    src/common.cpp
)

add_library(Protocol STATIC
    include/Protocol.h
    src/Protocol.cpp
)

add_library(Client STATIC
    include/Client.h
    src/Client.cpp
)

add_library(Server STATIC
    include/Server.h
    src/Server.cpp
)


add_executable(client
    src/client_main.cpp
    include/common.h
    src/common.cpp
    include/Protocol.h
    src/Protocol.cpp
    include/Client.h
    src/Client.cpp
)
target_link_libraries(client common Protocol Client)

add_executable (server
    src/server_main.cpp
    include/common.h
    src/common.cpp
    include/Protocol.h
    src/Protocol.cpp
    include/Server.h
    src/Server.cpp
)
target_link_libraries(server common Protocol  Server)
