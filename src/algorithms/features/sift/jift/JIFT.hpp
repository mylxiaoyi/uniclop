#ifndef _JIFT_H
#define _JIFT_H

// This file is and adaptation of the code wrote by Jun Liu
// http://www.cs.man.ac.uk/~liuja/

#include <vil/vil_image_view.h>
#include <vector>
#include <string>
using namespace std;

//#include "features_detection.hpp"
// forward declaration to avoid including directly features_detectoin.hpp
class SIFTFeature;

// forward declaration to avoid including directly CImg.h
namespace cimg_library
{
template<typename T> struct CImg;
}
using namespace cimg_library;


namespace JIFT
{

class Keypoint
{
public:
    // its location in the image, subpixel precision, so use float
    float xi;
    float yi;

    // its magnitude and orientation
    vector<double> mag;
    vector<double> orien;

    // the scale where it is detected
    unsigned int scale;


    // the trival constructor
    Keypoint () {}
    Keypoint(float x, float y):xi(x),yi(y) {}

    Keypoint(float x, float y, vector<double> const& m, vector<double> const& o, unsigned int s)
            : xi(x), yi(y), mag(m), orien(o), scale(s) {}

};

// the main class for JIFT operation
class JIFT
{
public:
    JIFT( unsigned int octaves = 4, unsigned int intervals = 2);
    void init();
    ~JIFT();
    void doJIFT(const vil_image_view<vxl_byte>& srcimg);
    const vector<SIFTFeature> &descriptor() const;
    const vector<Keypoint>  &keypoint() const;

private:
    void buildPyramid(const vil_image_view<vxl_byte>& srcimg);
    void detectLocalExtrema(void);
    void assignOrientation(void);
    void extractKeypointDescriptor(void);

private:

    // octaves and intervals, oriented from SIFT
    unsigned int m_octaves;
    unsigned int m_intervals;

    // gaussian pyramid
    vil_image_view<double> ** m_glist;

    // difference of gaussian pyramid
    vil_image_view<double> ** m_doglist;

    // extrema
    vil_image_view<bool>   ** m_extrema;

    // absolute sigma
    double ** m_abssigmas;

    // number of keypoints
    unsigned int m_kpnum;

    vector<Keypoint>   m_kps;
    vector<SIFTFeature> m_desc;
};

} // end of namespace JIFT

#endif
