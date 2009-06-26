

#if !defined(VISUAL_ODOMETRY_APPLICATION_HEADER)
#define VISUAL_ODOMETRY_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include <boost/scoped_ptr.hpp>

#include "devices/video/GstVideoInput.hpp"

namespace uniclop
{

namespace program_options =  boost::program_options;
using boost::scoped_ptr;
using namespace std;

class VisualOdometryApplication: public AbstractApplication
{

    scoped_ptr<GstVideoInput> gst_video_input_p;

public:
    string get_application_title() const;
    program_options::options_description get_command_line_options(void) const;
    int main_loop(program_options::variables_map &options);

private:

};

}
#endif // !defined(VISUAL_ODOMETRY_APPLICATION_HEADER)
