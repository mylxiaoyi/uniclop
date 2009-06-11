/*
 * Simple video input application, used to verify that uniclop is able to access your video device
 *
 */

#include "VideoInputApplication.hpp"
#include <boost/scoped_ptr.hpp>

int main(int argc, char *argv[])
{
    using uniclop::applications::AbstractApplication;
    using uniclop::applications::video_input::VideoInputApplication;

    boost::scoped_ptr<AbstractApplication> application_p(new VideoInputApplication());
    return application_p->main(argc, argv);
}
