# export PKG_CONFIG_PATH=$HOME/dev/gstreamer/lib/pkgconfig:$PKG_CONFIG_PATH

CC=g++
CPPFLAGS=-std=c++11 -Wall -fPIC -fpermissive $(shell pkg-config --cflags gstreamer-1.0 opencv4)
CFLAGS=-Wall -fPIC -fpermissive $(shell pkg-config --cflags gstreamer-1.0 opencv4)
$(info $(CPPFLAGS))
gstopencvfilter.so : gstopencvfilter.o gstopencvfilter-link.o
	$(CC) -shared -o $@ $? $(shell pkg-config --libs gstreamer-1.0 gstreamer-video-1.0 opencv4)
test : gstopencvfilter.so
	$(shell GST_DEBUG='opencvfilter:5' GST_PLUGIN_PATH=$(PWD) gst-launch-1.0 -e -v videotestsrc  ! opencvfilter ! videoconvert ! ximagesink) 
check : gstopencvfilter.so
	$(shell GST_PLUGIN_PATH=$(PWD) gst-launch-1.0 -e -v videotestsrc ! opencvfilter ! videoconvert ! ximagesink) 
clean: 
	rm -f *.o
	rm -f *.so
