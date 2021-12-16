#cmake_minimum_required(VERSION 3.21.4)
project(libcobalt_plaintext)
#message("plaintext is being built")

add_executable(sort_wordlist sort_wordlist.c)

add_custom_target(wordlist
	COMMAND python3 uncomment.py
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(wordlist_sorted
	COMMAND sort_wordlist
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS wordlist sort_wordlist)

add_custom_command(
	OUTPUT wordtable.bin ${CMAKE_SOURCE_DIR}/src/globals/sizes.c
	COMMAND python3 newline_to_null.py
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS wordlist_sorted)
add_custom_target(generate_wordtable
	DEPENDS wordtable.bin)
add_custom_target(sizes
	DEPENDS ${CMAKE_SOURCE_DIR}/src/globals/sizes.c)

add_custom_target(wordtable
	COMMAND c_hexdump 1 wordtable.bin ${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS generate_wordtable c_hexdump)

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/50k-newline-separated.txt
	${CMAKE_CURRENT_SOURCE_DIR}/50k-newline-separated-sorted.txt
	${CMAKE_CURRENT_SOURCE_DIR}/wordtable.bin
	${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	${CMAKE_SOURCE_DIR}/src/globals/sizes.c
	PROPERTIES
	GENERATED TRUE)