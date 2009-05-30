
#ifndef ABSTRACTAPPLICATION_HPP_
#define ABSTRACTAPPLICATION_HPP_

#include <boost/program_options.hpp>
// see http://boost.org/doc/html/program_options/

#include <string>

namespace uniclop
{
namespace applications
{

namespace args = boost::program_options;
using namespace std;

/**
 * Base class for applications objects
 */
class AbstractApplication
{

	args::variables_map options;

public:
	AbstractApplication();
	~AbstractApplication();

	int main(int argc, char *argv[]);

private:
	args::variables_map parse_arguments(int argc, char *argv[]);

public:
	virtual string get_application_title() const = 0;
	virtual args::options_description get_command_line_options(void) const = 0;
	virtual int main_loop(args::variables_map &options) = 0;

};

}
}

#endif /* ABSTRACTAPPLICATION_HPP_ */
