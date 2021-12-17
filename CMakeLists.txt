# I'm choosing this version because it's the oldest version that I have tested
# on my computer. If you find an older version that has all the necessary
# features, then this may change.
cmake_minimum_required(VERSION 3.17)

project(libcobalt
#	VERSION 0.1	# do I really wanna implement this right now?
	DESCRIPTION "Text compression library by a lookup table"
	LANGUAGES C
)

# TODO:
# Add a way to configure which file will be used as the core word list!

add_subdirectory(util)
add_subdirectory(plaintext)
add_subdirectory(map)
#add_subdirectory(examples)

# this is our main target
add_library(cobalt SHARED
	${CMAKE_SOURCE_DIR}/src/findword.c
	${CMAKE_SOURCE_DIR}/src/sentence.c
	${CMAKE_SOURCE_DIR}/src/blocksize.c
	${CMAKE_SOURCE_DIR}/src/globals/sizes.c
	${CMAKE_SOURCE_DIR}/src/globals/wordtable.c
	${CMAKE_SOURCE_DIR}/src/globals/wordmap.c
	${CMAKE_SOURCE_DIR}/src/globals/guidetable.c)

set_source_files_properties(
	src/globals/sizes.c
	src/globals/wordtable.c
	src/globals/wordmap.c
	src/globals/guidetable.c
	PROPERTIES
	GENERATED TRUE)

set_target_properties(cobalt PROPERTIES
	PUBLIC_HEADER include/cobalt.h)
add_dependencies(cobalt
	sizes
	wordtable
	wordmap
	guidetable)
target_include_directories(cobalt PRIVATE include)
# target_include_directories(cobalt PRIVATE src)

include(GNUInstallDirs)
install(TARGETS cobalt
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

