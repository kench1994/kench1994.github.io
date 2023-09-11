cmake_minimum_required(VERSION 3.00)

project(bg_service)

add_executable(service
    main.cpp
    )

add_executable(worker
    worker.cpp
    )

add_executable(invoker
    invoker.cpp
    )

target_link_libraries(invoker PUBLIC pthread)
target_link_libraries(invoker PUBLIC dl)


set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../)
install(TARGETS invoker LIBRARY DESTINATION bin)
