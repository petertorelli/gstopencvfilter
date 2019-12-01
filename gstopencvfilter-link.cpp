#include <cv.h>

int
openCvInterface(int height, int width, char *i, char* o) {
	cv::Mat img (height, width, CV_8UC4, i);
	cv::Mat dst (height, width, CV_8UC4, o);
	cv::Mat img2, src, src_gray, grad_x, grad_y, abs_grad_x, abs_grad_y;
	cv::GaussianBlur(img, src, cv::Size(3, 3), 0, 0);
	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	//cv::cvtColor(src_gray, dst, cv::COLOR_GRAY2BGRA);
	cv::Sobel(src_gray, grad_x, CV_16S, 1, 0, 1 /*ksize*/, 1 /*scale*/, 0 /*delta*/, cv::BORDER_DEFAULT);
	cv::Sobel(src_gray, grad_y, CV_16S, 0, 1, 1 /*ksize*/, 1 /*scale*/, 0 /*delta*/, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, img2);
	cv::cvtColor(img2, dst, cv::COLOR_GRAY2BGRA);
	return 0;
}