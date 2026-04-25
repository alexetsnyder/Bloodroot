#include "BloodrootApp.h"

#include <iostream>

int main()
{
	try
	{
		BloodrootApp app;
		app.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
} 