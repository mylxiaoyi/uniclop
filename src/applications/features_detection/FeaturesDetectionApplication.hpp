
#if !defined(FEATURES_DETECTION_APPLICATION_HEADER)
#define FEATURES_DETECTION_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include "algorithms/features/fast/SimpleFAST.hpp"

#include "devices/video/GstVideoInput.hpp"

#include <vector>
#include <boost/scoped_ptr.hpp>


namespace cimg_library
{
class rgb8_cimg_t;
}

namespace uniclop
{

class GstVideoInput;

namespace program_options =  boost::program_options;
using boost::scoped_ptr;
using namespace cimg_library;
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

    void draw_features(const vector<FASTFeature> &features, rgb8_cimg_t &image);

};

}

#endif // FEATURES_DETECTION_APPLICATION_HEADER
