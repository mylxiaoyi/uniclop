/*******************************************************************************
 *
 * FILENAME:     Gaussian.cpp
 * DESCRIPTION:  The Gaussian filter
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

#include "Gaussian.hpp"
#include <cassert>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

namespace JIFT
{

const unsigned int MAX_KERNEL_SIZE = 20;


double gaussian2D ( double x, double y, double sigma)
{
        // according to the definition of gaussian filter
    return 1.0/(2*M_PI*sigma*sigma) * exp(-(x*x+y*y)/(2.0*sigma*sigma));
}


unsigned int Gaussian::getGaussianKernelSize(double sigma, double cut_off)
{
    unsigned int i;
    for (i=0;i<MAX_KERNEL_SIZE;i++)
        if (exp(-(static_cast<double>(i*i))/(2.0*sigma*sigma))<cut_off)
            break;
    unsigned int size = 2*i-1;
    return size;
}


vnl_matrix<double> Gaussian::buildInterpolatedGaussianTable(unsigned int size, double sigma)
{
    unsigned int i, j;
    double half_kernel_size = size/2 - 0.5;
    double sog;
    vnl_matrix<double>G(size,size);


        // this should work with the size of even number
    assert (size%2 == 0);


        // construct the Gaussian kernel table
    for (i=0; i<size; i++)
        for (j=0; j<size; j++)
            G[i][j] = gaussian2D(static_cast<double>(i)-half_kernel_size,
                                 static_cast<double>(j)-half_kernel_size,
                                 sigma);

        // normalise, to make the sum of coefficient equals 1.0
    sog = 0.0;
    for (i=0; i<size; i++)
        for (j=0; j<size; j++)
            sog += G[i][j];
    for (i=0; i<size; i++)
        for (j=0; j<size; j++)
            G[i][j] = 1.0/sog * G[i][j];

    return G;
}


void Gaussian::gaussian_filter(vil_image_view<double>     & src_im,          // the source image view
                               vil_image_view<double>     & dst_im,          // the destination image view
                               double sigma,                                 // the sigma (standard deviation)
                               double cut_off)                               // the cut_off value
{
    assert(src_im.nplanes()==1 && dst_im.nplanes()==1); // make sure it is a single plane image view
    dst_im.set_size(src_im.ni(),src_im.nj());


    unsigned int i,j,k,istart,ilimit,jstart,jlimit;
    double G[MAX_KERNEL_SIZE];
    double sum;
    unsigned int filter_width;

    unsigned int xsize = src_im.ni();
    unsigned int ysize = src_im.nj();

    vil_image_view<double> buffer(xsize,ysize);

    for (i=0;i<MAX_KERNEL_SIZE;i++)
    {
        G[i] = exp(-(static_cast<double>(i*i))/(2.0*sigma*sigma));
        if (G[i] < cut_off)
            break;
    }

    filter_width = i;

        // sum of gaussian
    sum = G[0];
    for (i=1;i<filter_width;i++)
        sum += 2.0*G[i];

        // normalise, make sure the sum equals to 1.0
    for (i=0;i<filter_width;i++)
        G[i] /= sum;

    istart = filter_width;
    ilimit = xsize - filter_width;
    jstart = filter_width;
    jlimit = ysize - filter_width;

        // double * src_top_left = src_im.top_left_ptr();
        // double * buf_top_left = buffer.top_left_ptr();
        // vcl_ptrdiff_t istep = src_im.istep();
        // vcl_ptrdiff_t jstep = src_im.jstep();



        // blur horizontally
    for (j=0;j<ysize;j++)
        for (i=0;i<xsize;i++)
        {
            if (i>=istart && i<ilimit)
            {
                buffer(i,j) = G[0] * src_im(i,j);
                    // *(buf_top_left+i*istep+j*jstep) = G[0] * (*(src_top_left+i*istep+j*jstep));

                for (k=1;k<filter_width;k++)
                {
                        // *(buf_top_left+i*istep+j*jstep) += G[k] * (*(src_top_left+(i-k)*istep+j*jstep));
                        // *(buf_top_left+i*istep+j*jstep) += G[k] * (*(src_top_left+(i+k)*istep+j*jstep));
                    buffer(i,j) += G[k] * src_im(i-k,j);
                    buffer(i,j) += G[k] * src_im(i+k,j);
                }
            }
            else
            {
                sum = 0.0;
                for (k=1;k<filter_width;k++)
                {
                        // out of boundary
                    if(i<k || i+k>=xsize)
                        sum += G[k];
                    else
                        continue;
                }

                sum = 1.0 - sum;

                    // *(buf_top_left+i*istep+j*jstep) = G[0] * (*(src_top_left+i*istep+j*jstep));
                buffer(i,j) = G[0]/sum * src_im(i,j);

                for (k=1;k<filter_width;k++)
                {
                    if (i<k || i+k>=xsize) continue;
                    buffer(i,j) += G[k]/sum * src_im(i-k,j);
                    buffer(i,j) += G[k]/sum * src_im(i+k,j);
                        //*(buf_top_left+i*istep+j*jstep) += G[k] * (*(src_top_left+(i-k)*istep+j*jstep));
                        //*(buf_top_left+i*istep+j*jstep) += G[k] * (*(src_top_left+(i+k)*istep+j*jstep));
                }
            }

        }


        // blur vertically
    for (i=0;i<xsize;i++)
        for (j=0;j<ysize;j++)
        {
            if (j>=jstart && j<jlimit)
            {

                dst_im(i,j) = G[0] * buffer(i,j);
                for (k=1;k<filter_width;k++)
                {
                    dst_im(i,j) += G[k] * buffer(i,j-k);
                    dst_im(i,j) += G[k] * buffer(i,j+k);
                }
            }
            else
            {
                sum = 0.0;
                for (k=1;k<filter_width;k++)
                {
                    if(j<k || j+k>=ysize)
                        sum += G[k];
                    else
                        continue;
                }
                sum = 1.0 - sum;

                dst_im(i,j) = G[0]/sum * buffer(i,j);
                for (k=1;k<filter_width;k++)
                {
                    if (j<k || j+k>=ysize) continue;
                    dst_im(i,j) += G[k]/sum * buffer(i,j-k);
                    dst_im(i,j) += G[k]/sum * buffer(i,j+k);
                }
            }

        }
}

} // end of namespace JIFT




