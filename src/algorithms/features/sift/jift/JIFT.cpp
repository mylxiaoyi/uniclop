/*******************************************************************************
 *
 * FILENAME:     JIFT.cpp
 * DESCRIPTION:  Jun's invariant feature transformation (JIFT)
 *
 * AUTHOR:       Jun Liu (jun@cs.man.ac.uk)
 * INSTITUTION:  Advanced Interfaces Group,
 *               Department of Computer Science,
 *               the University of Manchester, United Kingdom
 * DATE:         August 2005
 *
 *******************************************************************************/

// This file is and adaptation of the code wrote by Jun Liu
// http://www.cs.man.ac.uk/~liuja/

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers
#include "JIFT.hpp"
#include "Gaussian.hpp"

// implementation specific headers
#include "features_detection.hpp"

#include <vil/vil_copy.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_bilin_interp.h>
#include <vil/vil_math.h>
#include <vnl/algo/vnl_matrix_inverse.h>

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Constants  definitions

namespace JIFT
{


const double       ANTIALIAS_SIGMA           = 0.5;
const double       PREBLUR_SIGMA             = 1.0;
const double       CURVATURE_THRESHOLD       = 10.0;
const double       CONTRAST_THRESHOLD        = 0.02;
const unsigned int SQUARE_SIZE               = 3;
const unsigned int NUM_BINS                  = 36;
const unsigned int FEATURE_WINDOW_SIZE       = 16;
const unsigned int DESC_NUM_BINS             = 8;
const double       FV_THRESHOLD              = 0.2;
const unsigned int FVSIZE                    = 128;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class JIFT methods implementation

// the constructor

JIFT::JIFT( unsigned int octaves, unsigned int intervals)
        : m_octaves(octaves), m_intervals(intervals)
{
    init();
}


void JIFT::init()
{
    unsigned int i;

    // allocate spaces for gaussian list, difference of gaussian list extrema and absolute sigma
    m_glist   = new vil_image_view<double>*[m_octaves];
    for (i=0;i<m_octaves;i++)
        m_glist[i] = new vil_image_view<double>[m_intervals+3];

    m_doglist = new vil_image_view<double>*[m_octaves];
    for (i=0;i<m_octaves;i++)
        m_doglist[i] = new vil_image_view<double>[m_intervals+2];

    m_extrema = new vil_image_view<bool> * [m_octaves];
    for (i=0;i<m_octaves;i++)
        m_extrema[i] = new vil_image_view<bool>[m_intervals];

    m_abssigmas = new double*[m_octaves];
    for (i=0;i<m_octaves;i++)
        m_abssigmas[i] = new double[m_intervals+3];

} // end of JIFT::init



// the destructor
JIFT::~JIFT()
{
    unsigned int i;
    for (i=0;i<m_octaves;i++)
    {
        delete [] m_glist[m_octaves-i-1];
        delete [] m_doglist[m_octaves-i-1];
        delete [] m_extrema[m_octaves-i-1];
        delete [] m_abssigmas[m_octaves-i-1];
    }
    delete [] m_glist;
    delete [] m_doglist;
    delete [] m_extrema;
    delete [] m_abssigmas;
}

void JIFT::buildPyramid(const vil_image_view<vxl_byte>& srcimg)
{
    if (false) cout << endl << "Building image pyramid..." << endl;

    // Generate the gaussian and the difference of gaussian pyramids
    // In order to detect key points on s intervals per octave,
    // we must generate s+3 blurred images in the gaussian pyramid.
    // This is because s+3 blurred images generate s+2 DoG images, and two
    // images are needed (one at the highest and one lowest scales of the octave)
    // for extrema detection

    unsigned int i,j,k,t;

    // convert to gray scale image
    vil_image_view<double> gray_scale_image;

    // if it is an rgb colour image
    if (srcimg.nplanes() == 3)
        vil_convert_planes_to_grey(srcimg, gray_scale_image);
    // if this is an gray scale image
    else if (srcimg.nplanes() ==1)
        vil_convert_cast(srcimg, gray_scale_image);


    // vil_convert_cast(this->m_srcimg,gray_scale_image);

    // Blur the image with a standard deviation of 0.5 to prevent aliasing
    // and then upsample the image by a factor of 2 using linear interpolation
    // Lowe claims that this increase the number of stable keypoints by a factor
    // of 4

    // use gaussian filter with sigma = antialias_sigma
    Gaussian::gaussian_filter(gray_scale_image,gray_scale_image,ANTIALIAS_SIGMA);

    // double the size
    m_glist[0][0].set_size(gray_scale_image.ni()*2,gray_scale_image.nj()*2);

    // bilinear interpolation, and normalise (divided by 255.0)
    for (i=0;i<m_glist[0][0].ni();i++)
        for (j=0;j<m_glist[0][0].nj();j++)
            m_glist[0][0](i,j) = vil_bilin_interp_safe_extend(gray_scale_image,
                                 static_cast<double>(i)/2.0,
                                 static_cast<double>(j)/2.0)/255.0;


    // now pre-blur the base image with a sigma of PREBLUR_SIGMA
    Gaussian::gaussian_filter(m_glist[0][0],m_glist[0][0],PREBLUR_SIGMA);

    double init_sigma = sqrt(2.0);

    // Keep track of the absolute sigma
    m_abssigmas[0][0] = init_sigma * 0.5;

    // to construct gaussian scale space
    for (i=0;i<m_octaves;i++)
    {
        double sigma = init_sigma;

        for (j=1;j<m_intervals+3;j++)
        {
            // Compute the standard deviation of the gaussian filter needed to produce
            // the next level of geometrically sampled pyramid.
            // Here sigma_(i+1) = k * sigma
            // By definitiojn of successive convolution, the required blurring sigma_f
            // to produce sigma_(i+1) from sigma_i is
            //
            //        sigma_(i+1)^2 = sigma_f,i^2 + sigma_i^2
            //        (k*sigma_i)^2 = sigma_f,i^2 + sigma_i^2
            //
            // Therefore
            //
            //        sigma_f, i = sqrt(k^2 -1) sigma_i
            //
            // where k = 2^(1/intervals) to span the octave, so
            //
            //        sigma_f, i = sqrt(2^(2/intervals)-1)sigma_i
            double sigma_f = sqrt(pow(2.0,2.0/m_intervals)-1) * sigma;
            sigma = pow(2.0,1.0/m_intervals) * sigma;

            // keep track of absolute sigma
            m_abssigmas[i][j] = sigma * 0.5 * pow(2.0f,static_cast<float>(i));
            Gaussian::gaussian_filter(m_glist[i][j-1],m_glist[i][j],sigma_f);

            // and difference of gaussian pyramid
            m_doglist[i][j-1].set_size(m_glist[i][j].ni(),m_glist[i][j].nj());
            vil_math_image_difference(m_glist[i][j],m_glist[i][j-1],m_doglist[i][j-1]);

        }

        if (i < m_octaves-1)
        {
            // the gaussian image 2 images from the top of the stack for this octave
            // have been blurred by by 2*sigma. Subsample this image by a factor of
            // 2 to produce the first image of the next octave
            m_glist[i+1][0].set_size(m_glist[i][0].ni()/2,m_glist[i][0].nj()/2);
            for (k=0;k<m_glist[i+1][0].ni();k++)
                for (t=0;t<m_glist[i+1][0].nj();t++)
                    m_glist[i+1][0](k,t) = m_glist[i][m_intervals](2*k,2*t);

            m_abssigmas[i+1][0] = m_abssigmas[i][m_intervals];
        }
    }
} // end of JIFT::buildPyramid


void JIFT::detectLocalExtrema(void)
{
    // The next step is to detect local maxima in the DOG pyramid. When
    // a maximum is found, two tests are applied before labeling it as a keypoint.
    // First, it mush have sufficient contrast. Second, it should not be an edge point
    // (i.e. the ration of principal curvation at the extremum should be below a threshold)

    // Compute threshold for the ratio of principle curvature test
    // applied to DoG extrema before classifying them as keypoints

    unsigned int i,j,xi,yi;

    double curvature_ratio, curvature_threshold;
    vil_image_view<double> middle, up, down;
    int scale;
    double dxx, dyy, dxy, trH, detH;

    unsigned int num = 0;

    curvature_threshold = (CURVATURE_THRESHOLD+1)*(CURVATURE_THRESHOLD+1)/CURVATURE_THRESHOLD;

    //detect local extrema in DoG pyramid
    for (i=0;i<m_octaves;i++)
    {
        scale = static_cast<int>(pow(2.0f,static_cast<float>(i)));

        for (j=1;j<m_intervals+1;j++)
        {
            m_extrema[i][j-1].set_size(m_doglist[i][0].ni(),m_doglist[i][0].nj());
            m_extrema[i][j-1].fill(false);

            // fetch the DoG data
            middle = m_doglist[i][j];
            up     = m_doglist[i][j+1];
            down   = m_doglist[i][j-1];

            for (xi=1;xi<m_doglist[i][0].ni()-1;xi++)
                for (yi=1;yi<m_doglist[i][0].nj()-1;yi++)
                {
                    // if it is the local maximum
                    if (middle(xi,yi) > middle(xi,yi-1)   &&
                            middle(xi,yi) > middle(xi,yi+1)   &&
                            middle(xi,yi) > middle(xi-1,yi)   &&
                            middle(xi,yi) > middle(xi+1,yi)   &&
                            middle(xi,yi) > middle(xi-1,yi-1) &&
                            middle(xi,yi) > middle(xi+1,yi-1) &&
                            middle(xi,yi) > middle(xi+1,yi+1) &&
                            middle(xi,yi) > middle(xi-1,yi+1) &&
                            middle(xi,yi) > up(xi,yi)         &&
                            middle(xi,yi) > up(xi,yi-1)       &&
                            middle(xi,yi) > up(xi,yi+1)       &&
                            middle(xi,yi) > up(xi-1,yi)       &&
                            middle(xi,yi) > up(xi+1,yi)       &&
                            middle(xi,yi) > up(xi-1,yi-1)     &&
                            middle(xi,yi) > up(xi+1,yi-1)     &&
                            middle(xi,yi) > up(xi+1,yi+1)     &&
                            middle(xi,yi) > up(xi-1,yi+1)     &&
                            middle(xi,yi) > down(xi,yi)       &&
                            middle(xi,yi) > down(xi,yi-1)     &&
                            middle(xi,yi) > down(xi,yi+1)     &&
                            middle(xi,yi) > down(xi-1,yi)     &&
                            middle(xi,yi) > down(xi+1,yi)     &&
                            middle(xi,yi) > down(xi-1,yi-1)   &&
                            middle(xi,yi) > down(xi+1,yi-1)   &&
                            middle(xi,yi) > down(xi+1,yi+1)   &&
                            middle(xi,yi) > down(xi-1,yi+1)   )

                    {
                        m_extrema[i][j-1](xi,yi) = true;
                        num ++;
                    }

                    // if it is the local minumum

                    else if (middle(xi,yi) < middle(xi,yi-1)   &&
                             middle(xi,yi) < middle(xi,yi+1)   &&
                             middle(xi,yi) < middle(xi-1,yi)   &&
                             middle(xi,yi) < middle(xi+1,yi)   &&
                             middle(xi,yi) < middle(xi-1,yi-1) &&
                             middle(xi,yi) < middle(xi+1,yi-1) &&
                             middle(xi,yi) < middle(xi+1,yi+1) &&
                             middle(xi,yi) < middle(xi-1,yi+1) &&
                             middle(xi,yi) < up(xi,yi)         &&
                             middle(xi,yi) < up(xi,yi-1)       &&
                             middle(xi,yi) < up(xi,yi+1)       &&
                             middle(xi,yi) < up(xi-1,yi)       &&
                             middle(xi,yi) < up(xi+1,yi)       &&
                             middle(xi,yi) < up(xi-1,yi-1)     &&
                             middle(xi,yi) < up(xi+1,yi-1)     &&
                             middle(xi,yi) < up(xi+1,yi+1)     &&
                             middle(xi,yi) < up(xi-1,yi+1)     &&
                             middle(xi,yi) < down(xi,yi)       &&
                             middle(xi,yi) < down(xi,yi-1)     &&
                             middle(xi,yi) < down(xi,yi+1)     &&
                             middle(xi,yi) < down(xi-1,yi)     &&
                             middle(xi,yi) < down(xi+1,yi)     &&
                             middle(xi,yi) < down(xi-1,yi-1)   &&
                             middle(xi,yi) < down(xi+1,yi-1)   &&
                             middle(xi,yi) < down(xi+1,yi+1)   &&
                             middle(xi,yi) < down(xi-1,yi+1)   )
                    {
                        m_extrema[i][j-1](xi,yi) = true;
                        num ++;
                    }

                    // if it is a local extremum,
                    // make sure the extremum is above the contrast threshold
                    if (m_extrema[i][j-1](xi,yi) == true &&
                            fabs(middle(xi,yi)) < CONTRAST_THRESHOLD)
                    {
                        m_extrema[i][j-1](xi,yi) = false;
                        num --;
                    }

                    // compute the entries of the Hessian matrix at the extrema location
                    if (m_extrema[i][j-1](xi,yi) == true)
                    {
                        dxx  =
                            middle(xi,yi-1) +
                            middle(xi,yi+1) -
                            2.0 * middle(xi,yi);

                        dyy  =
                            middle(xi-1,yi) +
                            middle(xi+1,yi) -
                            2.0 * middle(xi,yi);

                        dxy  =
                            (middle(xi-1,yi-1) +
                             middle(xi+1,yi+1) -
                             middle(xi+1,yi-1) -
                             middle(xi-1,yi+1))/4.0 ;

                        // Compute the trace and the determinant of the Hessian
                        trH  = dxx + dyy;
                        detH = dxx*dyy - dxy*dxy;

                        // Compute the ratio of the principal curvatures
                        curvature_ratio = (trH*trH)/detH;
                        if (detH<0 || curvature_ratio>curvature_threshold)
                        {
                            m_extrema[i][j-1](xi,yi) = false;
                            num --;
                        }

                    }
                }
        }
    }

    m_kpnum = num;
    if (false) cout << "Number of keypoints detected: " << m_kpnum << endl;
}



void JIFT::assignOrientation(void)
{
    // This function is to assign orientation to the keypoints.
    // For this, we histogram the gradient orientation over a region about
    // each keypoints


    if (false) cout << endl << "Assigning orientation to keypoints..." << endl;

    m_kps.clear(); // clearing the previous list

    unsigned int i,j,k,xi,yi;
    int kk, tt;

    vil_image_view<double> ** magnitude = new vil_image_view<double> * [m_octaves];
    for (i=0;i<m_octaves;i++)
        magnitude[i] = new vil_image_view<double>[m_intervals];
    vil_image_view<double> ** orientation = new vil_image_view<double> * [m_octaves];
    for (i=0;i<m_octaves;i++)
        orientation[i] = new vil_image_view<double>[m_intervals];



    // Compute the gradient direction and magnitude of
    // the gaussian pyramid images
    for (i=0;i<m_octaves;i++)
        for (j=1;j<m_intervals+1;j++)
        {

            magnitude[i][j-1].set_size(m_glist[i][j].ni(),m_glist[i][j].nj());
            orientation[i][j-1].set_size(m_glist[i][j].ni(),m_glist[i][j].nj());
            // fill the view with value 0
            magnitude[i][j-1].fill(0.0);
            orientation[i][j-1].fill(0.0);



            // fetch the gaussian pyramid image
            for (xi=1; xi<m_glist[i][j].ni()-1; xi++)
                for (yi=1; yi<m_glist[i][j].nj()-1;yi++)
                {
                    // compute x and y derivatives using pixel differences
                    double dx = m_glist[i][j](xi+1,yi) - m_glist[i][j](xi-1,yi);
                    double dy = m_glist[i][j](xi,yi+1) - m_glist[i][j](xi,yi-1);

                    // compute the magnitude and orientation of the gradient
                    magnitude[i][j-1](xi,yi)   = sqrt(dx * dx + dy * dy);
                    orientation[i][j-1](xi,yi) = (atan2(dy,dx)==M_PI)? -M_PI:atan2(dy,dx);
                }
            // zero padding, assign the boundary to 0.0

            /*
              for (xi=0;xi<magnitude[i][j-1].ni();xi++)
              {
              magnitude[i][j-1](xi,0)                          = 0.0;
              magnitude[i][j-1](xi,magnitude[i][j-1].nj()-1)   = 0.0;
              orientation[i][j-1](xi,0)                        = 0.0;
              orientation[i][j-1](xi,magnitude[i][j-1].nj()-1) = 0.0;
              }

              for (yi=0;yi<magnitude[i][j-1].nj();yi++)
              {
              magnitude[i][j-1](0,yi)                          = 0.0;
              magnitude[i][j-1](magnitude[i][j-1].ni()-1,yi)   = 0.0;
              orientation[i][j-1](0,yi)                        = 0.0;
              orientation[i][j-1](magnitude[i][j-1].ni()-1,yi) = 0.0;
              }
            */


        }


    // The next step of the algorithm is to assign orientation to the keypoints
    // that have been located.
    // This is done by looking for peaks in the histograms of gradient orientations in
    // the regions surrounding each keypoint. A keypoint may be assigned more than one
    // orientations. If this is the case, then two identical descriptors
    // are added to the database with different orientations


    vector<double>hist_orient(NUM_BINS);

    for (i=0;i<m_octaves;i++)
    {
        unsigned int scale = static_cast<unsigned int> (pow(2.0f,static_cast<float>(i)));
        unsigned int ni = m_glist[i][0].ni();
        unsigned int nj = m_glist[i][0].nj();

        for (j=1; j<m_intervals+1; j++)
        {
            // first, fetch the image from the absolute sigma
            double abs_sigma = m_abssigmas[i][j];

            // Each sample added to the histogram is weighted by its gradient magnitude
            // and by a Gaussian-weighted circular window with a sigma that is 1.5 times
            // that of the scale of the keypoints
            vil_image_view<double> weight;
            Gaussian::gaussian_filter(magnitude[i][j-1], weight, 1.5*abs_sigma);

            // get the gaussian kernel size
            int hfsz = Gaussian::getGaussianKernelSize(1.5*abs_sigma) / 2 ;


            // iterate over all the keypoints at this octave and orientation
            for (xi=0;xi<ni;xi++)
                for (yi=0;yi<nj;yi++)
                {
                    // if it is a keypoint
                    if (m_extrema[i][j-1](xi,yi)==true)
                    {
                        // histogram the gradient orientations for this keypoint weighted by
                        // the gradient magnitude and the gaussian weighting mask

                        // clear, set the histogram to all 0.0
                        for (k=0;k<NUM_BINS;k++)
                            hist_orient[k] = 0.0;

                        for (kk=-hfsz; kk<=static_cast<int>(hfsz); kk++)
                            for (tt=-hfsz; tt<=static_cast<int>(hfsz); tt++)
                            {
                                // if out of boundary do nothing
                                if (static_cast<int>(xi)+kk<0     ||
                                        static_cast<int>(xi)+kk>=static_cast<int>(ni)   ||
                                        static_cast<int>(yi)+tt<0     ||
                                        static_cast<int>(yi)+tt>=static_cast<int>(nj)  ) continue;

                                //fetch the orientation of sample point
                                double sampleorient = orientation[i][j-1](xi+kk,yi+tt);
                                // note that orient[xx][yy] is within the range [-M_PI,M_PI)
                                // should be normalised to [0, 2*M_PI)

                                assert (sampleorient >=-M_PI && sampleorient < M_PI);
                                sampleorient += M_PI;


                                // convert to degree
                                unsigned sampleorient_d = static_cast<unsigned int>(sampleorient * 180 / M_PI);
                                // accumulate the histogram bins
                                hist_orient[sampleorient_d / (360/NUM_BINS)] += weight(xi+kk,yi+tt);
                            }

                        // find the maximum peak in the orientation histogram
                        double max_peak_val       = hist_orient[0];
                        unsigned int max_peak_idx = 0;
                        for (k=1; k<NUM_BINS; k++)
                            if (hist_orient[k]>max_peak_val)
                            {
                                max_peak_val = hist_orient[k];
                                max_peak_idx = k;
                            }

                        // iterate over all peaks within 80% of the largest peak and add
                        // keypoints with the orientation corresponding to those peaks
                        // to the keypoint list
                        vector<double> orien;
                        vector<double> mag;

                        for (k=0; k<NUM_BINS; k++)
                            if (hist_orient[k] > 0.8 * max_peak_val)
                            {
                                // if this is the maxmium peak
                                // interpolate the peak by fitting a parabola to the three
                                // histogram values closest to each peak, for better accuracy
                                // Parabola??? oh my god ...
                                double x1 = static_cast<double>(k-1);
                                double y1;
                                double x2 = static_cast<double>(k);
                                double y2 = hist_orient[k];
                                double x3 = static_cast<double>(k+1);
                                double y3;

                                if (k==0)
                                {
                                    y1 = hist_orient[NUM_BINS-1];
                                    y3 = hist_orient[1];
                                }
                                else if (k==NUM_BINS-1)
                                {
                                    y1 = hist_orient[NUM_BINS-2];
                                    y3 = hist_orient[0];
                                }
                                else
                                {
                                    y1 = hist_orient[k-1];
                                    y3 = hist_orient[k+1];
                                }

                                // A downward parabola has the general form

                                // y = a * x^2 + bx + c

                                // Now the three equations stem from the three points
                                // (x1,y1) (x2,y2) (x3.y3) are

                                // y1 = a * x1^2 + b * x1 + c
                                // y2 = a * x2^2 + b * x2 + c
                                // y3 = a * x3^2 + b * x3 + c

                                // in Matrix notation, this is y = Xb, where
                                // y = (y1 y2 y3)' b = (a b c)' and
                                //
                                //     x1^2 x1 1
                                // X = x2^2 x2 1
                                //     x3^2 x3 1
                                //
                                // OK, we need to solve this equation for b
                                // this is done by inverse the matrix X
                                //
                                // b = inv(X) Y
                                vnl_vector<double> b(3);
                                vnl_matrix<double> X(3,3);

                                X(0,0) = x1*x1;
                                X(0,1) = x1;
                                X(0,2) = 1;
                                X(1,0) = x2*x2;
                                X(1,1) = x2;
                                X(1,2) = 1;
                                X(2,0) = x3*x3;
                                X(2,1) = x3;
                                X(2,2) = 1;

                                X = vnl_matrix_inverse<double>(X);

                                b[0] = X(0,0)*y1 + X(0,1)*y2 + X(0,2)*y3;
                                b[1] = X(1,0)*y1 + X(1,1)*y2 + X(1,2)*y3;
                                b[2] = X(2,0)*y1 + X(2,1)*y2 + X(2,2)*y3;

                                // the vertex of parabola is (-b/(2a),c-b^2/4a)
                                double x0 = -b[1]/(2*b[0]);


                                // abnormal situation
                                if (fabs(x0)>2*NUM_BINS)
                                    x0 = x2;


                                // make sure it is within the range
                                while (x0 < 0)
                                    x0 = x0 + NUM_BINS;
                                while (x0 >= NUM_BINS)
                                    x0 = x0 - NUM_BINS;

                                // normalise
                                double x0_n = x0*(2*M_PI/NUM_BINS);
                                // since x0_n is [0,2*M_PI) we should set it within the range [-M_PI,M_PI)

                                assert (x0_n>=0 && x0_n<2*M_PI);
                                x0_n -= M_PI;
                                assert (x0_n >= -M_PI && x0_n < M_PI);

                                // store the keypoint orientations
                                // allow multiple orientations
                                orien.push_back(x0_n);
                                mag.push_back(hist_orient[k]);
                            }

                        m_kps.push_back(
                            Keypoint(static_cast<float>(xi*scale)/2.0,  // x position
                                     static_cast<float>(yi*scale)/2.0,  // y position
                                     mag,                               // magnitude
                                     orien,                             // orientation
                                     i*m_intervals+j-1)                 // scale
                        );
                    }

                }

        }

    }


    assert (m_kps.size() == m_kpnum);

    // free the memory space
    for (i=0;i<m_octaves;i++)
    {
        delete [] magnitude[m_octaves-i-1];
        delete [] orientation[m_octaves-i-1];
    }
    delete [] magnitude;
    delete [] orientation;



}

void JIFT::extractKeypointDescriptor(void)
{
    if (false) cout << endl << "Extracting feature descriptors..." << endl;

    m_desc.clear(); // remove previous ones

    // The final of the SIFT algorithm is to extract feature descriptors for the keypoints
    // The descriptors are a grid of gradient orientation histograms, where the sampling
    // grid for the histograms is rotated to the main orientation of each keypoint.
    // The grid is a 4x4 array of 4x4 sample cells of a bin orientation histograms.
    // This produces 128 dimensional feature vectors

    // First, compute the gradient magnitude and orientation at the sample point (X.5)
    // As the sample point is not an integer number, it should be interpolated using
    // bilinear interpolation


    unsigned int i,j;
    vil_image_view<double> ** InterpolatedMagnitude   = new vil_image_view<double>*[m_octaves];
    vil_image_view<double> ** InterpolatedOrientation = new vil_image_view<double>*[m_octaves];
    for (i=0;i<m_octaves;i++)
    {
        InterpolatedMagnitude[i]   = new vil_image_view<double>[m_intervals];
        InterpolatedOrientation[i] = new vil_image_view<double>[m_intervals];
    }


    for (i=0; i<m_octaves; i++)
        for (j=1; j<m_intervals+1; j++)
        {
            // fetch the width and height of the pyramid image
            unsigned int ni = m_glist[i][j].ni();
            unsigned int nj = m_glist[i][j].nj();

            // vcl_cout << "ni = " << ni << ", nj = " << nj << vcl_endl;

            // note that n grids will have n+1 conjunctions
            InterpolatedMagnitude[i][j-1].set_size(ni+1,nj+1);
            InterpolatedOrientation[i][j-1].set_size(ni+1,nj+1);

            // starting from 0.5
            for (float ii=0.5;ii<ni-0.5;ii++)
                for (float jj=0.5;jj<nj-0.5;jj++)
                {
                    //compute x and y derivatives using pixel differences
                    double dx =  vil_bilin_interp_safe_extend(m_glist[i][j],ii+1.0,jj)
                                 - vil_bilin_interp_safe_extend(m_glist[i][j],ii-1.0,jj);
                    double dy =  vil_bilin_interp_safe_extend(m_glist[i][j],ii,jj+1.0)
                                 - vil_bilin_interp_safe_extend(m_glist[i][j],ii,jj-1.0);
                    unsigned int iii = static_cast<unsigned int>(ii+1.0);
                    unsigned int jjj = static_cast<unsigned int>(jj+1.0);

                    // vcl_cout << "iii = " << iii << ", jjj = " << jjj << vcl_endl;
                    assert (iii <= ni && jjj <= nj);

                    InterpolatedMagnitude[i][j-1](iii,jjj)   = sqrt(dx*dx + dy*dy);
                    // vcl_cout << InterpolatedMagnitude[i][j-1](iii,jjj) << vcl_endl;
                    InterpolatedOrientation[i][j-1](iii,jjj) = (atan2(dy,dx)==M_PI)? -M_PI:atan2(dy,dx);
                    // vcl_cout << InterpolatedOrientation[i][j-1](iii,jjj) << vcl_endl;
                }

            // zero padding, assign the boundary to 0.0
            for (unsigned int iii=0;iii<ni+1;iii++)
            {
                InterpolatedMagnitude[i][j-1](iii,0)      = 0.0;
                InterpolatedMagnitude[i][j-1](iii,nj)     = 0.0;
                InterpolatedOrientation[i][j-1](iii,0)    = 0.0;
                InterpolatedOrientation[i][j-1](iii,nj)   = 0.0;
            }
            for (unsigned int jjj=0; jjj<nj+1; jjj++)
            {
                InterpolatedMagnitude[i][j-1](0,jjj)      = 0.0;
                InterpolatedMagnitude[i][j-1](ni,jjj)     = 0.0;
                InterpolatedOrientation[i][j-1](0,jjj)    = 0.0;
                InterpolatedOrientation[i][j-1](ni,jjj)   = 0.0;
            }

        }

    // Then, build a Interpolated gaussian table of size FEATURE_WINDOW_SIZE
    // Lowe suggest the sigma to be one half of the window width
    vnl_matrix<double> G = Gaussian::buildInterpolatedGaussianTable(FEATURE_WINDOW_SIZE,0.5*FEATURE_WINDOW_SIZE);

    // the histogram
    vector<double> hist(DESC_NUM_BINS);


    // Loop over all the keypoints
    for (unsigned int ikp = 0; ikp<m_kpnum; ikp++)
    {
        // vcl_cout << "ikp = " << ikp << vcl_endl;

        unsigned int scale = this->m_kps[ikp].scale;
        float kpxi         = this->m_kps[ikp].xi;
        float kpyi         = this->m_kps[ikp].yi;

        // the descriptor's position information
        float descxi       = kpxi;
        float descyi       = kpyi;

        unsigned int ii = static_cast<unsigned int>(kpxi*2) /
                          static_cast<unsigned int>(pow(2.0f,static_cast<float>(scale/m_intervals)));
        unsigned int jj = static_cast<unsigned int>(kpyi*2) /
                          static_cast<unsigned int>(pow(2.0f,static_cast<float>(scale/m_intervals)));

        unsigned int ni = m_glist[scale/m_intervals][0].ni();
        unsigned int nj = m_glist[scale/m_intervals][0].nj();

        // In order to achieve orientation invariance, the coordinates of the descriptor and the
        // gradient orientations are rotated relative to the keypoint orientation.
        // find the main orientation,
        // which is the orientation corresponding to the largest magnitude
        vector<double> orien = this->m_kps[ikp].orien;
        vector<double> mag   = this->m_kps[ikp].mag;

        double main_mag = mag[0];
        double main_orien = orien[0];

        for (unsigned int orient_count=1; orient_count<mag.size(); orient_count++)
            if (mag[orient_count] > main_mag)
            {
                main_orien = orien[orient_count];
                main_mag = mag[orient_count];
            }

        // vcl_cout << "main_orient = " << main_orien << vcl_endl;
        // vcl_cout << "main_mag    = " << main_mag    << vcl_endl;


        // sample points in FEATURE_WINDOW_SIZE x FEATURE_WINDOW_SIZE region
        // assign a weight to the magnitude of each sample point
        unsigned int hfsz = FEATURE_WINDOW_SIZE/2;
        vnl_matrix<double> weight(FEATURE_WINDOW_SIZE,FEATURE_WINDOW_SIZE);
        vcl_vector<double> fv(FVSIZE);

        for (i=0;i<FEATURE_WINDOW_SIZE;i++)
            for (j=0;j<FEATURE_WINDOW_SIZE;j++)
            {
                // vcl_cout << "ii = "   << ii   << ", "
                //     << "i = "    << i    << ", "
                //     << "ni = "   << ni   << ", "
                //     << "jj = "   << jj   << ", "
                //     << "j ="     << j    << ", "
                //     << "nj = "   << nj   << ", "
                //    << "hfsz = " << hfsz << endl;

                if (ii+i+1<hfsz || ii+i+1>ni+hfsz || jj+j+1<hfsz || jj+j+1>nj+hfsz)
                    weight[i][j] = 0;
                else
                {
                    // vcl_cout << scale/m_intervals << " " << scale%m_intervals << vcl_endl;
                    //  vcl_cout << ii +i+1 - hfsz << " " << jj+j+1-hfsz << vcl_endl;

                    // vcl_cout << InterpolatedMagnitude[scale/m_intervals][scale%m_intervals].ni() << " "
                    //     << InterpolatedMagnitude[scale/m_intervals][scale%m_intervals].nj() << " "
                    //     << ii+i+1-hfsz << " "
                    //     << jj+j+1-hfsz << vcl_endl;


                    weight[i][j] = G[i][j] *
                                   InterpolatedMagnitude[scale/m_intervals][scale%m_intervals](ii+i+1-hfsz,jj+j+1-hfsz);


                }



//                vcl_cout << "G["      << i << "][" << j << "] = " << G[i][j]      << vcl_endl;
//                vcl_cout << "weight[" << i << "][" << j << "] = " << weight[i][j] << vcl_endl;
            }




        // OK, now generate the 4x4x8 = 128 feature vector for each keypoint
        // the 8-feature vectors are in the region

        // (kprow-hfsz+1, kpcol-hfsz+1) -- (kprow-hfsz/2, kpcol-hfsz/2)
        // (kprow-hfsz/2+1,kpcol-hfsz+1) --  (kprow, kpcol-hfsz/2)
        // (kprow+1,kpcol-hfsz+1) -- (kprow+hfsz/2,kpcol-hfsz/2)
        // (kprow+hfsz/2+1,kpcol-hfsz+1) -- (kprow+hfsz,kpcol-hfsz/2)

        // (kprow-hfsz+1, kpcol-hfsz/2+1) -- (kprow-hfsz/2, kpcol)
        // (kprow-hfsz/2+1,kpcol-hfsz/2+1) --  (kprow, kpcol)
        // (kprow+1,kpcol-hfsz/2+1) -- (kprow+hfsz/2,kpcol)
        // (kprow+hfsz/2+1,kpcol-hfsz/2+1) -- (kprow+hfsz,kpcol)

        // (kprow-hfsz+1, kpcol+1) -- (kprow-hfsz/2, kpcol+hfsz/2)
        // (kprow-hfsz/2+1,kpcol+1) --  (kprow, kpcol+hfsz/2)
        // (kprow+1,kpcol+1) -- (kprow+hfsz/2,kpcol+hfsz/2)
        // (kprow+hfsz/2+1,kpcol+1) -- (kprow+hfsz,kpcol+hfsz/2)

        // (kprow-hfsz+1, kpcol+hfsz/2+1) -- (kprow-hfsz/2, kpcol+hfsz)
        // (kprow-hfsz/2+1,kpcol+hfsz/2+1) --  (kprow, kpcol+hfsz)
        // (kprow+1,kpcol+hfsz/2+1) -- (kprow+hfsz/2,kpcol+hfsz)
        // (kprow+hfsz/2+1,kpcol+hfsz/2+1) -- (kprow+hfsz,kpcol+hfsz)

        // so in general for i=0..3 j=0..3

        // (kprow-hfsz+1+(hfsz/2)*i, kpcol-hfsz+1+(hfsz/2)*j) -- (kprow+(hfsz/2)*(i-1), kpcol+(hfsz/2)*(j-1))

//        vcl_cout << "start" << vcl_endl;

        for (i=0;i<FEATURE_WINDOW_SIZE/4;i++)
            for (j=0;j<FEATURE_WINDOW_SIZE/4;j++)
            {
                // clear the histogram
                for (unsigned int t=0;t<DESC_NUM_BINS;t++)
                    hist[t] = 0.0;

                // find the start and the limit
                int starti = static_cast<int>(ii)-static_cast<int>(hfsz)+1+static_cast<int>((hfsz/2)*i);
                int startj = static_cast<int>(jj)-static_cast<int>(hfsz)+1+static_cast<int>((hfsz/2)*j);
                int limiti = static_cast<int>(ii)+static_cast<int>(hfsz/2)*(static_cast<int>(i)-1);
                int limitj = static_cast<int>(jj)+static_cast<int>(hfsz/2)*(static_cast<int>(j)-1);

                // vcl_cout << "starti = " << starti << ", "
                //     << "startj = " << startj << ", "
                //     << "limiti = " << limiti << ", "
                //     << "limitj = " << limitj << vcl_endl;
                // loop over the region
                for (int k=starti;k<=limiti;k++)
                    for (int t=startj;t<=limitj;t++)
                    {
                        // if out of boundary
                        if (k<0 || k>static_cast<int>(ni) || t<0 || t>static_cast<int>(nj))
                            continue;
                        // fetch the sample point orientation
                        double sample_orien = InterpolatedOrientation[scale/m_intervals][scale%m_intervals](k,t);
                        // rotate relative to keypoint orientation
                        sample_orien -= main_orien;

                        // as sample_orient is within [-M_PI,M_PI) and main_orient is also within [-M_PI,M_PI)
                        // the subtraction will result in (-2*M_PI,2*M_PI)
                        // so we need to normalise it to [0,2*M_PI)
                        if (sample_orien<0)
                            sample_orien += 2*M_PI;

                        // vcl_cout << "sample_orien = " << sample_orien << vcl_endl;

                        assert (sample_orien >= 0 && sample_orien < 2*M_PI);


                        // now convert into degree [0,360)
                        unsigned int sample_orien_d = static_cast<unsigned int>(sample_orien*180/M_PI);

                        assert (sample_orien_d<360);


                        // bin represent the bin index
                        // while bin_f is the actual entry
                        unsigned int bin = sample_orien_d/(360/DESC_NUM_BINS);
                        double bin_f = static_cast<double>(sample_orien_d)/static_cast<double>(360/DESC_NUM_BINS);


                        // To avoid boundary effects, each sample is accumulated into neighbouring bins
                        // weighted by 1-d in all dimensions, where d is the distance from the center of
                        // bin measured in units of bin spacing

                        // the central value of bin entry is (bin+0.5)
                        // so the distance of sample from the central value of the bin is
                        // d = fabs(bin_f-(bin+0.5))
                        // so the weight 1-d is
                        // w = 1 - fabs(bin_f-(bin+0.5))

                        // the weight of (k,t) correspond to weight[?][?]
                        // index of (k,t) corresponding to keypoint position is
                        // (k-kprow, t-kpcol)
                        // which should correspond to weight[k-kprow+hfsz-1][t-kpcol+hfsz-1]

                        // then accumulate into the histogram
                        //  vcl_cout << k-ii+hfsz-1 << " " << t-jj+hfsz-1 << vcl_endl;

                        assert (bin < DESC_NUM_BINS);
                        assert (k+hfsz-1-ii<FEATURE_WINDOW_SIZE && t+hfsz-1-jj < FEATURE_WINDOW_SIZE);


                        hist[bin] += (1-fabs(bin_f-(bin+0.5))) * weight[k+hfsz-1-ii][t+hfsz-1-jj];
                    }
                // assign the histogram to feature vector
                // the index of feature vector is (i*FEATURE_WINDOW_SIZE+j)*DESC_NUM_BINS + bin

                for (unsigned int t=0;t<DESC_NUM_BINS;t++)
                {
                    fv[(i*FEATURE_WINDOW_SIZE/4+j)*DESC_NUM_BINS + t] = hist[t];
                    // vcl_cout << "hist[" << t << "] = " << hist[t] << vcl_endl;
                }

            }


        //        vcl_cout << "end" << vcl_endl;


        // then, normalise ...
        double norm = 0.0;
        for (unsigned int t=0;t<FVSIZE;t++)
            norm += pow(fv[t],2.0);
        norm = sqrt(norm);
        for (unsigned int t=0;t<FVSIZE;t++)
        {
            fv[t] /= norm;
            // vcl_cout << fv[t] << vcl_endl;
        }


        // for(t=0;t<FVSIZE;t++)
        // printf("ikp=%d,t=%d,fv=%f\n",ikp,t,this -> m_desc[ikp].fv[t]);

        // thresholding the vector to no more than FV_THRESHOLD
        for (unsigned int t=0;t<FVSIZE;t++)
            if (fv[t] > FV_THRESHOLD)
                fv[t] =  FV_THRESHOLD;

        //renomalise

        norm = 0.0;
        for (unsigned int t=0;t<FVSIZE;t++)
            norm += pow(fv[t],2.0);
        norm = sqrt(norm);
        for (unsigned int t=0;t<FVSIZE;t++)
        {
            fv[t] /= norm;
            // vcl_cout << fv[t] << vcl_endl;

        }


        // vcl_cout << "start" << vcl_endl;


        // finally push back the descriptor
        m_desc.push_back(SIFTFeature(descxi, descyi, fv));

        // vcl_cout << "end" << vcl_endl;
    }


    // make sure it is OK
    assert (m_desc.size()==m_kpnum);


    // free the storage
    for (i=0;i<m_octaves;i++)
    {
        delete [] InterpolatedMagnitude[m_octaves-i-1];
        delete [] InterpolatedOrientation[m_octaves-i-1];
    }
    delete [] InterpolatedMagnitude;
    delete [] InterpolatedOrientation;

}



void JIFT::doJIFT(const vil_image_view<vxl_byte>& srcimg)
{
    this -> buildPyramid(srcimg);
    this -> detectLocalExtrema();
    this -> assignOrientation();
    this -> extractKeypointDescriptor();
}

const vector<SIFTFeature> & JIFT::descriptor() const
{
    return m_desc;
}

const vector<Keypoint>   & JIFT::keypoint() const
{
    return m_kps;
}


} // end of namespace JIFT

