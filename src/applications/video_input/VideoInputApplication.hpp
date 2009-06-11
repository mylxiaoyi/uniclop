
#ifndef VIDEOINPUTAPPLICATION_HPP_
#define VIDEOINPUTAPPLICATION_HPP_

#include "applications/AbstractApplication.hpp"

#include "devices/video/GstVideoInput.hpp"

namespace uniclop
{
namespace applications
{
namespace video_input
{

namespace args = boost::program_options;
using namespace std;
using uniclop::devices::video::GstVideoInput;


class VideoInputApplication : public AbstractApplication
{

    boost::scoped_ptr<GstVideoInput> gst_video_input_p;

public:
    string get_application_title() const;
    args::options_description get_command_line_options(void) const;
    int main_loop(args::variables_map &options);

private:
    void init_video_input(args::variables_map &options);

};

}
}
}

#endif /* VIDEOINPUTAPPLICATION_HPP_ */
