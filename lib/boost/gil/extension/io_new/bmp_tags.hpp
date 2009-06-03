/*
    Copyright 2008 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_BMP_TAGS_HPP_INCLUDED 
#define BOOST_GIL_EXTENSION_IO_BMP_TAGS_HPP_INCLUDED

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

struct bmp_tag : format_tag {};

// the offset, i.e. starting address, of the byte where the bitmap data can be found.
struct bmp_offset : property_base< uint32_t > {};

// the size of this header:
// - 40 bytes for Windows V3 header
// - 12 bytes for OS/2 V1 header
struct bmp_header_size : property_base< uint32_t > {};

// The bitmap width in pixels (signed integer).
struct bmp_image_width : property_base< int32_t > {};

// The bitmap height in pixels (signed integer).
struct bmp_image_height : property_base< int32_t > {};

// The number of bits per pixel, which is the color depth of the image.
// Typical values are 1, 4, 8, 16, 24 and 32.
struct bmp_bits_per_pixel : property_base< uint16_t > {};

// The compression method being used.
static const uint32_t ct_rgb      = 0; // RGB without compression
static const uint32_t ct_rle8     = 1; // 8 bit index with RLE compression
static const uint32_t ct_rle4     = 2; // 4 bit index with RLE compression
static const uint32_t ct_bitfield = 3; // 16 or 32 bit fields without compression

struct bmp_compression : property_base< uint32_t > {};

// The image size. This is the size of the raw bitmap data (see below), 
// and should not be confused with the file size.
struct bmp_image_size : property_base< uint32_t > {};

// The horizontal resolution of the image. (pixel per meter, signed integer)
struct bmp_horizontal_resolution : property_base< int32_t > {};

// The vertical resolution of the image. (pixel per meter, signed integer)
struct bmp_vertical_resolution : property_base< int32_t > {};

// The number of colors in the color palette, or 0 to default to 2^n - 1.
struct bmp_num_colors : property_base< uint32_t > {};


// The number of important colors used, or 0 when every color is important; 
// generally ignored.
struct bmp_num_important_colors : property_base< uint32_t > {};


template<>
struct image_read_info< bmp_tag >
{
    image_read_info< bmp_tag >()
    : _valid( false )
    {}

    bmp_offset::type _offset;

    bmp_header_size::type _header_size;

    bmp_image_width::type  _width;
    bmp_image_height::type _height;

    bmp_bits_per_pixel::type _bits_per_pixel;

    bmp_compression::type _compression;

    bmp_image_size::type _image_size;

    bmp_horizontal_resolution::type _horizontal_resolution;
    bmp_vertical_resolution::type   _vertical_resolution;

    bmp_num_colors::type            _num_colors;
    bmp_num_important_colors::type  _num_important_colors;

    bool _valid;
};


template<>
struct image_read_settings< bmp_tag > : public image_read_settings_base
{
    image_read_settings< bmp_tag >()
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
struct image_write_info< bmp_tag >
{
};

static const uint32_t bmp_header_size     = 14;
static const uint32_t bmp_win32_info_size = 40;
static const uint32_t bmp_os2_info_size   = 12;
static const uint32_t bmp_signature       = 0x4D42;

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_BMP_TAGS_HPP_INCLUDED
