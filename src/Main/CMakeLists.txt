
include_directories(../common struct/)

aux_source_directory(. MAINSRCS)
aux_source_directory(server/ SERVER_SRCS)
aux_source_directory(client/ CLIENT_SRCS)
aux_source_directory(struct/ STRUCT_SRCS)

add_executable(CR ${MAINSRCS} ${STRUCT_SRCS} ${SERVER_SRCS} ${CLIENT_SRCS}) 
target_link_libraries(CR cdk ncurses common)

