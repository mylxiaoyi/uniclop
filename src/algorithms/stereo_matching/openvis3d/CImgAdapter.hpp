
#if !defined(CIMG_ADAPTER_HEADER_INCLUDED)
#define CIMG_ADAPTER_HEADER_INCLUDED

#include "CImg/CImg.h"
using namespace cimg_library;

#include "ImageAdapter.hpp"

#include <stdexcept> // for runtime_error
#include <boost/cstdint.hpp> 

template<typename T>
class CImgAdapter :	public ImageAdapter
{

  CImg<T> * const image_p;
  const CImg<T> &const_image_ref;

public:
  explicit CImgAdapter(CImg<T> &image);
  explicit CImgAdapter(const CImg<T> &image);
  virtual ~CImgAdapter();

  
  void getSize(int & height, int & width, int & nColorChannels) const; 
  void getDataType(ImageAdapter::DataType & dataType) const;

  double getPixel(int row, int column, int channel) const;
  void   setPixel(double value, int row, int column, int channel);

};

// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
// Class CImgAdapter methods implementation

template <typename T>
CImgAdapter<T>::CImgAdapter(CImg<T> &image)
: ImageAdapter(), image_p(&image), const_image_ref(image)
{
	return;
}

template <typename T>
CImgAdapter<T>::CImgAdapter(const CImg<T> &image)
: ImageAdapter(), image_p(NULL), const_image_ref(image)
{
	return;
}


template <typename T>
CImgAdapter<T>::~CImgAdapter()
{
	// nothing to do
	return;
}


template <typename T>
void CImgAdapter<T>::getSize(int & height, int & width, int & nColorChannels) const
{
  // for some strange reason coordinates needs to be transposed

  height = const_image_ref.dimy(); width = const_image_ref.dimx();
  //height = image_ref.dimx(); width = image_ref.dimy();
  nColorChannels = const_image_ref.dimv();
  return;
}


template <typename T>
void CImgAdapter<T>::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = OV_DATA_UNKNOWN;
  return; 
}

template <>
void CImgAdapter<boost::uint8_t>::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = OV_DATA_UINT8;
  return; 
}

template <>
void CImgAdapter<float>::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = OV_DATA_FLOAT32;
  return; 
}

template <>
void CImgAdapter<double>::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = OV_DATA_DOUBLE64;
  return; 
}

template <>
void CImgAdapter<int>::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = OV_DATA_INT32;
  return; 
}

//     OV_DATA_UINT8,   /**< unsigned char */
//     OV_DATA_INT8,	 /**< char */
//     OV_DATA_UINT16,  /**< short */
//     OV_DATA_INT16,   /**< unsigned short */
//     OV_DATA_UINT32,  /**< int */
//     OV_DATA_UINT64,  /**< unsigned long long */
//     OV_DATA_INT64,   /**< long long */



template <typename T>
inline double CImgAdapter<T>::getPixel(int row, int column, int channel) const
{
  // for some strange reason coordinates needs to be transposed
  return static_cast<double>( const_image_ref(column, row, 0, channel) );
  //return static_cast<double>( image_ref(row, column, 0, channel) );
}


template <typename T>
inline void CImgAdapter<T>::setPixel(double value, int row, int column, int channel)
{
	if(image_p != NULL)
	  // for some strange reason coordinates needs to be transposed
  	 (*image_p)(column, row, 0, channel) = static_cast<T>(value);
  	//(*image_p)(row, column, 0, channel) = static_cast<T>(value);
  	else
  		throw std::runtime_error("Calling CImgAdapter<T>::setPixel over a const instance");
  return;
}

// end of CImgAdapter methods implementation
// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~

#endif //CIMG_ADAPTER_HEADER_INCLUDED

