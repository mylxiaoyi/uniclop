
#ifndef VIDEOINPUTAPPLICATION_HPP_
#define VIDEOINPUTAPPLICATION_HPP_

#include "applications/AbstractApplication.hpp"

namespace uniclop
{
namespace applications
{
namespace features_detection
{

namespace args = boost::program_options;
using namespace std;

class FeaturesDetectionApplication : public AbstractApplication {


	int width, height, depth;

public:
	string get_application_title() const;
	args::options_description get_command_line_options(void) const;
	int main_loop(args::variables_map &options);

private:
	
};

}
}
}

#endif /* VIDEOINPUTAPPLICATION_HPP_ */
