/*
    Copyright 2008 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_PNM_TAGS_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_PNM_TAGS_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief 
/// \author Christian Henning \n
///         
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include "detail/base.hpp"

namespace boost { namespace gil {

struct pnm_tag : format_tag {};

static const uint32_t pnm_type_mono_asc  = 1; // Monochrome ASCII encoding
static const uint32_t pnm_type_gray_asc  = 2; // Gray level ASCII encoding
static const uint32_t pnm_type_color_asc = 3; // sRGB color ASCII encoding
static const uint32_t pnm_type_mono_bin  = 4; // Monochrome binary encoding
static const uint32_t pnm_type_gray_bin  = 5; // Gray level binary encoding
static const uint32_t pnm_type_color_bin = 6; // sRGB color binary encoding

struct pnm_image_type : property_base< uint32_t > {};

struct pnm_image_width : property_base< uint32_t > {};

struct pnm_image_height : property_base< uint32_t > {};

struct pnm_image_max_value : property_base< uint32_t > {};


template<>
struct image_read_info< pnm_tag >
{
    pnm_image_type::type      _type;
    pnm_image_width::type     _width;
    pnm_image_height::type    _height;
    pnm_image_max_value::type _max_value;
};

template<>
struct image_read_settings< pnm_tag > : public image_read_settings_base
{
    image_read_settings< pnm_tag >()
    : image_read_settings_base()
    {}

    image_read_settings( const point_t& top_left
                       , const point_t& dim
                       )
    : image_read_settings_base( top_left
                              , dim
                              )
    {}
};


template<>
struct image_write_info< pnm_tag >
{
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_PNM_TAGS_HPP_INCLUDED
