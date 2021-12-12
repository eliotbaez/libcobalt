#cmake_minimum_required(VERSION 3.21.4)

project(libcobalt_map)

add_executable(construct_map construct_map.c)
target_include_directories(construct_map PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_dependencies(construct_map
	wordtable.h
)

add_custom_target(wordmap.bin
	COMMAND construct_map
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS construct_map
)

add_custom_target(wordmap.h
	COMMAND c_hexdump 4 wordmap.bin ../src/wordmap.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS wordmap.bin c_hexdump
)

add_executable(construct_guidetable construct_guidetable.c)
target_include_directories(construct_guidetable PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_dependencies(construct_guidetable
	wordtable.h
	wordmap.h
)

add_custom_target(guidetable.bin
	COMMAND construct_guidetable
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS construct_guidetable 
)

add_custom_target(guidetable.h ALL
	COMMAND c_hexdump 2 guidetable.bin ../src/guidetable.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS guidetable.bin c_hexdump wordmap.h
)
