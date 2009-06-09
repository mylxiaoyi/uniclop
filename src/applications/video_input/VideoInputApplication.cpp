#include "VideoInputApplication.hpp"

#include "devices/video/ImagesInput.hpp"

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

#include <cstdio>
#include <iostream>


// using C code because gstreamermm was too much paint to install

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

void VideoInputApplication::init_video_input() {

	  Glib::RefPtr<Gst::Pipeline> pipeline;
	  Glib::RefPtr<Gst::Element> element_source, element_filter, element_sink;

	  // Initialize Gstreamermm:
	  Gst::init(argc, argv);

	  // Create pipeline:
	  pipeline = Gst::Pipeline::create("my-pipeline");

	  // Create elements:
	  element_source = Gst::ElementFactory::create_element("v4l2src");
	  element_filter = Gst::ElementFactory::create_element("identity");
	  element_sink = Gst::ElementFactory::create_element("fakesink");

	  // We must add the elements to the pipeline before linking them:
	  try
	  {
	    pipeline->add(element_source)->add(element_filter)->add(element_sink);
	  }
	  catch (std::runtime_error& ex)
	  {
	    std::cerr << "Exception while adding: " << ex.what() << std::endl;
	    return 1;
	  }

	  // Link the elements together:
	  try
	  {
	    element_source->link(element_filter)->link(element_sink);
	  }
	  catch(const std::runtime_error& error)
	  {
	    std::cerr << "Exception while linking: " << error.what() << std::endl;
	  }


	return;
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
