#include "FeaturesDetectionApplication.hpp"

#include "devices/video/ImagesInput.hpp"

#include "algorithms/features/features_detection.hpp"

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>

namespace uniclop
{
namespace applications
{
namespace features_detection
{

using namespace uniclop::devices::video;

string FeaturesDetectionApplication::get_application_title() const
{

    return "Features detection input. A simple test application. Uniclop 2009";
}

args::options_description FeaturesDetectionApplication::get_command_line_options(void) const
{

    args::options_description desc;

    desc.add(ImagesInput<uint8_t>::get_options_description());

    return desc;
}


int FeaturesDetectionApplication::main_loop(args::variables_map &options)
{
    printf("FeaturesDetectionApplication::main_loop says hello world !\n");


    //init_gui(options);
    //run_gui();


    //gst_video_input_p.reset(new GstVideoInput(options));
    //features_detector_p.reset(new SimpleFAST(options));

    // color_image_view =gst_video_input_p -> get_new_image
    // color to gray_image
    // features_detector_p -> detect_features(grey_image)
    // plot features on output image

    return 0;
}

}
}
}
