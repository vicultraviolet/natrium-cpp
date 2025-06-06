#include "Pch.hpp"
#include "Natrium/Main.hpp"
#undef main

#if defined(NA_WINDOWED_APP)
#include <Windows.h>
#endif // NA_WINDOWED_APP

extern int Na::Main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	try
	{
		return Na::Main(argc, argv);
	} catch (const std::exception& e)
	{
		Na::g_Logger.printf(Na::Fatal, "Unhandled exception reached main! {}", e.what());
		return -1;
	}
	return 0;
}

#if defined(NA_WINDOWED_APP)
int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE HPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{
	return main(__argc, __argv);
}
#endif // NA_WINDOWED_APP
