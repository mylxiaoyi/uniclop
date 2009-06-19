
#include "AbstractApplication.hpp"

#include <fstream>
#include <iostream>

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

namespace uniclop
{
namespace applications
{

AbstractApplication::AbstractApplication()
{

    return;
}

AbstractApplication::~AbstractApplication()
{

    return;
}

int AbstractApplication::main(int argc, char *argv[])
{

#ifdef WIN32
    freopen( "CON", "w", stdout ); // redirect stdout to console output
    freopen( "CON", "w", stderr ); // redirect stderr to console output
#endif

    try
    {
        cout << get_application_title() << endl;

        args::variables_map options = parse_arguments(argc, argv);

        const int ret =  main_loop(options);
        cout << "End of game, have a nice day." << endl;
        return ret;

    }
    catch (std::exception & e)
    {
        cout << endl << "[FATAL ERROR] A std::exception was raised: " << e.what() << endl;
    }
    catch (...)
    {
        cout << "[FATAL ERROR] An exception was raised" << endl;
    }
    return 1;
}



args::variables_map AbstractApplication::parse_arguments(int argc, char *argv[])
{

    args::options_description desc("Allowed options");
    desc.add_options()("help", "produces this help message");

    desc.add(get_command_line_options());

    args::variables_map options;

    try
    {
        args::store(args::parse_command_line(argc, argv, desc), options);
        args::notify(options);
    }
    catch (...)
    {
        cout << "Error parsing the command line options." << endl;
        cout << desc << endl;
        exit(-1);
    }

    if (options.count("help"))
    {
        cout << desc << "\n";
        exit(0);
    }

    return options;
}


	void AbstractApplication::wait_some_seconds(const float seconds_to_wait) {
		
        boost::xtime t_time;
        boost::xtime_get(&t_time, boost::TIME_UTC);
        t_time.nsec += (1000 * 1000) * seconds_to_wait;
        boost::thread::sleep(t_time);
        
	return;	
	}



}
}
