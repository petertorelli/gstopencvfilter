#include <opencv2/opencv.hpp>

int
openCvInterface(int height, int width, uint8_t *i, uint8_t *o) {
	/*
	cv::Mat img (height, width, CV_8UC4, i);
	cv::Mat dst (height, width, CV_8UC4, o);
	img.copyTo(dst);
	cv::Mat img2, src, src_gray, grad_x, grad_y, abs_grad_x, abs_grad_y;
	cv::GaussianBlur(img, src, cv::Size(3, 3), 0, 0);
	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	cv::Sobel(src_gray, grad_x, CV_16S, 1, 0);
	cv::Sobel(src_gray, grad_y, CV_16S, 0, 1);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, img2);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img2, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	for (size_t i=0; i<contours.size(); ++i) {
		cv::Rect r = cv::boundingRect(contours[i]);
		cv::rectangle(dst, r, cv::Scalar(255, 0, 0));
	}
	*/

	cv::Mat img (height, width, CV_8UC4, i);
	cv::Mat dst (height, width, CV_8UC4, o);
	img.copyTo(dst);
	cv::Mat img2;
	cv::cvtColor(img, img2, cv::COLOR_BGRA2GRAY);
	cv::threshold(img2, img2, 128, 255, cv::THRESH_BINARY);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img2, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	//cv::drawContours(dst, contours, -1, cv::Scalar(255, 0, 0));
	for (size_t i=0; i<contours.size(); ++i) {
		cv::Rect r = cv::boundingRect(contours[i]);
		double area = cv::contourArea(contours[i]);
		if (area > 100) {
			cv::rectangle(dst, r, cv::Scalar(255, 0, 0));
		}
	}

	/*
	cv::Mat img (height, width, CV_8UC4, i);
	cv::Mat dst (height, width, CV_8UC4, o);
	cv::Mat img2, src, src_gray, grad_x, grad_y, abs_grad_x, abs_grad_y;
	cv::GaussianBlur(img, src, cv::Size(3, 3), 0, 0);
	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	cv::Sobel(src_gray, grad_x, CV_16S, 1, 0);
	cv::Sobel(src_gray, grad_y, CV_16S, 0, 1);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, img2);
	cv::cvtColor(img2, dst, cv::COLOR_GRAY2BGRA);
	*/

	return 0;
}