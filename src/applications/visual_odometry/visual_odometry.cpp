

#include "VisualOdometryApplication.hpp"
#include <boost/scoped_ptr.hpp>


int main(int argc, char *argv[])
{
    using uniclop::AbstractApplication;
    using uniclop::VisualOdometryApplication;
    boost::scoped_ptr<AbstractApplication> application_p(new VisualOdometryApplication());
    return application_p->main(argc, argv);
}
