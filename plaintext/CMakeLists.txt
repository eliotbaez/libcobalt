cmake_minimum_required(VERSION 3.21.4)
project(libcobalt_plaintext)
message("plaintext is being built")
add_executable(sort_wordlist sort_wordlist.c)

add_custom_target(50k-newline-separated.txt
	COMMAND python3 uncomment.py
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(50k-newline-separated-sorted.txt
	COMMAND sort_wordlist
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS 50k-newline-separated.txt sort_wordlist
)

message("building wordtable.h")
add_custom_target(wordtable.h ALL
	COMMAND python3 convert_to_c.py
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS 50k-newline-separated-sorted.txt
)
