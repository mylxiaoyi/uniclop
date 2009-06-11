#! /usr/bin/env python
# encoding: utf-8

env = Environment()

env.ParseConfig("pkg-config --cflags --libs gstreamer-0.10")
env.ParseConfig("pkg-config --cflags --libs gtk+-2.0")

# building video_input 

source_files =  Glob("./src/applications/video_input/*.cpp")
source_files +=  Glob("./src/applications/*.cpp")
source_files +=  Glob("./src/devices/video/*.cpp")

print(source_files)

env.Append(CPPPATH = Dir("./src") )
env.Append(LIBSPATH = Dir("/usr/X11R6/lib") )

libs = ["boost_program_options", "boost_filesystem", "boost_thread", 
"Xext", "Xrandr", "jpeg", "png", "avcodec", "avformat", "cv", "cvaux", "highgui", "cxcore",
]

env.Append(LIBS = libs)

env.Program("video_input", source_files)
