#include "VideoInputApplication.hpp"

#include "devices/video/ImagesInput.hpp"

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

#include <cstdio>

namespace uniclop
{
namespace applications
{
namespace video_input
{

using namespace uniclop::devices::video;

string VideoInputApplication::get_application_title() const
{

	return "Video input. A simple test application. Uniclop 2009";
}

args::options_description VideoInputApplication::get_command_line_options(void) const
{

	args::options_description desc;

	desc.add(ImagesInput<uint8_t>::get_options_description());

	return desc;
}

int VideoInputApplication::main_loop(args::variables_map &options)
{

	//init_gui(options);
	//run_gui();

	printf("Hello world !\n");

	// FIXME should port ImagesInput to Gil
	ImagesInput<uint8_t> images_input(options);
	const CImg<uint8_t> &current_image = images_input.get_new_image();

	CImgDisplay video_display(current_image.dimx(), current_image.dimy(), "Video stream");
	video_display.show();
	video_display.display(current_image);

	do
	{
		if (true)
		{
			printf(".");
		}

		const CImg<uint8_t> &current_image = images_input.get_new_image();
		video_display.display(current_image);

		const float seconds_to_wait = 0.1; // [seconds]
		boost::xtime t_time;
		boost::xtime_get(&t_time, boost::TIME_UTC);
		t_time.nsec += (1000 * 1000) * seconds_to_wait;
		boost::thread::sleep(t_time);

	} while (video_display.is_closed == false);

	return 0;
}

}
}
}
