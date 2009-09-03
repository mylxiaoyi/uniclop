#ifndef __OVFLOWT_H
#define __OVFLOWT_H

#include "ImageAdapter.hpp"
#include "ImageT.hpp"
#include "ImagePairPreprocessorT.hpp"
#include "LocalMatcherT.hpp"
#include "FlowGlobalMatcherT.hpp"
#include "FlowPostprocessor.hpp"



namespace Openvis3d {

///Class for managing the execution of an optical flow algorithm.
/** 
* The FlowT class manages the execution of an optical flow algorithm, allowing the user to
* select any image preprocessors, local matching methods, global flow algorithm, and
* optical flow and occlusion postprocessors.
*
* @author Abhijit Ogale
*/
template<typename T>
class FlowT
{
public:

  FlowT(); /**< Default constructor with no parameters */

  virtual ~FlowT(); /**< Destructor */

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
  * Specifies the global optical flow matcher (algorithm).
  * @param flowGlobalMatcher this specifies the global optical flow algorithm to be used.
  */
  virtual void setGlobalMatcher(FlowGlobalMatcherT<T> & flowGlobalMatcher);

  /**
  * Useful to set any parameters of the global flow algorithm.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setGlobalMatcherParams(int nparams, double*params);

  /**
  * Specifies the optical flow and occlusion postprocessor to be used.
  * @param flowPostProcessor this object specifies the flow postprocessor to be used.
  */
  virtual void setFlowPostprocessor(FlowPostprocessor & flowPostProcessor);

  /**
  * Useful to set any parameters of the flow postprocessor.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual void setFlowPostprocessorParams(int nparams, double*params);

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
  virtual bool doOpticalFlow(const ImageAdapter & i1, const ImageAdapter & i2, double minshiftX, double maxshiftX, double minshiftY, double maxshiftY, ImageAdapter & u1, ImageAdapter & v1, ImageAdapter & o1, ImageAdapter & u2, ImageAdapter & v2,  ImageAdapter & o2);


protected:

  ImagePairPreprocessorT<T>		*mImagePairPreprocessor;	/**< Image pair preprocessor */
  LocalMatcherT<T>				*mLocalImageMatcher;		/**< Local image pair matcher */
  FlowGlobalMatcherT<T>			*mFlowGlobalMatcher;		/**< Global optical flow algorithm */
  FlowPostprocessor				*mFlowPostprocessor;	/**< Flow post processor */

