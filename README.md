Motiviation:

1. I needed a write an RTSP server consisting of mutliple UDP video feeds that be inferenced at 30fps using non-hardware-specific libraries (e.g, DeepStream).

2. There are roughly 50,000+ OpenCV "streaming" projects that dribble out >1 fps using CV2/Python/Flask

3. OpenCV doesn't support writing to a video stream (AFAIK, please let me know if I'm wrong!)

4. GStreamer is the de-facto tool for working at high framerates.

This drop-in element lets you call into OpenCV, do your magic, and write back out ot GStreamer. I included the standard 1-kernel Sobel filter just to get you up and running.

This was compiled on Ubuntu 16.04, GStreamer 1.17.0, OpenCV 2.4.9.1, g++ 5.4.0.
