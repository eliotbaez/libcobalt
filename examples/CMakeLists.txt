cmake_minimum_required(VERSION 3.17)
project(libcobalt_examples)

add_executable(encode encode.c)
target_link_libraries(encode cobalt)
target_include_directories(encode 
	PRIVATE
	${CMAKE_SOURCE_DIR}/include)

add_executable(decode decode.c)
target_link_libraries(decode cobalt)
#target_include_directories(decode 
#	PRIVATE 
#	${CMAKE_SOURCE_DIR}/include)

add_custom_target(examples
	DEPENDS encode decode)
