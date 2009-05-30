
#include "VideoInputApplication.hpp"

#include <cstdio>

namespace uniclop
{
namespace applications
{
namespace video_input
{

string VideoInputApplication::get_application_title() const {

	return "Video input. A simple test application. Uniclop 2009";
}


args::options_description VideoInputApplication::get_command_line_options(void) const {

	args::options_description desc;


	return desc;
}

int VideoInputApplication::main_loop(args::variables_map &options) {

	//init_gui(options);
	//run_gui();

	printf("Hello world !\n");

	return 0;
}



}
}
}
