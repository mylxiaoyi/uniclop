
#if !defined(IVIDEO_INPUT_HEADER)
#define IVIDEO_INPUT_HEADER

#include <typeinfo> // to avoid 'bad_cast not in std' error
#include <boost/gil/gil_all.hpp>

namespace uniclop {

using boost::gil::rgb8_view_t;
using boost::gil::rgb8_planar_view_t;
using boost::gil::gray8_view_t;
using boost::gil::gray8_view_t;
using boost::gil::point2;


class IVideoInput {
	
public:

	typedef point2<int> dimensions_t;

	/**
    Blocking call to retrieve a new image.
    Will copy image buffer to the given  image view.
    */
   virtual void get_new_image(rgb8_view_t &) = 0;
   virtual void get_new_image(rgb8_planar_view_t &) = 0;

   virtual void get_new_image(gray8_view_t &) = 0;

	// FIXME should be a virtual template, how to fix this ??
	//	template<typename ImageView>
	//void get_new_image(ImageView &);
	
/**
Returns the dimensions of the input image
*/
	virtual const dimensions_t &get_image_dimensions() = 0;
	
		
};


} // end of namespace uniclop


#endif // !defined(IVIDEO_INPUT_HEADER)
