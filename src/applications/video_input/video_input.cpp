/*
 * Simple video input application, used to verify that uniclop is able to access your video device
 *
 */

#include "applications/video_input/VideoInputApplication.hpp"
#include <boost/scoped_ptr.hpp>

int main(int argc, char *argv[])
{
	boost::scoped_ptr<AbstractApplication> application_p(new VideoInputApplication());
	return application_p->main(argc, argv);
}
