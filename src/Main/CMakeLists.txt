
include_directories(../common struct/)

aux_source_directory(. MAINSRCS)
aux_source_directory(server/ SERVER_SRCS)
aux_source_directory(client/ CLIENT_SRCS)

add_executable(CR ${MAINSRCS} ${SERVER_SRCS} ${CLIENT_SRCS})
target_link_libraries(CR cdk ncurses common)

