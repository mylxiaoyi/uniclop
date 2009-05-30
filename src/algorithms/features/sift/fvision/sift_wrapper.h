#pragma once

/*
Detects SIFT features in two images and finds matches between them.

Copyright (C) 2008  Jin Zhou <ferryzhou@gmail.com>

*/

#include <cv.h>

#include <vector>

/********************************** Main *************************************/

void sift_compute(const IplImage* im1, 
				  const IplImage* im2, 
				  std::vector<CvPoint2D32f>& x1s, 
				  std::vector<CvPoint2D32f>& x2s);

