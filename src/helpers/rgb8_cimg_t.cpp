

#include "rgb8_cimg_t.hpp"

#include <typeinfo>

#include <boost/gil/gil_all.hpp>


namespace cimg_library {
	
	using boost::gil::planar_view_get_raw_data;
	using boost::gil::image_view;
	using boost::gil::rgb8_image_t;
	using boost::gil::rgb8_view_t;
	using boost::gil::rgb8_planar_image_t;
	using boost::gil::rgb8_planar_view_t;
	
	rgb8_cimg_t::rgb8_cimg_t(const int width, const int height) {
		
		init(width, height);
		return;
	}
	
	rgb8_cimg_t::rgb8_cimg_t(const boost::gil::point2<int> &size) {
		
		init(size.x, size.y);
		return;	
	}
	
	void rgb8_cimg_t::init(const int width, const int height) {
	
		planar_image.recreate(width, height);
	 	view = boost::gil::view(planar_image);
	   
		const uint8_t *data_p =  planar_view_get_raw_data(view, 0);
		static_cast< CImg<uint8_t> *>(this)->assign( data_p, width, height, 1, 3, true );
	  	
	  	return;
	}


	rgb8_cimg_t::~rgb8_cimg_t() {
		// nothing to do here
		return;	
	}
	
	template<typename T>
	void rgb8_cimg_t::assign(image_view<T> &reference_view) {
	
		copy_pixels(reference_view, view);
		
		return;	
	}
	
	template<typename T>
	void rgb8_cimg_t::operator=(boost::gil::image_view<T> &reference_view) {
		assign(reference_view);
		return;	
	}
	
	
	typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<boost::gil::pixel<unsigned char, boost::gil::layout<boost::mpl::vector3<boost::gil::red_t, boost::gil::green_t, boost::gil::blue_t>, boost::mpl::range_c<int, 0, 3> > > const*> > locator_type_a;
	template void rgb8_cimg_t::assign<rgb8_view_t::locator>(image_view<rgb8_view_t::locator> &);	
	template void rgb8_cimg_t::assign<rgb8_planar_view_t::locator>(image_view<rgb8_planar_view_t::locator> &);	
	template void rgb8_cimg_t::assign<locator_type_a>(image_view<locator_type_a> &);	
	
	template void rgb8_cimg_t::operator=<rgb8_view_t::locator>(image_view<rgb8_view_t::locator> &);	
	template void rgb8_cimg_t::operator=<rgb8_planar_view_t::locator>(image_view<rgb8_planar_view_t::locator> &);	
	template void rgb8_cimg_t::operator=<locator_type_a>(image_view<locator_type_a> &);	
	
}