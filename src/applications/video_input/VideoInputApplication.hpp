
#ifndef VIDEOINPUTAPPLICATION_HPP_
#define VIDEOINPUTAPPLICATION_HPP_

#include "applications/AbstractApplication.hpp"

#include <gst/gst.h>

namespace uniclop
{
namespace applications
{
namespace video_input
{

namespace args = boost::program_options;
using namespace std;

class VideoInputApplication : public AbstractApplication {


	int width, height, depth;

public:
	string get_application_title() const;
	args::options_description get_command_line_options(void) const;
	int main_loop(args::variables_map &options);

private:
	void init_video_input();
	
	static void on_new_frame_callback(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer self_p);
	
	void on_new_frame(GstElement *element, GstBuffer * buffer, GstPad* pad);
	
};

}
}
}

#endif /* VIDEOINPUTAPPLICATION_HPP_ */
