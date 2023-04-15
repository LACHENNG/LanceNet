

set(Logging_SRC
    LogStream.cpp
    Logging.cpp
    Time.cpp
)

add_library(log ${Logging_SRC})

add_executable(logging test/Logging_test.cpp)

target_include_directories(logging PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}
                         )

target_link_libraries(logging PRIVATE
                    log
                    LanceNet_base
                    )
