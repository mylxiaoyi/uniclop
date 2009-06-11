

#include "FeaturesDetectionApplication.hpp"
#include <boost/scoped_ptr.hpp>


int main(int argc, char *argv[])
{
	using uniclop::applications::AbstractApplication;
	using uniclop::applications::features_detection::FeaturesDetectionApplication;
	boost::scoped_ptr<AbstractApplication> application_p(new FeaturesDetectionApplication());
	return application_p->main(argc, argv);
}
