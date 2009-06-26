
#include "GstVideoInput.hpp"

#include <stdexcept>

// using C code because gstreamermm was too much paint to install
#include <glib.h>
#include <glibmm.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include <gst/video/video.h>

#include <CImg/CImg.h>

#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>

namespace uniclop
{

using namespace cimg_library;
using boost::uint8_t;
using namespace boost::gil;

program_options::options_description GstVideoInput::get_options_description()
{
    program_options::options_description desc("GstVideoInput options");
    desc.add_options()
    ("width", program_options::value< int >()->default_value(640),
     "Define the input image width to define the input size. Accepted values are 320, 640, 800 (320x240, 640x480 and 800x600 respectivelly)")

    ("video_sink",  program_options::value<string>()->default_value("v4l2src"),
     "video input gstreamer module. Example elements are: v4l2src, videotestsrc, etc...")
    ;

    return desc;
}


void GstVideoInput::parse_options(program_options::variables_map &options)
{

    video_sink_name = "v4l2src";
    if (options.count("video_sink") != 0)
    {
        video_sink_name = options["video_sink"].as<string>();
    }

    width = 640;
    height = 480;

    if (options.count("width") != 0)
    {
        width = options["width"].as<int>();
    }
    switch (width)
    {
    case 320:
        height = 240;
        break;
    case 640:
        height = 480;
        break;
    case 800:
        height = 600;
        break;

    default:
        throw std::runtime_error("GstVideoInput received and unmanaged video width value");
    }


    // FIXME how to get the depth from the image type ?
    //GstVideoInput::image_t::point_t
    //GstVideoInput::image_t::value_t
    depth = 24; // 8 bits RGB

    return;
}

GstVideoInput::GstVideoInput(program_options::variables_map &options)
{

    image_is_new = false;
    pipeline = NULL;

    parse_options(options);

// FIXME just for debugging
    //current_image_p.reset(new GstVideoInput::image_t(width, height));
    //current_image_view = boost::gil::const_view(*current_image_p);


    setup_video_input_pipeline(video_sink_name);

    // launch video_input_thread in a thread

    bool glib_thread_initialized = Glib::thread_supported ();
    if (glib_thread_initialized == false)
    {
        Glib::thread_init();
    }

    const bool video_input_thread_is_joinable = false;
    sigc::slot<void> thead_slot = sigc::mem_fun(*this, &GstVideoInput::video_input_thread);
    Glib::Thread::create( thead_slot , video_input_thread_is_joinable);
    return;
}


GstVideoInput::~GstVideoInput()
{
    // nothing to do here

    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
    gst_object_unref(pipeline);

    return;
}

void GstVideoInput::setup_video_input_pipeline(const string &video_sink_name)
{


    // the following pipeline works on my laptop
    // gst-launch-0.10 v4l2src ! video/x-raw-yuv, width=640,height=480 ! xvimagesink

    GstElement *camera_source, *tee, *videosink_queue, *fakesink_queue, *color_space, *videosink, *fakesink;
    //GstPad *pad;
    GstCaps *videosink_capabilities, *color_space_capabilities;

    gboolean link_ok;

    int argc = 0;
    gst_init(&argc, NULL);

    // pipeline element
    pipeline = GST_PIPELINE(gst_pipeline_new("test-camera"));

    // the camera
    camera_source = gst_element_factory_make(video_sink_name.c_str(), "camera_source");

    // tee
    tee = gst_element_factory_make("tee", "tee");

    // queue for video sink
    videosink_queue = gst_element_factory_make("queue", "videosink_queue");

    // queue  for fake sink
//    fakesink_queue = gst_element_factory_make("queue ! ffmpegcolorspace ! video/x-raw-rgb !", "fakesink_queue");
    fakesink_queue = gst_element_factory_make("queue", "fakesink_queue");

    // color space transform
    color_space = gst_element_factory_make("ffmpegcolorspace", "color_space");

    // the screen sink
    videosink = gst_element_factory_make("xvimagesink", "videosink");

    // fake sink to capture buffer
    fakesink = gst_element_factory_make("fakesink", "fakesink");

    gst_bin_add_many(GST_BIN(pipeline), camera_source, tee, videosink_queue, fakesink_queue, color_space, videosink, fakesink, NULL);


    videosink_capabilities = gst_caps_new_simple(
                                 "video/x-raw-yuv",
                                 "width", G_TYPE_INT, width, "height", G_TYPE_INT, height,
                                 //"framerate", GST_TYPE_FRACTION, 15, 1,
                                 "bpp", G_TYPE_INT, depth, "depth", G_TYPE_INT, depth,
                                 NULL);

    // Camera -> Tee
    link_ok = gst_element_link_filtered(camera_source, tee, videosink_capabilities);
    if (!link_ok)
    {
        g_warning("Failed to link elements !");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the tee element");
    }


    // Tee -> Queue > Videosink
    link_ok = gst_element_link(tee, videosink_queue);
    link_ok = link_ok && gst_element_link(videosink_queue, videosink);
    if (!link_ok)
    {
        g_warning("Failed to link elements!");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the videosink element");
    }


    color_space_capabilities = gst_caps_new_simple(
                                   "video/x-raw-rgb",
                                   "bpp", G_TYPE_INT, depth, "depth", G_TYPE_INT, depth,
                                   "width", G_TYPE_INT, width, "height", G_TYPE_INT, height,
                                   NULL);

    // Tee -> Queue -> ColorSpace -> Fakesink
    link_ok = gst_element_link(tee, fakesink_queue);
    //link_ok = link_ok && gst_element_link(fakesink_queue, fakesink);
    link_ok = link_ok && gst_element_link(fakesink_queue, color_space);
    link_ok = link_ok && gst_element_link_filtered(color_space, fakesink, color_space_capabilities);
    //link_ok = link_ok && gst_element_link_filtered(fakesink_queue, color_space, color_space_capabilities);
    //link_ok = link_ok && gst_element_link(color_space, fakesink);
    if (!link_ok)
    {
        g_warning("Failed to link elements!");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the fakesink element");
    }

    // As soon as screen is exposed, window ID will be advised to the sink
    //g_signal_connect(screen, "expose-event", G_CALLBACK(expose_cb), videosink);

    g_object_set(G_OBJECT(fakesink), "signal-handoffs", TRUE, NULL);
    g_signal_connect(fakesink, "handoff", G_CALLBACK(GstVideoInput::on_new_frame_callback), this);


    return;
}


void GstVideoInput::video_input_thread()
{

    GMainLoop * loop = g_main_loop_new(NULL, FALSE);

    const GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {

        GstMessage * msg;
        g_print("Failed to start up pipeline!\n");
        msg = gst_bus_poll(gst_pipeline_get_bus(pipeline), GST_MESSAGE_ERROR, 0);
        if (msg)
        {
            GError * err = NULL;
            gst_message_parse_error(msg, &err, NULL);
            g_print("ERROR: %s\n", err->message);
            g_error_free(err);
            gst_message_unref(msg);
        }
        throw std::runtime_error("VideoInputApplication::init_video_input failed to start up pipeline!");
    }

    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return;
}



void GstVideoInput::on_new_frame_callback(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer self_p)
{

    ((GstVideoInput *) self_p)->on_new_frame(element, buffer, pad);

    return;
}

void GstVideoInput::on_new_frame(GstElement *element, GstBuffer * buffer, GstPad* pad)
{

    // create buffer view
    const uint8_t *data_p = (uint8_t *) GST_BUFFER_DATA(buffer);
    assert(depth == 24);
    const ptrdiff_t row_size = width*3;
    const size_t buffer_size = GST_BUFFER_SIZE(buffer);
    const size_t height =  buffer_size / row_size;

    typedef boost::gil::rgb8c_view_t buffer_view_t;
    typedef boost::gil::rgb8c_ptr_t buffer_pixel_ptr_t;

    buffer_view_t buffer_view =
        boost::gil::interleaved_view<boost::gil::rgb8c_ptr_t>(static_cast<size_t>(width), height,
                reinterpret_cast<buffer_pixel_ptr_t>(data_p), row_size);


    if (current_image_p.get() == NULL)
    {
        // lazy initialization
        current_image_p.reset(new GstVideoInput::image_t(width, height));
        current_image_view = boost::gil::const_view(*current_image_p);
    }


    if (false)
    {
        // just for debugging
        gint pad_width, pad_height;
        gst_video_get_size(pad, &pad_width, &pad_height);
        printf("Buffer_size / (height * 3) == %i. Pad size (%i, %i)\n", buffer_size / row_size, pad_width, pad_height);
    }

    {
        // copy the buffer into *current_image_p
        //boost::lock_guard<boost::mutex>
        boost::mutex::scoped_lock image_lock(current_image_mutex);
        copy_pixels(buffer_view, view(*current_image_p));
        image_is_new = true;
    }
    new_image_condition.notify_all();


    if (false)
    {
        g_debug("on_new_frame was called");
    }

    return;
}

	const point2<int> &GstVideoInput::get_image_dimensions() {
		
		return	current_image_view.dimensions();
	}

   void get_new_image(rgb8_view_t &view){
	get_new_image<rgb8_view_t > (view);
	return;   
   }

   void get_new_image(gray8_view_t &view){
	get_new_image<gray8_view_t > (view);
	return;   
   }

	template<typename ImageView>
	void GstVideoInput::get_new_image(ImageView &view) {
    //boost::unique_lock<boost::mutex>
    boost::mutex::scoped_lock lock(current_image_mutex);
    while (image_is_new == false)
    {
        // wait until a new image has arrived
        new_image_condition.wait(lock);
    }

	copy_and_convert_pixels(current_image_view, view);

    image_is_new = false;
	return;	
	}

}