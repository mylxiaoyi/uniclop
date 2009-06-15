

#if !defined(FEATURES_TRACKING_APPLICATION_HEADER)
#define FEATURES_TRACKING_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include <boost/scoped_ptr.hpp>

#include "devices/video/GstVideoInput.hpp"

#include "algorithms/features/FeaturesTracks.hpp"

namespace uniclop
{

namespace algorithms
{
namespace features
{
class FASTFeaturesMatcher;
}
}
namespace applications
{
namespace features_tracking
{

using uniclop::devices::video::GstVideoInput;
namespace program_options =  boost::program_options;
using boost::scoped_ptr;
using namespace std;

class FeaturesTrackingApplication: public AbstractApplication
{

    scoped_ptr<GstVideoInput> gst_video_input_p;

    // FIXME need to fix IFeaturesMatcher class design so we can use IFeaturesMatcher
    // instead of this specific matcher
    scoped_ptr<FASTFeaturesMatcher> features_matcher_p;

    FeaturesTracks features_tracks;

public:
    string get_application_title() const;
    program_options::options_description get_command_line_options(void) const;
    int main_loop(program_options::variables_map &options);

private:

};

}
}
}
#endif // !defined(FEATURES_TRACKING_APPLICATION_HEADER)
