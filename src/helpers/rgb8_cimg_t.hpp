
#if !defined(RGB8_CIMG_T_HEADER)
#define RGB8_CIMG_T_HEADER

#include <typeinfo>

#include <boost/gil/gil_all.hpp>

#include <boost/cstdint.hpp>

#include <CImg/CImg.h>



namespace cimg_library
{

using boost::uint8_t;
using boost::gil::image_view;
using boost::gil::rgb8_planar_image_t;

/**
Helper class that allows to exchange between image_view and CImg
*/
class rgb8_cimg_t : public CImg<uint8_t>
{

    rgb8_planar_image_t planar_image;
public:

    typedef rgb8_planar_image_t::view_t view_t;
    view_t view;
	
	typedef CImg<uint8_t> cimg_t;

    rgb8_cimg_t(const int width, const int height);
    rgb8_cimg_t(const boost::gil::point2<int> &size);
    ~rgb8_cimg_t();

    template<typename T>
    void assign(boost::gil::image_view<T> &view);

    template<typename T>
    void operator=(boost::gil::image_view<T> &view);

private:
    void init(const int width, const int height);
};

}


#endif // !defined(RGB8_CIMG_T_HEADER)