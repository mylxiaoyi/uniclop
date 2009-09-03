#ifndef __OVDISPARITYPOSTPROCESSOR_H
#define __OVDISPARITYPOSTPROCESSOR_H

#include "ImageT.hpp"


namespace Openvis3d {


///Abstract Base Class for postprocessing stereo disparity results.
/** 
* The DisparityPostprocessor class defines a basic interface for postprocessing stereo disparity results.
* Subclasses can implement the interface to provide a variety of preprocessors 
* (e.g., occlusion filling, median filtering, plane fitting, etc.)
*
* @author Abhijit Ogale
*/

class DisparityPostprocessor
{
public:

  DisparityPostprocessor(); /**< Default constructor with no parameters */

  virtual ~DisparityPostprocessor(); /**< Destructor */

  /**
  * Main method for postprocessing stereo disparity results.
  * Note: This method modifies the inputs, so be careful.
  * @param leftDisparityMap the disparity map for the left image
  * @param rightDisparityMap the disparity map for the right image
  * @param leftOcclusions the occlusion map for the left image
  * @param rightOcclusions the occlusion map for the right image	
  * @return true if successful.
  */
  virtual bool postProcessDisparity(ImageT<double> & leftDisparityMap, ImageT<double> & rightDisparityMap, ImageT<double> & leftOcclusions, ImageT<double> & rightOcclusions) = 0;

  /**
  * Used for specifying any parameters required.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual bool setParams(int nparams, double*params) = 0;
};

DisparityPostprocessor::DisparityPostprocessor()
{
}

DisparityPostprocessor::~DisparityPostprocessor()
{
}

}

#endif //__OVDISPARITYPOSTPROCESSOR_H
