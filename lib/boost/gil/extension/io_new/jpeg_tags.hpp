/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_JPEG_TAGS_HPP_INCLUDED 
#define BOOST_GIL_EXTENSION_IO_JPEG_TAGS_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief All supported jpeg tags by the gil io extension.
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///         
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
#include <jpeglib.h>
}

#include "detail/base.hpp"

namespace boost { namespace gil {

struct jpeg_tag : format_tag {};

struct jpeg_image_width : property_base< JDIMENSION > {};

struct jpeg_image_height : property_base< JDIMENSION > {};

struct jpeg_num_components : property_base< int > {};

struct jpeg_color_space : property_base< J_COLOR_SPACE > {};

struct jpeg_quality : property_base< int > {};

struct jpeg_data_precision : property_base< int > {};

struct jpeg_dct_method : property_base< J_DCT_METHOD >
{
    static const type slow        = JDCT_ISLOW;
    static const type fast        = JDCT_IFAST;
    static const type floating_pt = JDCT_FLOAT;
    static const type fastest     = JDCT_FASTEST;
};

template<>
struct image_read_info< jpeg_tag >
{
   jpeg_image_width::type  _width;
   jpeg_image_height::type _height;

   jpeg_num_components::type _num_components;
   jpeg_color_space::type    _color_space;

    // I believe this number is always 8 in the case libjpeg is built with 8.
    // see: http://www.asmail.be/msg0055405033.html
   jpeg_data_precision::type _data_precision;
};


template<>
struct image_read_settings< jpeg_tag > : public image_read_settings_base
{
    image_read_settings<jpeg_tag>()
    : image_read_settings_base()
    , _dct_method( jpeg_dct_method::slow )
    {}

    image_read_settings( const point_t&               top_left
                       , const point_t&               dim
                       , const jpeg_dct_method::type& dct_method
                       )
    : image_read_settings_base( top_left
                              , dim
                              )
    , _dct_method()
    {}

    jpeg_dct_method::type _dct_method;
};


template<>
struct image_write_info< jpeg_tag >
{
    image_write_info( const jpeg_quality::type& quality = 100 )
    : _quality( quality )
    {}

    jpeg_quality::type _quality;
};


} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_JPEG_TAGS_HPP_INCLUDED
