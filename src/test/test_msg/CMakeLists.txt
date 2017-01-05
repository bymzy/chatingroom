include_directories(../../common)
aux_source_directory(. COMMON_SRCS)
add_executable(test_msg ${COMMON_SRCS})
target_link_libraries(test_msg common)

