
#if !defined(FEATURES_DETECTION_APPLICATION_HEADER)
#define FEATURES_DETECTION_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include "algorithms/features/features_detection.hpp"

#include <boost/scoped_ptr.hpp>

#include "devices/video/GstVideoInput.hpp"

#include <vector>

namespace uniclop
{

namespace devices
{
namespace video
{
class GstVideoInput;
}
}

namespace applications
{
namespace features_detection
{

using uniclop::devices::video::GstVideoInput;
using uniclop::algorithms::features::SimpleFAST;
using uniclop::algorithms::features::FASTFeature;

namespace program_options =  boost::program_options;
using boost::scoped_ptr;
using namespace std;

class FeaturesDetectionApplication : public AbstractApplication
{

    scoped_ptr<GstVideoInput> gst_video_input_p;
    scoped_ptr<SimpleFAST> features_detector_p;

public:
    string get_application_title() const;
    program_options::options_description get_command_line_options(void) const;
    int main_loop(program_options::variables_map &options);

private:

    void draw_features(const vector<FASTFeature> &features);

};

}
}
}

#endif // FEATURES_DETECTION_APPLICATION_HEADER
