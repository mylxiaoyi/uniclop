/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_PNG_TAGS_HPP
#define BOOST_GIL_EXTENSION_IO_PNG_TAGS_HPP 

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///         
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
#include <png.h>
}
#include "detail/base.hpp"

namespace boost { namespace gil {

struct png_tag : format_tag {};

struct png_image_width : property_base< png_uint_32 > {};

struct png_image_height : property_base< png_uint_32 > {};

struct png_interlace_method : property_base< int > {};

struct png_compression_method : property_base< int > {};

struct png_filter_method : property_base< int > {};

struct png_bitdepth : property_base< int > {};

struct png_num_channels : property_base< png_byte > {};

struct png_color_type : property_base< int > {};

struct png_significant_bits : property_base< png_color_8p > {};

struct png_x_resolution : property_base< png_uint_32 > {};

struct png_y_resolution : property_base< png_uint_32 > {};

/// @todo srgb intent + gamma ?
#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    struct png_gamma : property_base< double > {};
#else 
    struct png_gamma : property_base< png_fixed_point > {};
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED

template<>
struct image_read_info< png_tag >
{
    png_image_width::type  _width;
    png_image_height::type _height;

    png_num_channels::type _num_channels;
    png_bitdepth::type     _bit_depth;
    png_color_type::type   _color_type;

    png_interlace_method::type   _interlace_method;
    png_compression_method::type _compression_method;
    png_filter_method::type      _filter_method;


    png_gamma::type            _gamma;
    png_x_resolution::type     _x_res;
    png_y_resolution::type     _y_res;
    png_significant_bits::type _sbits;
};

template<>
struct image_read_settings< png_tag > : public image_read_settings_base
{
    image_read_settings< png_tag >()
    : image_read_settings_base()
#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    , _gamma( 1.0 )
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    {}

    image_read_settings( const point_t& top_left
                       , const point_t& dim
#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
                       , double         gamma = 1.0
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
                       )
    : image_read_settings_base( top_left
                              , dim
                              )
#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    , _gamma( gamma )
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    {}

#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
    png_gamma::type _gamma;
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
};

template<>
struct image_write_info<png_tag>
{
    image_write_info()
    : _interlace_method  ( PNG_INTERLACE_NONE           )
    , _compression_method( PNG_COMPRESSION_TYPE_DEFAULT )
    , _filter_method     ( PNG_FILTER_TYPE_DEFAULT      )

    , _gamma( 0 )
    , _x_res( 0 )
    , _y_res( 0 )
    , _sbits( NULL )
    {}

    png_interlace_method::type   _interlace_method;
    png_compression_method::type _compression_method;
    png_filter_method::type      _filter_method;

    png_gamma::type            _gamma;
    png_x_resolution::type     _x_res;
    png_y_resolution::type     _y_res;
    png_significant_bits::type _sbits;
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_PNG_TAGS_HPP