  // The flags below are for marking whether the above variables are internally allocated.
  // In case they are, we are responsible for releasing them at destruction. 
  // If not, then we do not release them.
  bool isImagePairPreprocessorInternallyAllocated;
  bool isLocalImageMatcherInternallyAllocated;
  bool isFlowGlobalMatcherInternallyAllocated;
  bool isFlowPostprocessorInternallyAllocated;
};

template<typename T>
FlowT<T>::FlowT()
: mImagePairPreprocessor(0), mLocalImageMatcher(0), mFlowGlobalMatcher(0), mFlowPostprocessor(0), 
isImagePairPreprocessorInternallyAllocated(false), isLocalImageMatcherInternallyAllocated(false), isFlowGlobalMatcherInternallyAllocated(false), isFlowPostprocessorInternallyAllocated(false)
{
}

template<typename T>
FlowT<T>::~FlowT()
{
  if(isImagePairPreprocessorInternallyAllocated && (mImagePairPreprocessor!=0)) delete mImagePairPreprocessor;
  if(isLocalImageMatcherInternallyAllocated && (mLocalImageMatcher!=0)) delete mLocalImageMatcher;
  if(isFlowGlobalMatcherInternallyAllocated && (mFlowGlobalMatcher!=0)) delete mFlowGlobalMatcher;
  if(isFlowPostprocessorInternallyAllocated && (mFlowPostprocessor!=0)) delete mFlowPostprocessor;
}

template<typename T>
void FlowT<T>::setImagePairPreprocessor(ImagePairPreprocessorT<T> & imagePairPreprocessor)
{
  mImagePairPreprocessor = & imagePairPreprocessor;
}

template<typename T>
void FlowT<T>::setImagePairPreprocessorParams(int nparams, double*params)
{
  if(mImagePairPreprocessor)mImagePairPreprocessor->setParams(nparams, params);
}

template<typename T>
void FlowT<T>::setLocalImageMatcher(LocalMatcherT<T> & localImageMatcher)
{
  mLocalImageMatcher = &localImageMatcher;
}

template<typename T>
void FlowT<T>::setLocalImageMatcherParams(int nparams, double*params)
{
  if(mLocalImageMatcher)mLocalImageMatcher->setParams(nparams, params);
}

template<typename T>
void FlowT<T>::setGlobalMatcher(FlowGlobalMatcherT<T> & flowGlobalMatcher)
{
  mFlowGlobalMatcher = &flowGlobalMatcher;
}

template<typename T>
void FlowT<T>::setGlobalMatcherParams(int nparams, double*params)
{
  if(mFlowGlobalMatcher)mFlowGlobalMatcher->setParams(nparams, params);
}

template<typename T>
void FlowT<T>::setFlowPostprocessor(FlowPostprocessor & flowPostProcessor)
{
  mFlowPostprocessor = &flowPostProcessor;
}

template<typename T>
void FlowT<T>::setFlowPostprocessorParams(int nparams, double*params)
{
  if(mFlowPostprocessor)mFlowPostprocessor->setParams(nparams, params);
}

template<typename T>
bool FlowT<T>::doOpticalFlow(const ImageAdapter & i1, const ImageAdapter & i2, double minshiftX, double maxshiftX, double minshiftY, double maxshiftY, ImageAdapter & u1, ImageAdapter & v1, ImageAdapter & o1, ImageAdapter & u2, ImageAdapter & v2,  ImageAdapter & o2)
{
  ImageT<T> mImage1, mImage2;
  ImageT<double> mU1, mV1, mO1, mU2, mV2, mO2;

  mImage1.copyFromAdapter(i1);
  mImage2.copyFromAdapter(i2);
  mU1.copyFromAdapter(u1);
  mV1.copyFromAdapter(v1);
  mO1.copyFromAdapter(o1);
  mU2.copyFromAdapter(u2);
  mV2.copyFromAdapter(v2);
  mO2.copyFromAdapter(o2);

  if(!haveEqualDimensions(mImage1, mImage2)) return false; //return if images have different dimensions	

  //output images must have the same height and width as input, but at least one channel
  if(!haveEqualHeightWidth(mImage1, mU1)) return false; 
  if(mU1.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mV1)) return false; 
  if(mV1.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mO1)) return false; 
  if(mV1.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mU2)) return false; 
  if(mU2.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mV2)) return false; 
  if(mV2.getChannels()<=0) return false;
  if(!haveEqualHeightWidth(mImage1, mO2)) return false; 
  if(mV2.getChannels()<=0) return false;

  if(mLocalImageMatcher==0) return false; //must select a local matcher
  if(mFlowGlobalMatcher==0) return false; //must select a global matcher

  mFlowGlobalMatcher->setLocalImageMatcher(*mLocalImageMatcher); //tell global matcher about local matcher

  if(mImagePairPreprocessor)mImagePairPreprocessor->preProcessImagePair(mImage1, mImage2);
  mFlowGlobalMatcher->doMatching(mImage1, mImage2, minshiftX, maxshiftX, minshiftY, maxshiftY, mU1, mV1, mO1, mU2, mV2, mO2);
  if(mFlowPostprocessor)mFlowPostprocessor->postProcessFlow(mU1, mV1, mO1, mU2, mV2, mO2);

  mU1.copyToAdapter(u1);
  mV1.copyToAdapter(v1);
  mO1.copyToAdapter(o1);
  mU2.copyToAdapter(u2);
  mV2.copyToAdapter(v2);
  mO2.copyToAdapter(o2);

  return true;
}

}
#endif //__OVFLOWT_H
