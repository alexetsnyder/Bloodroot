add_library(FileIO)

target_sources(
	FileIO
	PRIVATE
		FileIO.cpp 

	PUBLIC
		FILE_SET HEADERS
		FILES
			FileIO.h
)