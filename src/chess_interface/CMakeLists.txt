add_library(chess_interface
    movegen_interface.h
    movegen_interface.cpp

)

target_link_libraries(chess_interface PUBLIC blowfish_engine)

