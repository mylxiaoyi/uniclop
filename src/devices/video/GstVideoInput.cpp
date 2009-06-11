

#include "GstVideoInput.hpp"


#include <boost/gil/gil_all.hpp>

#include <stdexcept>

namespace uniclop
{
namespace devices
{
namespace video
{


program_options::options_description GstVideoInput::get_options_description()
{
    program_options::options_description desc("GstVideoInput options");
    desc.add_options()
    ("width", program_options::value< int >()->default_value(640),
     "Define the input image width to define the input size. Accepted values are 320, 640, 800 (320x240, 640x480 and 800x600 respectivelly)")

    ("video_sink",  program_options::value<string>()->default_value("v4l2src"), "video input  gstreamer module")
    ;

    return desc;
}


void GstVideoInput::parse_options(program_options::variables_map &options)
{


    video_sink_name = "v4l2src";
    if (options.count("video_sink") != 0)
    {
        video_sink_name = options["video_sink"].as<string>();
    }

    width = 640;
    height = 480;

    if (options.count("width") != 0)
    {
        width = options["width"].as<int>();
    }
    switch (width)
    {
    case 320:
        height = 240;
        break;
    case 640:
        height = 480;
        break;
    case 800:
        height = 600;
        break;


    default:
        throw std::runtime_error("GstVideoInput received and unmanaged video width value");
    }


    return;
}

GstVideoInput::GstVideoInput(program_options::variables_map &options)
{

    parse_options(options);

    current_image_p.reset(new GstVideoInput::image_t(width, height));
    current_image_view = boost::gil::const_view(*current_image_p);

    init_gstreamer(video_sink_name);

    return;
}


GstVideoInput::~GstVideoInput()
{
    // nothing to do here
    return;
}

void GstVideoInput::init_gstreamer(const string &video_sink_name)
{



    return;
}


GstVideoInput::const_view_t &GstVideoInput::get_new_image()
{
    // wait until a new image has arrived

    return  current_image_view;
}


}
}
}