
aux_source_directory(. COMMON_SRCS)
add_executable(test_ncurse ${COMMON_SRCS})
target_link_libraries(test_ncurse ncurses)

