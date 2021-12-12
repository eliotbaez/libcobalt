# I'm choosing this version because it's the oldest version that I have tested
# on my computer. If you find an older version that has all the necessary
# features, then this may change.
cmake_minimum_required(VERSION 3.21.4)

project(libcobalt
#	VERSION 0.1	# do I really wanna implement this right now?
	DESCRIPTION "Text compression library by a lookup table"
	LANGUAGES C
)

# this is our main target
add_library(cobalt SHARED
	src/blocksize.c
	src/findword.c
	src/sentence.c
)

set_target_properties(cobalt PROPERTIES
	PUBLIC_HEADER include/cobalt.h
)

target_include_directories(cobalt PRIVATE include)
target_include_directories(cobalt PRIVATE src)

include(GNUInstallDirs)
install(TARGETS cobalt
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_dependencies(cobalt
	wordtable.h
	wordmap.h
	guidetable.h
)


add_subdirectory(plaintext)
add_subdirectory(util)
add_subdirectory(map)
#target_link_libraries(cobalt util)
#target_link_libraries(cobalt plaintext)
#target_link_libraries(cobalt map)

#add_dependencies(cobalt 
	#plaintext
	#util
	#map
	#)



