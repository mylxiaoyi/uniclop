

#include "VisualOdometryApplication.hpp"
#include <boost/scoped_ptr.hpp>


int main(int argc, char *argv[])
{
    using uniclop::applications::AbstractApplication;
    using uniclop::applications::visual_odometry::VisualOdometryApplication;
    boost::scoped_ptr<AbstractApplication> application_p(new VisualOdometryApplication());
    return application_p->main(argc, argv);
}
