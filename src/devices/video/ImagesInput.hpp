
#if !defined(IMAGES_INPUT_HEADER_INCLUDED)
#define IMAGES_INPUT_HEADER_INCLUDED

#include <string>
#include <vector>

#include <boost/program_options.hpp>

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers


// Cimg http://cimg.sf.net
#include <CImg/CImg.h>

struct CvCapture; // forward declaration to avoid including all of OpenCv

namespace uniclop
{

using namespace std;	
namespace args = boost::program_options;
using namespace cimg_library;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Video input
// class ImagesInput  declaration

template<typename T>
class ImagesInput
{
    vector< string > input_images;
    vector< string >::const_iterator input_images_it;
    bool video_input;
    CvCapture* video_capture;
    bool _reached_last_image;

    int desired_width, desired_height;

    double blur_sigma; // apply blurring to input images (-1 to disable)
    bool grayscale; // transform image to grayscale or not

    CImg<T> current_image;
public:


    static args::options_description get_options_description();
    ImagesInput(args::variables_map &options);
    ~ImagesInput();
    CImg<T> &get_new_image();
    ///< does not return a const reference to avoid inecessary copies
    ///< image will change only in the next call to get_new_image
    bool is_video();
    bool reached_last_image();

private:

    void color_to_grey( CImg<T> &color_image, CImg<T> &grey_image);
    bool grab_frame();

};


}


#endif // !defined(IMAGES_INPUT_HEADER_INCLUDED)
