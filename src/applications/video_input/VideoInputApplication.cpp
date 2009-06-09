#include "VideoInputApplication.hpp"

#include "devices/video/ImagesInput.hpp"

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>

// using C code because gstreamermm was too much paint to install
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

namespace uniclop
{
namespace applications
{
namespace video_input
{

using namespace uniclop::devices::video;

string VideoInputApplication::get_application_title() const
{

	return "Video input. A simple test application. Uniclop 2009";
}

args::options_description VideoInputApplication::get_command_line_options(void) const
{

	args::options_description desc;

	desc.add(ImagesInput<uint8_t>::get_options_description());

	return desc;
}

int picture_requested = 0;
static gboolean cb_have_data(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer user_data)
{
	//unsigned char *data_photo = (unsigned char *) GST_BUFFER_DATA(buffer);
	if (picture_requested)
	{
		picture_requested = 0;
		//create_jpeg(data_photo);
		// create view and start processing
	}
	return TRUE;
}

void VideoInputApplication::init_video_input()
{

	GstElement  *src, *csp, *csp2, *tee, *queue1, *queue2, *videosink, *fakesink;
	//GstPad *pad;
	GstCaps *filter;

	GstPipeline *pipeline;

	gboolean link_ok;

	GMainLoop * loop;
	loop = g_main_loop_new(NULL, FALSE);

	int argc = 0;
	gst_init(&argc, NULL);

	// Pipeline elements
	pipeline = GST_PIPELINE(gst_pipeline_new("test-camera"));

	// The camera
	src = gst_element_factory_make("v4l2src", "src");

	// Filters
	csp = gst_element_factory_make("ffmpegcolorspace", "csp");
	csp2 = gst_element_factory_make("ffmpegcolorspace", "csp2");

	// Tee
	tee = gst_element_factory_make("tee", "tee");

	// Queue 1 for video sink
	queue1 = gst_element_factory_make("queue", "queue1");

	// Queue 2 for video sink
	queue2 = gst_element_factory_make("queue", "queue2");

	// The screen sink
	videosink = gst_element_factory_make("xvimagesink", "videosink");

	// Fake sink to capture buffer
	fakesink = gst_element_factory_make("fakesink", "fakesink");

	gst_bin_add_many(GST_BIN(pipeline), src, csp, csp2, tee, queue1, queue2, videosink, fakesink, NULL);

	filter = gst_caps_new_simple("video/x-raw-rgb", "width", G_TYPE_INT, 640, "height", G_TYPE_INT, 480, "framerate",
			GST_TYPE_FRACTION, 15, 1, "bpp", G_TYPE_INT, 24, "depth", G_TYPE_INT, 24, NULL);

	// Camera -> Colorspace Filter -> Tee
	gst_element_link(src, csp);
	link_ok = gst_element_link_filtered(csp, tee, filter);

	if (!link_ok)
	{
		g_warning("Failed to link elements !");
		throw std::runtime_error("VideoInputApplication::init_video_input failed to link the Gstreamer elements");
	}

	filter = gst_caps_new_simple("video/x-raw-yuv", NULL);

	// Tee -> Queue1 -> Colorspace Filter -> Videosink
	gst_element_link(tee, queue1);
	gst_element_link(queue1, csp2);
	link_ok = gst_element_link_filtered(csp2, videosink, filter);

	if (!link_ok)
	{
		g_warning("Failed to link elements!");
		throw std::runtime_error("VideoInputApplication::init_video_input failed to link the Gstreamer elements");
	}

	// Tee -> Queue2 -> Fakesink
	gst_element_link(tee, queue2);
	gst_element_link(queue2, fakesink);

	// As soon as screen is exposed, window ID will be advised to the sink
	//g_signal_connect(screen, "expose-event", G_CALLBACK(expose_cb), videosink);

	g_object_set(G_OBJECT(fakesink), "signal-handoffs", TRUE, NULL);
	g_signal_connect(fakesink, "handoff", G_CALLBACK(cb_have_data), NULL);

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
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(pipeline);

	return;
}

int VideoInputApplication::main_loop(args::variables_map &options)
{
	printf("VideoInputApplication::main_loop says hello world !\n");

	//init_gui(options);
	//run_gui();

	init_video_input();

	// FIXME should port ImagesInput to Gil
	ImagesInput<uint8_t> images_input(options);
	const CImg<uint8_t> &current_image = images_input.get_new_image();

	CImgDisplay video_display(current_image.dimx(), current_image.dimy(), "Video stream");
	video_display.show();
	video_display.display(current_image);

	do
	{
		if (true)
		{
			printf(".");
		}

		const CImg<uint8_t> &current_image = images_input.get_new_image();
		video_display.display(current_image);

		const float seconds_to_wait = 0.1; // [seconds]
		boost::xtime t_time;
		boost::xtime_get(&t_time, boost::TIME_UTC);
		t_time.nsec += (1000 * 1000) * seconds_to_wait;
		boost::thread::sleep(t_time);

	} while (video_display.is_closed == false);

	return 0;
}

}
}
}
