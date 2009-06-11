

#include "applications/AbstractApplication.hpp"
#include <boost/scoped_ptr.hpp>


int main(int argc, char *argv[])
{
    boost::scoped_ptr<AbstractApplication> application_p(new FeaturesTrackingApplication());
    return application_p->main(argc, argv);
}
