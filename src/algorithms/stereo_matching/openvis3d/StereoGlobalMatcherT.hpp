#ifndef __OVSTEREOGLOBALMATCHERT_H
#define __OVSTEREOGLOBALMATCHERT_H

#include "ImageT.hpp"
#include "LocalMatcherT.hpp"


namespace Openvis3d {
	
///Abstract Base Class Template for defining a global matching stereo algorithm.
/** 
* The StereoGlobalMatcherT class defines a basic interface for a global matching stereo algorithm.
* Subclasses can implement the interface to provide a variety of global matching methods.
*
* @author Abhijit Ogale
*/
template<typename T>
class StereoGlobalMatcherT
{
public:

  StereoGlobalMatcherT(); /**< Default constructor with no parameters */

  virtual ~StereoGlobalMatcherT(); /**< Destructor */

  /**
  * Main method for stereo matching an image pair.
  * Note: This method modifies the input images, so be careful.
  * @param i1 the first image
  * @param i2 the second image
  * @param minshift method searches for disparities from minshift to maxshift
  * @param maxshift method searches for disparities from minshift to maxshift
  * @param leftDisparityMap the disparity map for the left image. (method sets this).
  * @param rightDisparityMap the disparity map for the right image. (method sets this).
  * @param leftOcclusions the occlusion map for the left image. (method sets this).
  * @param rightOcclusions the occlusion map for the right image. (method sets this).	
  * @return true if successful.
  */
  virtual bool doMatching(ImageT<T> & i1, ImageT<T> & i2, double minshift, double maxshift, ImageT<double> & leftDisparityMap, ImageT<double> & rightDisparityMap, ImageT<double> & leftOcclusions, ImageT<double> & rightOcclusions) = 0;

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
StereoGlobalMatcherT<T>::StereoGlobalMatcherT()
{
}

template<typename T>
StereoGlobalMatcherT<T>::~StereoGlobalMatcherT()
{
}

}
#endif //__OVSTEREOGLOBALMATCHERT_H
