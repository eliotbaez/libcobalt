#cmake_minimum_required(VERSION 3.21.4)

project(libcobalt_map)

add_executable(construct_map
	construct_map.c
	${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	${CMAKE_SOURCE_DIR}/src/globals/sizes.c)
target_include_directories(construct_map PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/include)
add_dependencies(construct_map
	wordtable
	sizes)

add_custom_target(generate_wordmap
	COMMAND construct_map
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS construct_map)

add_custom_target(wordmap
	COMMAND c_hexdump 4 wordmap.bin ../src/globals/wordmap.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS generate_wordmap c_hexdump)

add_executable(construct_guidetable
	construct_guidetable.c
	${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	${CMAKE_SOURCE_DIR}/src/globals/wordmap.c
	${CMAKE_SOURCE_DIR}/src/globals/sizes.c)
target_include_directories(construct_guidetable PRIVATE
	${CMAKE_SOURCE_DIR}/include)
add_dependencies(construct_guidetable
	wordtable
	wordmap
	sizes)

add_custom_target(generate_guidetable
	COMMAND construct_guidetable
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS construct_guidetable)

add_custom_target(guidetable
	COMMAND c_hexdump 2 guidetable.bin ../src/globals/guidetable.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS generate_guidetable c_hexdump wordmap)

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/wordmap.bin
	${CMAKE_CURRENT_SOURCE_DIR}/guidetable.bin
	${CMAKE_SOURCE_DIR}/src/globals/wordmap.c
	${CMAKE_SOURCE_DIR}/src/globals/guidetable.c
	# these 2 from another directory:
	${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	${CMAKE_SOURCE_DIR}/src/globals/sizes.c
	PROPERTIES
	GENERATED TRUE)
