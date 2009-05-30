

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers
#include "images_input.hpp"

// extra implementation specific headers
// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

// OpenCv http://opencvlibrary.sf.net
#include <cv.h>
#include <highgui.h>

// Boost http://boost.org
#include <boost/cstdint.hpp>
using boost::uint8_t;

// C++ std
#include <vector>
#include <iostream>
using namespace std;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

// class ImagesInput  methods implementation
template<typename T>
args::options_description ImagesInput<T>::get_options_description()
{
    args::options_description desc("Images Input options");
    desc.add_options()

    ( "blur_sigma", args::value<double>()->default_value(0.0f),
      "defines a bluring level for images")

    ("max_size", args::value< int >()->default_value(800),
     "Maximum size (height or width) of the test image (will be resized if necessary)")

    ("input_video",  args::value<string>(), "input video file")
    ("input_camera",  args::value<int>(), "input camera number")
    ("input_images", args::value< vector<string> >(), "a list of filenames")

    ("use_color_images",  args::value<bool>()->default_value(false),
     "use color images for the algorithm")
    ;

    return desc;
}


template<typename T>
ImagesInput<T>::ImagesInput(args::variables_map &options)
{
    _reached_last_image = false;
    video_capture = NULL;

    // parse options --
    if(options.count("input_images"))
        input_images = options["input_images"].as< vector<string> >();

    video_input = (options.count("input_video") != 0 || options.count("input_camera") !=0 );


    blur_sigma = -1.0f; // negative value indicates no blurring
    if(options.count("blur_sigma") &&  false) // <<< disabling blur sigma because it is not good for features detection
    {
        blur_sigma = options["blur_sigma"].as<double>();
        cout << "Blurring the images with sigma value " << blur_sigma << endl;
    }

    grayscale = true;
    if(options.count("use_color_images"))
        grayscale = ! options["use_color_images"].as<bool>();

    // load first image --

    video_capture = NULL;


    if(!video_input)
    {
        if( input_images.empty() )
            throw runtime_error("No input images where defined");
        if(input_images.size() < 2)
            throw runtime_error("The software requires at least two input images");


        input_images_it = input_images.begin();

        current_image = CImg<T>( input_images_it->c_str() );

        if(grayscale && current_image.dimv() != 1)
        {
            CImg<T> color_image = current_image;
            current_image = CImg<T>(current_image.dimx(), current_image.dimy(), 1, 1);
            color_to_grey(color_image, current_image);
        }

    }
    else
    {
        // open the video input --
        if(options.count("input_camera"))
        {
            printf("Trying to capture from a video device...");
            video_capture = cvCaptureFromCAM( options["input_camera"].as<int>() );
            if (!video_capture)
                printf("Failed.\n");
            else
                printf("\n");

            cvSetCaptureProperty(video_capture, CV_CAP_PROP_FRAME_WIDTH, 640);
            cvSetCaptureProperty(video_capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
        }

        if(options.count("input_video"))
        {
            printf("Trying to capture from a video file...");
            video_capture = cvCaptureFromAVI( options["input_video"].as<string>().c_str() );
            if (!video_capture)
                printf("Failed.\n");
            else
                printf("\n");
        }

        if(!video_capture)
        {
            throw runtime_error("\nCould not obtain a video input. Please try with another parameters.\n");
            return;
        }

        int i;
        for(i=0; i < 5; i+=1)
        { // skip the initial frames (camera auto adjustment, video codec initialization)
            bool ret = grab_frame();
            if(ret == false)
                throw runtime_error("Could not grab the first frames");
        }
    }

    desired_width = current_image.dimx();
    desired_height = current_image.dimy();

    { // resize the image to avoid too long computation
        const int
        max_size = options["max_size"].as< int >(); // copy

        const int max_width = max_size, max_height = max_size;

        desired_width = min(desired_width, max_width);
        desired_height = min(desired_height, max_height);

        if((desired_width != current_image.dimx()) || (desired_height != current_image.dimy()))
        {
            printf("Going to resize the images to (%i, %i) [pixels]\n", desired_width, desired_height);
        }
    }

    return;
}

template<typename T>
ImagesInput<T>::~ImagesInput()
{
    if(video_capture != NULL) cvReleaseCapture( &video_capture );
    return;
}

template<typename T>
bool ImagesInput<T>::is_video()
{
    return video_input;
}

template<typename T>
bool ImagesInput<T>::reached_last_image()
{
    return _reached_last_image;
}

template<typename T>
void ImagesInput<T>::color_to_grey( CImg<T> &color_image, CImg<T> &grey_image)
{

    if(color_image.is_sameXYZ(grey_image) == false
            || color_image.dimv() !=  3
            ||   grey_image.dimv() != 1)
        throw runtime_error("convert_to_grey input image does not match the expected dimensions");

    cimg_forXY(color_image, x, y)
    {

        const float r= (float) color_image(x,y,0,0);
        const float g= (float) color_image(x,y,0,1);
        const float b= (float) color_image(x,y,0,2);
        float grey = 0.3*r+0.6*g+0.1*b;
        grey_image(x,y) = (T) grey;
    }

    return;
}

template<typename T>
bool ImagesInput<T>::grab_frame()
{
    // return false if no new frame was acquired

    if(video_capture == NULL)
        throw runtime_error("grab_frame was called but video_capture == NULL");

    CImg<T> &frame = current_image;

    IplImage* grabbed_frame_p = cvQueryFrame( video_capture );
    if(grabbed_frame_p == NULL)
        return false;

#if defined(_MSC_VER)
    //	cvConvertImage(grabbed_frame_p, grabbed_frame_p, CV_CVTIMG_FLIP); // upside down under windows
#endif

    if(grabbed_frame_p->nChannels != 3)
        throw runtime_error("Expected a color video input");

    const int num_channels = (grayscale == true)? 1:grabbed_frame_p->nChannels;

    if( (grabbed_frame_p->width != frame.dimx())
            || (grabbed_frame_p->height != frame.dimy())
            || (num_channels != frame.dimv()) )
    { // resize the frame
        if(false)
        { // just for debugging
            printf("grabbed_frame_p dimensions == (%i,%i,0,%i)\n",
                   grabbed_frame_p->width, grabbed_frame_p->height, grabbed_frame_p->nChannels);

        }
        frame= CImg<T>(grabbed_frame_p->width, grabbed_frame_p->height,
                       1, num_channels);
    }

    if(grabbed_frame_p->width == 0 || grabbed_frame_p->height == 0)
        throw runtime_error("The grabbed_frame has zero size");

    if(frame.dimx() == 0 || frame.dimy() == 0)
        throw runtime_error("The data frame has zero size");

    if(grabbed_frame_p->nChannels != 3)
        throw runtime_error("Grayscale video input not yet managed");

    // pixel per pixel copy
    unsigned char *row_ptr, *data_ptr;
    int x,y;

    for(row_ptr = (unsigned char *) grabbed_frame_p->imageData, y = 0;
            y < grabbed_frame_p->height;
            row_ptr+=  grabbed_frame_p->widthStep, y+=1)
    {
        for(data_ptr = row_ptr, x = 0;
                x < grabbed_frame_p->width;
                data_ptr+=grabbed_frame_p->nChannels, x+=1 )
        {
#if defined(_MSC_VER)
            const int w = grabbed_frame_p->width - 1;
            const int h = grabbed_frame_p->height - 1;
            const int u = w - x;
            const int v = h - y;
#else

            const int u=x;
            const int v=y;
#endif

            if(grayscale)
            { // color to grey conversion
                const float r= (float) data_ptr[0], g=(float) data_ptr[1], b= (float) data_ptr[2];
                float grey = 0.3*r+0.6*g+0.1*b;
                frame(u,v) = (T) grey;
            }
            else
            { // colors image
                frame(u,v,0,0) = data_ptr[0];
                frame(u,v,0,1) = data_ptr[1];
                frame(u,v,0,2) = data_ptr[2];
            }

        } // for each column
    } // for each row

    return true;
}


template<typename T>
CImg<T> & ImagesInput<T>::get_new_image()
{
    // acquire the new image --
    if(video_input)
    {
        bool ret= grab_frame();

        if( ret == false) // could not retrieve a new image
            _reached_last_image = true;
    }
    else
    {
        if( input_images_it != input_images.end() )
        {
            current_image = CImg<T>( input_images_it->c_str() );

            if(grayscale && current_image.dimv() != 1)
            {
                CImg<T> color_image = current_image;
                current_image = CImg<T>(current_image.dimx(), current_image.dimy(), 1, 1);
                color_to_grey(color_image, current_image);
            }

            ++input_images_it;
        }

	// check if we reached the last image or not
  	_reached_last_image = (input_images_it == input_images.end());
        
    }

    // post process the acquired image --
    // resize
    if((desired_width != current_image.dimx()) || (desired_height != current_image.dimy()))
    {
        current_image.resize(desired_width, desired_height, -100, -100, 3);
    }


    // blur the image
    if(blur_sigma >= 0.0f)
    {
        current_image.blur(blur_sigma);
    }


    return current_image;
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Force the compilation of the following types
// for some strange reason, in linux, this hast to be at the end of the defitions (?!)
template class ImagesInput<uint8_t>;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

