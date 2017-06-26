
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



#include "SignHandler.h"


using namespace cv;
using namespace std;


CSignImage::CSignImage()
{
}


CSignImage::CSignImage(int id, string filename, string action, CSignHandler * handler)
{
	m_img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	m_id = id;
	if (!m_img.data)
	{
		printf("file open error \n");
	}
	threshold(m_img, m_img, 100, 255, 0);
	m_actionname = action;
	m_handler = handler;
}



CSignImage::~CSignImage()
{
}

int CSignImage::GetDiff(Mat & cam_sign)
{
	Mat diffImg;
	bitwise_xor(cam_sign, m_img, diffImg,	noArray());
	return countNonZero(diffImg);
}

void CSignImage::CallHandler()
{
	if (m_handler)
		m_handler->onSignAction(m_id, m_actionname);
}


