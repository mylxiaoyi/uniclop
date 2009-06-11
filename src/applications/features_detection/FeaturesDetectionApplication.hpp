
#if !defined(FEATURES_DETECTION_APPLICATION_HEADER)
#define FEATURES_DETECTION_APPLICATION_HEADER

#include "applications/AbstractApplication.hpp"

#include "algorithms/features/features_detection.hpp"

#include <boost/scoped_ptr.hpp>

namespace uniclop
{

namespace algorithms
{
namespace features
{
class SimpleFAST;
}
}

namespace applications
{
namespace features_detection
{

using uniclop::algorithms::features::SimpleFAST;
namespace args = boost::program_options;
using boost::scoped_ptr;
using namespace std;

class FeaturesDetectionApplication : public AbstractApplication
{

    scoped_ptr<SimpleFAST> features_detector_p;

public:
    string get_application_title() const;
    args::options_description get_command_line_options(void) const;
    int main_loop(args::variables_map &options);

private:

};

}
}
}

#endif // FEATURES_DETECTION_APPLICATION_HEADER
