#ifndef __OVFLOWGLOBALMATCHERT_H
#define __OVFLOWGLOBALMATCHERT_H

#include "ImageT.hpp"
#include "LocalMatcherT.hpp"


namespace Openvis3d {

///Abstract Base Class Template for defining a global matching flow algorithm.
/** 
* The FlowGlobalMatcherT class defines a basic interface for a global matching flow algorithm.
* Subclasses can implement the interface to provide a variety of global matching methods.
*
* @author Abhijit Ogale
*/
template<typename T>
class FlowGlobalMatcherT
{
public:

  FlowGlobalMatcherT(); /**< Default constructor with no parameters */

  virtual ~FlowGlobalMatcherT(); /**< Destructor */

  /**
  * Main method for computing optical flow on an image pair.
  * Note: This method modifies the input images, so be careful.
  * @param i1 the first image
  * @param i2 the second image
  * @param minshiftX method searches for horizontal flow values from minshiftX to maxshiftX
  * @param maxshiftX method searches for horizontal flow values from minshiftX to maxshiftX
  * @param minshiftY method searches for vertical flow values from minshiftY to maxshiftY
  * @param maxshiftY method searches for vertical flow values from minshiftY to maxshiftY
  * @param u1 the horizontal flow for image 1. (method sets this).
  * @param v1 the vertical flow for image 1. (method sets this).
  * @param o1 the occlusion map for image 1. (method sets this).
  * @param u2 the horizontal flow for image 2. (method sets this).
  * @param v2 the vertical flow for image 2. (method sets this).
  * @param o2 the occlusion map for image 2. (method sets this).
  * @return true if successful.
  */
  virtual bool doMatching(ImageT<T> & i1, ImageT<T> & i2, double minshiftX, double maxshiftX, double minshiftY, double maxshiftY, ImageT<double> & u1, ImageT<double> & v1, ImageT<double> & o1, ImageT<double> & u2, ImageT<double> & v2,  ImageT<double> & o2) = 0;


  /**
  * Used for specifying any parameters required.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual bool setParams(int nparams, double*params) = 0;

  /**
  * Specifies the local image matcher to be used by the algorithm.
  * @param localImageMatcher this is an LocalMatcherT<T> object which matches a pair of images.
  */
  virtual void setLocalImageMatcher(LocalMatcherT<T> & localImageMatcher) = 0;
};

template<typename T>
FlowGlobalMatcherT<T>::FlowGlobalMatcherT()
{
}

template<typename T>
FlowGlobalMatcherT<T>::~FlowGlobalMatcherT()
{
}

}

#endif //__OVFLOWGLOBALMATCHERT_H
