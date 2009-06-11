#ifndef _GAUSSIAN_H
#define _GAUSSIAN_H

// This file is and adaptation of the code wrote by Jun Liu
// http://www.cs.man.ac.uk/~liuja/

#include<vil/vil_image_view.h>
#include<vnl/vnl_matrix.h>

namespace JIFT
{

class Gaussian
{
  public:
    static void gaussian_filter(vil_image_view<double>&, vil_image_view<double>&, double, double cut_off=0.001);
    static unsigned int getGaussianKernelSize(double sigma, double cut_off=0.001);
    static vnl_matrix<double> buildInterpolatedGaussianTable(unsigned int size, double sigma);
};

} // end of namespace JIFT




#endif
