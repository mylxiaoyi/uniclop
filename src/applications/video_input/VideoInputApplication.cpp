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

void VideoInputApplication::on_new_frame_callback(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer self_p){
	
	((VideoInputApplication *) self_p)->on_new_frame(element, buffer, pad);
	
	return;
}

void VideoInputApplication::on_new_frame(GstElement *element, GstBuffer * buffer, GstPad* pad){
	
	// here goes the video processing
	
	// unsigned char *data_photo = (unsigned char *) GST_BUFFER_DATA(buffer);
	
	
	if (true)
	{
		g_debug("on_new_frame was called");
	}
	
	return;
}


void VideoInputApplication::init_video_input()
{

	// the following pipeline works on my laptop
	// gst-launch-0.10 v4l2src ! video/x-raw-yuv, width=640,height=480 ! xvimagesink

	GstElement *camera_source, *tee, *videosink_queue, *fakesink_queue, *videosink, *fakesink;
	//GstPad *pad;
	GstCaps *filter;

	GstPipeline *pipeline;

	gboolean link_ok;

	GMainLoop * loop;
	loop = g_main_loop_new(NULL, FALSE);

	int argc = 0;
	gst_init(&argc, NULL);

	// pipeline element
	pipeline = GST_PIPELINE(gst_pipeline_new("test-camera"));

	// the camera
	camera_source = gst_element_factory_make("v4l2src", "camera_source");

	// tee
	tee = gst_element_factory_make("tee", "tee");

	// queue for video sink
	videosink_queue = gst_element_factory_make("queue", "videosink_queue");

	// queue  for fake sink
	fakesink_queue = gst_element_factory_make("queue", "fakesink_queue");

	// the screen sink
	videosink = gst_element_factory_make("xvimagesink", "videosink");

	// fake sink to capture buffer
	fakesink = gst_element_factory_make("fakesink", "fakesink");

	gst_bin_add_many(GST_BIN(pipeline), camera_source, tee, videosink_queue, fakesink_queue, videosink, fakesink, NULL);

	filter = gst_caps_new_simple("video/x-raw-yuv", "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, /*"framerate",
	 GST_TYPE_FRACTION, 15, 1,*/"bpp", G_TYPE_INT, depth, "depth", G_TYPE_INT, depth, NULL);

	// Camera -> Tee
	link_ok = gst_element_link_filtered(camera_source, tee, filter);
	if (!link_ok)
	{
		g_warning("Failed to link elements !");
		throw std::runtime_error("VideoInputApplication::init_video_input failed to link the tee element");
	}


	// Tee -> Queue1 -> Videosink
	link_ok = gst_element_link(tee, videosink_queue);
	link_ok = link_ok && gst_element_link(videosink_queue, videosink);
	if (!link_ok)
	{
		g_warning("Failed to link elements!");
		throw std::runtime_error("VideoInputApplication::init_video_input failed to link the videosink element");
	}

	// Tee -> Queue2 -> Fakesink
	link_ok = gst_element_link(tee, fakesink_queue);
	link_ok = link_ok && gst_element_link(fakesink_queue, fakesink);
	if (!link_ok)
	{
		g_warning("Failed to link elements!");
		throw std::runtime_error("VideoInputApplication::init_video_input failed to link the fakesink element");
	}

	// As soon as screen is exposed, window ID will be advised to the sink
	//g_signal_connect(screen, "expose-event", G_CALLBACK(expose_cb), videosink);

	g_object_set(G_OBJECT(fakesink), "signal-handoffs", TRUE, NULL);
	g_signal_connect(fakesink, "handoff", G_CALLBACK(VideoInputApplication::on_new_frame_callback), this);

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

 	width = 640;
 	 height = 480;
 	  depth = 24;
 
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
