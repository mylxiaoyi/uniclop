#ifndef __OVSTEREOT_H
#define __OVSTEREOT_H

#include "ImageAdapter.hpp"
#include "ImageT.hpp"
#include "ImagePairPreprocessorT.hpp"
#include "LocalMatcherT.hpp"
#include "StereoGlobalMatcherT.hpp"
#include "DisparityPostprocessor.hpp"


namespace Openvis3d {

///Class for managing the execution of a stereo algorithm.
/** 
* The StereoT class manages the execution of a stereo algorithm, allowing the user to
* select any image preprocessors, local matching methods, global stereo algorithm, and
* disparity and occlusion postprocessors.
*
* @author Abhijit Ogale
*/
template<typename T>
class StereoT
{
public:

  StereoT(); /**< Default constructor with no parameters */

  virtual ~StereoT(); /**< Destructor */

  /**
  * Specifies the image pair preprocessor.
  * @param imagePairPreprocessor this specifies the preprocessor to be used.
  */
  virtual void setImagePairPreprocessor(ImagePairPreprocessorT<T> & imagePairPreprocessor);

  /**
  * Useful to set any parameters of the image pair preprocessor.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setImagePairPreprocessorParams(int nparams, double*params);


  /**
  * Specifies the local image matcher to be used.
  * @param localImageMatcher this is an LocalMatcherT<T> object which matches a pair of images.
  */
  virtual void setLocalImageMatcher(LocalMatcherT<T> & localImageMatcher);

  /**
  * Useful to set parameters of the local image matcher.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setLocalImageMatcherParams(int nparams, double*params);

  /**
  * Specifies the global stereo matcher (algorithm).
  * @param stereoGlobalMatcher this specifies the global stereo algorithm to be used.
  */
  virtual void setGlobalMatcher(StereoGlobalMatcherT<T> & stereoGlobalMatcher);

  /**
  * Useful to set any parameters of the global stereo algorithm.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setGlobalMatcherParams(int nparams, double*params);

  /**
  * Specifies the disparity postprocessor to be used.
  * @param disparityPostProcessor this DisparityPostprocessor object specifies the postprocessor to be used.
  */
  virtual void setDisparityPostprocessor(DisparityPostprocessor & disparityPostProcessor);

  /**
  * Useful to set any parameters of the disparity postprocessor.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setDisparityPostprocessorParams(int nparams, double*params);

  /**
  * Main method for executing stereo matching on an image pair.
  * Note: This method modifies the input images, so be careful.
  * @param i1 the first image
  * @param i2 the second image
  * @param minshift method searches for disparities from minshift to maxshift
  * @param maxshift method searches for disparities from minshift to maxshift
  * @param leftDisparityMap the disparity map for the left image. (method returns this).
  * @param rightDisparityMap the disparity map for the right image. (method returns this).
  * @param leftOcclusions the occlusion map for the left image. (method returns this).
  * @param rightOcclusions the occlusion map for the right image. (method returns this).	
  * @return true if successful.
  */
  virtual bool doStereoMatching(const ImageAdapter & i1, const ImageAdapter & i2, double minshift, double maxshift, ImageAdapter & leftDisparityMap, ImageAdapter & rightDisparityMap, ImageAdapter & leftOcclusions, ImageAdapter & rightOcclusions);


protected:

  ImagePairPreprocessorT<T>		*mImagePairPreprocessor;	/**< Image pair preprocessor */
  LocalMatcherT<T>				*mLocalImageMatcher;		/**< Local image pair matcher */
  StereoGlobalMatcherT<T>		*mStereoGlobalMatcher;		/**< Global stereo algorithm */
  DisparityPostprocessor		*mDisparityPostprocessor;	/**< Disparity map post processor */

