#include "ImageAdapter.hpp"


namespace Openvis3d {

ImageAdapter::ImageAdapter()
: mHeight(0), mWidth(0), mChannels(0), mDataType(OV_DATA_UNKNOWN)
{
}

ImageAdapter::~ImageAdapter()
{
}

void ImageAdapter::getSize(int & height, int & width, int & nColorChannels) const
{
  height = mHeight;
  width = mWidth;
  nColorChannels = mChannels;
}

void ImageAdapter::getDataType(ImageAdapter::DataType & dataType) const
{
  dataType = mDataType;
}

}
