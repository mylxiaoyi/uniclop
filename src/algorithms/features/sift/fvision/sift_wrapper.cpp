#include "sift_wrapper.h"

extern "C" {
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
}

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>

#include <iostream>

using namespace std;

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

bool isPointNotIn(const CvPoint2D32f& x, const vector<CvPoint2D32f>& xs, double epsilon = 1e-5) {
	for (size_t i = 0; i < xs.size(); i++) {
		double dist = abs(x.x - xs[i].x) + abs(x.y - xs[i].y);
		if (dist < epsilon) return false;
	}
	return true;
}

void removeDuplicatePoints(const vector<CvPoint2D32f>& x1s, 
						   const vector<CvPoint2D32f>& x2s, 
						   vector<CvPoint2D32f>& nx1s, 
						   vector<CvPoint2D32f>& nx2s) 
{
	nx1s.clear();
	nx2s.clear();
	for (size_t i = 0; i < x1s.size(); i++) {
		CvPoint2D32f x1 = x1s[i];
		CvPoint2D32f x2 = x2s[i];
		if (isPointNotIn(x1, nx1s)) {
			nx1s.push_back(x1);
			nx2s.push_back(x2);
		}
	}
}

void sift_compute(const IplImage* im1, const IplImage* im2, vector<CvPoint2D32f>& x1s, vector<CvPoint2D32f>& x2s) {

	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	double d0, d1;
	int n1, n2, k, i, m = 0;

	IplImage* nim1 = cvCloneImage(im1);
	IplImage* nim2 = cvCloneImage(im2);

	cout<<"Finding features in first image..."<<endl;
	n1 = sift_features( nim1, &feat1 );
	cout<<"Finding features in second image..."<<endl;
	n2 = sift_features( nim2, &feat2 );
	cout<<"n1: "<<n1<<endl;
	cout<<"n2: "<<n1<<endl;

	vector<CvPoint2D32f> ox1s, ox2s;

	kd_root = kdtree_build( feat2, n2 );
	for( i = 0; i < n1; i++ ) {
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 ) {
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR ) {
				ox1s.push_back(cvPoint2D32f(feat->x, feat->y));
				ox2s.push_back(cvPoint2D32f(nbrs[0]->x, nbrs[0]->y));
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	removeDuplicatePoints(ox1s, ox2s, x1s, x2s);
	cout<<"matched "<<x1s.size()<<endl;

	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );

	cvReleaseImage(&nim1);
	cvReleaseImage(&nim2);
}