  // The flags below are for marking whether the above variables are internally allocated.
  // In case they are, we are responsible for releasing them at destruction. 
  // If not, then we do not release them.
  bool isImagePairPreprocessorInternallyAllocated;
  bool isLocalImageMatcherInternallyAllocated;
  bool isStereoGlobalMatcherInternallyAllocated;
  bool isDisparityPostprocessorInternallyAllocated;
};

template<typename T>
StereoT<T>::StereoT()
: mImagePairPreprocessor(0), mLocalImageMatcher(0), mStereoGlobalMatcher(0), mDisparityPostprocessor(0), 
isImagePairPreprocessorInternallyAllocated(false), isLocalImageMatcherInternallyAllocated(false), isStereoGlobalMatcherInternallyAllocated(false), isDisparityPostprocessorInternallyAllocated(false)
{
}

template<typename T>
StereoT<T>::~StereoT()
{
  if(isImagePairPreprocessorInternallyAllocated && (mImagePairPreprocessor!=0)) delete mImagePairPreprocessor;
  if(isLocalImageMatcherInternallyAllocated && (mLocalImageMatcher!=0)) delete mLocalImageMatcher;
  if(isStereoGlobalMatcherInternallyAllocated && (mStereoGlobalMatcher!=0)) delete mStereoGlobalMatcher;
  if(isDisparityPostprocessorInternallyAllocated && (mDisparityPostprocessor!=0)) delete mDisparityPostprocessor;
}

template<typename T>
void StereoT<T>::setImagePairPreprocessor(ImagePairPreprocessorT<T> & imagePairPreprocessor)
{
  mImagePairPreprocessor = & imagePairPreprocessor;
}

template<typename T>
void StereoT<T>::setImagePairPreprocessorParams(int nparams, double*params)
{
  if(mImagePairPreprocessor)mImagePairPreprocessor->setParams(nparams, params);
}

template<typename T>
void StereoT<T>::setLocalImageMatcher(LocalMatcherT<T> & localImageMatcher)
{
  mLocalImageMatcher = &localImageMatcher;
}

template<typename T>
void StereoT<T>::setLocalImageMatcherParams(int nparams, double*params)
{
  if(mLocalImageMatcher)mLocalImageMatcher->setParams(nparams, params);
}

template<typename T>
void StereoT<T>::setGlobalMatcher(StereoGlobalMatcherT<T> & stereoGlobalMatcher)
{
  mStereoGlobalMatcher = &stereoGlobalMatcher;
}

template<typename T>
void StereoT<T>::setGlobalMatcherParams(int nparams, double*params)
{
  if(mStereoGlobalMatcher)mStereoGlobalMatcher->setParams(nparams, params);
}

template<typename T>
void StereoT<T>::setDisparityPostprocessor(DisparityPostprocessor & disparityPostProcessor)
{
  mDisparityPostprocessor = &disparityPostProcessor;
}

template<typename T>
void StereoT<T>::setDisparityPostprocessorParams(int nparams, double*params)
{
  if(mDisparityPostprocessor)mDisparityPostprocessor->setParams(nparams, params);
}

template<typename T>
bool StereoT<T>::doStereoMatching(const ImageAdapter & i1, const ImageAdapter & i2, double minshift, double maxshift, ImageAdapter & leftDisparityMap, ImageAdapter & rightDisparityMap, ImageAdapter & leftOcclusions, ImageAdapter & rightOcclusions)
{
  ImageT<T> mImage1, mImage2;
  ImageT<T> mLeftDisparityMap, mRightDisparityMap, mLeftOcclusions, mRightOcclusions;

  mImage1.copyFromAdapter(i1);
  mImage2.copyFromAdapter(i2);
  mLeftDisparityMap.copyFromAdapter(leftDisparityMap);
  mRightDisparityMap.copyFromAdapter(rightDisparityMap);
  mLeftOcclusions.copyFromAdapter(leftOcclusions);
  mRightOcclusions.copyFromAdapter(rightOcclusions);

  if(!haveEqualDimensions(mImage1, mImage2)) return false; //return if images have different dimensions	

  //output images must have the same height and width as input, but at least one channel
  if(!haveEqualHeightWidth(mImage1, mLeftDisparityMap)) return false; 
  if(mLeftDisparityMap.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mRightDisparityMap)) return false;
  if(mRightDisparityMap.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mLeftOcclusions)) return false;
  if(mLeftOcclusions.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mRightOcclusions)) return false;
  if(mRightOcclusions.getChannels()<=0) return false;

  if(mLocalImageMatcher==0) return false; //must select a local matcher
  if(mStereoGlobalMatcher==0) return false; //must select a global matcher

  mStereoGlobalMatcher->setLocalImageMatcher(*mLocalImageMatcher); //tell global matcher about local matcher

  if(mImagePairPreprocessor)mImagePairPreprocessor->preProcessImagePair(mImage1, mImage2);
  mStereoGlobalMatcher->doMatching(mImage1, mImage2, minshift, maxshift, mLeftDisparityMap, mRightDisparityMap, mLeftOcclusions, mRightOcclusions);
  if(mDisparityPostprocessor)mDisparityPostprocessor->postProcessDisparity(mLeftDisparityMap, mRightDisparityMap, mLeftOcclusions, mRightOcclusions);

  mLeftDisparityMap.copyToAdapter(leftDisparityMap);
  mRightDisparityMap.copyToAdapter(rightDisparityMap);
  mLeftOcclusions.copyToAdapter(leftOcclusions);
  mRightOcclusions.copyToAdapter(rightOcclusions);

  return true;
}


}

#endif //__OVSTEREOT_H
