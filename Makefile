CC=g++
CPPFLAGS=-Wall -fPIC -fpermissive $(shell pkg-config --cflags gstreamer-1.0 gstreamer-video-1.0 opencv)
gstopencvfilter.so : gstopencvfilter.o gstopencvfilter-link.o
	$(CC) -shared -o $@ $? $(shell pkg-config --libs gstreamer-1.0 gstreamer-video-1.0 opencv)
test : gstopencvfilter.so
	$(shell GST_DEBUG='opencvfilter:5' GST_PLUGIN_PATH=$(PWD) gst-launch-1.0 -e -v videotestsrc  ! opencvfilter ! videoconvert ! ximagesink) 
check : gstopencvfilter.so
	$(shell GST_PLUGIN_PATH=$(PWD) gst-launch-1.0 -e -v videotestsrc ! opencvfilter ! videoconvert ! ximagesink) 
clean: 
	rm *.o
	rm *.so
