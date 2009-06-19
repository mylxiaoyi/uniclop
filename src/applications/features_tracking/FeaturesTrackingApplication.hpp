

#if !defined(FEATURES_TRACKING_APPLICATION_HEADER)
#define FEATURES_TRACKING_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include <boost/scoped_ptr.hpp>


#include "devices/video/GstVideoInput.hpp"

#include "algorithms/features/FeaturesTracks.hpp"

#include "algorithms/features/fast/SimpleFAST.hpp"
#include "algorithms/features/IFeaturesMatcher.hpp"
#include "algorithms/features/IFeaturesDetector.hpp"


namespace cimg_library {
class rgb8_cimg_t;
}

namespace uniclop
{

namespace algorithms
{
namespace features
{ namespace fast { 
class FASTFeaturesMatcher; 
}}
}

namespace applications
{
namespace features_tracking
{
	
using namespace std;

using namespace uniclop::applications::args;
using namespace uniclop::algorithms::features;

using boost::scoped_ptr;
using uniclop::devices::video::GstVideoInput;


class FeaturesTrackingApplication: public AbstractApplication
{

    scoped_ptr<GstVideoInput> gst_video_input_p;

    // FIXME need to fix IFeaturesMatcher class design so we can use IFeaturesMatcher
    // instead of this specific matcher
    typedef fast::SimpleFAST::features_type features_type; 
    scoped_ptr<IFeaturesDetector< features_type > > features_detector_p;
    scoped_ptr<IFeaturesMatcher< features_type > > features_matcher_p;
  	
    FeaturesTracks features_tracks;

public:
    string get_application_title() const;
    args::options_description get_command_line_options(void) const;
    int main_loop(args::variables_map &options);

private:
    void draw_tracks(const FeaturesTracks &tracks, cimg_library::rgb8_cimg_t &image);
    
};


}
}
}
#endif // !defined(FEATURES_TRACKING_APPLICATION_HEADER)
