#ifndef __OVFLOWPOSTPROCESSOR_H
#define __OVFLOWPOSTPROCESSOR_H

#include "ImageT.hpp"


namespace Openvis3d {

///Abstract Base Class for postprocessing optical flow results.
/** 
* The FlowPostprocessor class defines a basic interface for postprocessing optical flow results.
* Subclasses can implement the interface to provide a variety of preprocessors 
* (e.g., occlusion filling, median filtering, plane fitting, etc.)
*
* @author Abhijit Ogale
*/

class FlowPostprocessor
{
public:

  FlowPostprocessor(); /**< Default constructor with no parameters */

  virtual ~FlowPostprocessor(); /**< Destructor */

  /**
  * Main method for postprocessing flow results.
  * Note: This method modifies the inputs, so be careful.
  * @param u1 the horizontal flow for image 1. (method sets this).
  * @param v1 the vertical flow for image 1. (method sets this).
  * @param o1 the occlusion map for image 1. (method sets this).
  * @param u2 the horizontal flow for image 2. (method sets this).
  * @param v2 the vertical flow for image 2. (method sets this).
  * @param o2 the occlusion map for image 2. (method sets this).
  * @return true if successful.
  */
  virtual bool postProcessFlow(ImageT<double> & u1, ImageT<double> & v1, ImageT<double> & o1, ImageT<double> & u2, ImageT<double> & v2,  ImageT<double> & o2) = 0;

  /**
  * Used for specifying any parameters required.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual bool setParams(int nparams, double*params) = 0;
};

FlowPostprocessor::FlowPostprocessor()
{
}

FlowPostprocessor::~FlowPostprocessor()
{
}

}
#endif //__OVFLOWPOSTPROCESSOR_H
