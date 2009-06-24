
#if !defined(GST_VIDEO_INPUT_HEADER)
#define GST_VIDEO_INPUT_HEADER


#include "IVideoInput.hpp"

#include <string>

#include <boost/program_options.hpp>

//#include <boost/gil/typedefs.hpp>
//#include <boost/gil/image_view.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include <gst/gst.h>


namespace uniclop
{
	
using std::string;
namespace program_options = boost::program_options;
using boost::gil::rgb8_image_t;
using boost::scoped_ptr;

/**
GStreamer video input class
*/
class GstVideoInput: public IVideoInput
{

public:
    // RGB 8 bits interleaved image type
    typedef rgb8_image_t image_t;
    typedef image_t::const_view_t const_view_t;

private:
    GstPipeline *pipeline;

    scoped_ptr<image_t> current_image_p;
    const_view_t current_image_view;
    bool image_is_new;
    boost::mutex current_image_mutex;
    boost::condition new_image_condition;

    string video_sink_name;
    int width, height, depth;

public:
    static program_options::options_description get_options_description();
    GstVideoInput(program_options::variables_map &options);
    ~GstVideoInput();

    /**
    Blocking call to retrieve a new image
    */
    const_view_t &get_new_image();

private:
    void parse_options(program_options::variables_map &options);

    void setup_video_input_pipeline(const string &video_sink_name);

    void video_input_thread();

    static void on_new_frame_callback(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer self_p);

    void on_new_frame(GstElement *element, GstBuffer * buffer, GstPad* pad);
};

}


#endif // GST_VIDEO_INPUT_HEADER