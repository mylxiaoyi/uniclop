#ifndef __OVIMAGEPAIRPREPROCESSORT_H
#define __OVIMAGEPAIRPREPROCESSORT_H

#include "ImageT.hpp"


namespace Openvis3d {

///Abstract Base Class Template for preprocessing a pair of images.
/** 
* The ImagePairPreprocessorT class defines a basic interface for preprocessing a pair of images.
* Subclasses can implement the interface to provide a variety of preprocessors 
* (e.g., contrast alignment, color to gray, median filtering, etc.)
*
* @author Abhijit Ogale
*/
template<typename T>
class ImagePairPreprocessorT
{
public:

  ImagePairPreprocessorT(); /**< Default constructor with no parameters */

  virtual ~ImagePairPreprocessorT(); /**< Destructor */

  /**
  * Main method for preprocessing an image pair.
  * Note: This method modifies the input images, so be careful.
  * @param i1 the first image
  * @param i2 the second image
  * @return true if successful.
  */
  virtual bool preProcessImagePair(ImageT<T> & i1, ImageT<T> & i2) = 0;

  /**
  * Used for specifying any parameters required.
  * @param nparams number of parameters which are being passed
  * @param params the values of the parameters
  * @return true if successful.
  */
  virtual bool setParams(int nparams, double*params) = 0;
};

template<typename T>
ImagePairPreprocessorT<T>::ImagePairPreprocessorT()
{
}

template<typename T>
ImagePairPreprocessorT<T>::~ImagePairPreprocessorT()
{
}

}
#endif //__OVIMAGEPAIRPREPROCESSORT_H
