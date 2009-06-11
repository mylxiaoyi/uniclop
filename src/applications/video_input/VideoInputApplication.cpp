#include "VideoInputApplication.hpp"

#include "devices/video/ImagesInput.hpp"

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/assert.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <cassert>


// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

namespace uniclop
{
namespace applications
{
namespace video_input
{

using namespace uniclop::devices::video;
using namespace cimg_library;
using boost::uint8_t;

string VideoInputApplication::get_application_title() const
{

    return "Video input. A simple test application. Uniclop 2009";
}

args::options_description VideoInputApplication::get_command_line_options(void) const
{

    args::options_description desc;

    desc.add(ImagesInput<uint8_t>::get_options_description());
    desc.add(GstVideoInput::get_options_description());

    return desc;
}


void VideoInputApplication::init_video_input(args::variables_map &options)
{

    gst_video_input_p.reset(new GstVideoInput(options));

    return;
}

int VideoInputApplication::main_loop(args::variables_map &options)
{
    printf("VideoInputApplication::main_loop says hello world !\n");

    //init_gui(options);
    //run_gui();

    init_video_input(options);

    GstVideoInput::const_view_t view = gst_video_input_p->get_new_image();
    BOOST_MPL_ASSERT(( is_same< stVideoInput::const_view_t, rgb8_image_t::const_view_t > ));

    const uint8_t *data_p = static_cast<const uint8_t *>(&(view.begin()[0][0]));
    const bool shared_memory = true;
    const CImg<uint8_t> current_image(data_p, view.width(), view.height(), 1, 3, shared_memory);

    // FIXME should port ImagesInput to Gil
    //ImagesInput<uint8_t> images_input(options);
    //const CImg<uint8_t> &current_image = images_input.get_new_image();

    CImgDisplay video_display(current_image.dimx(), current_image.dimy(), "Video stream");
    video_display.show();
    video_display.display(current_image);

    do
    {
        if (true)
        {
            printf(".");
        }

        //const CImg<uint8_t> &current_image = images_input.get_new_image();

        // since we use only memory references, simply updating the image, will update the CImg current_image
        gst_video_input_p->get_new_image();

        video_display.display(current_image);

        const float seconds_to_wait = 0.1; // [seconds]
        boost::xtime t_time;
        boost::xtime_get(&t_time, boost::TIME_UTC);
        t_time.nsec += (1000 * 1000) * seconds_to_wait;
        boost::thread::sleep(t_time);

    }
    while (video_display.is_closed == false);

    return 0;
}

}
}
}
