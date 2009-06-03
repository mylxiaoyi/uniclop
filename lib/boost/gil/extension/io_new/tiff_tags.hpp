/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP
#define BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief All supported tiff tags by the gil io extension.
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

/// For a description see: http://www.awaresystems.be/imaging/tiff/tifftags/baseline.html
/// see http://www.remotesensing.org/libtiff/
/// or http://www.libtiff.org/man/


extern "C" {
#include "tiff.h"
}

#include "detail/base.hpp"
#include "formats/tiff/log.hpp"

namespace boost { namespace gil {

template< typename T, int Value >
struct tiff_property_base : property_base< T >
{
    static const unsigned int tag = Value;
};

struct tiff_tag : format_tag {};

struct tiff_new_subfile_type : tiff_property_base< uint32_t, TIFFTAG_SUBFILETYPE > {};

struct tiff_subfile_type : tiff_property_base< uint16_t, TIFFTAG_OSUBFILETYPE > {};

struct tiff_image_width : tiff_property_base< uint32_t, TIFFTAG_IMAGEWIDTH > {};

struct tiff_image_height : tiff_property_base< uint32_t, TIFFTAG_IMAGELENGTH > {};

struct tiff_bits_per_sample : tiff_property_base< uint16_t, TIFFTAG_BITSPERSAMPLE > {};

struct tiff_compression : tiff_property_base< uint16_t, TIFFTAG_COMPRESSION > {};

struct tiff_photometric_interpretation : tiff_property_base< uint16_t, TIFFTAG_PHOTOMETRIC > {};

struct tiff_thresholding : tiff_property_base< uint16_t, TIFFTAG_THRESHHOLDING > {};

struct tiff_cell_width : tiff_property_base< uint16_t, TIFFTAG_CELLWIDTH > {};

struct tiff_cell_length : tiff_property_base< uint16_t, TIFFTAG_CELLLENGTH > {};

struct tiff_fill_order : tiff_property_base< std::string, TIFFTAG_FILLORDER > {};

struct tiff_image_description : tiff_property_base< std::string, TIFFTAG_IMAGEDESCRIPTION > {};

struct tiff_make : tiff_property_base< std::string, TIFFTAG_MAKE > {};

struct tiff_model : tiff_property_base< std::string, TIFFTAG_MODEL > {};

struct tiff_orientation : tiff_property_base< uint16_t, TIFFTAG_ORIENTATION > {};

struct tiff_samples_per_pixel : tiff_property_base< uint16_t, TIFFTAG_SAMPLESPERPIXEL > {};

struct tiff_rows_per_strip : tiff_property_base< uint32_t, TIFFTAG_ROWSPERSTRIP > {};

struct tiff_min_sample_value : tiff_property_base< uint16_t, TIFFTAG_MINSAMPLEVALUE > {};

struct tiff_max_sample_value : tiff_property_base< uint16_t, TIFFTAG_MAXSAMPLEVALUE > {};

struct tiff_x_resolution : tiff_property_base< float, TIFFTAG_XRESOLUTION > {};

struct tiff_y_resolution : tiff_property_base< float, TIFFTAG_YRESOLUTION > {};

struct tiff_planar_configuration : tiff_property_base< uint16_t, TIFFTAG_PLANARCONFIG > {};

struct tiff_gray_response_unit : tiff_property_base< uint16_t, TIFFTAG_GRAYRESPONSEUNIT > {};

struct tiff_gray_response_curve : tiff_property_base< uint16_t*, TIFFTAG_GRAYRESPONSECURVE > {};

struct tiff_resolution_unit : tiff_property_base< uint16_t, TIFFTAG_RESOLUTIONUNIT > {};

struct tiff_software : tiff_property_base< std::string, TIFFTAG_SOFTWARE > {};

struct tiff_date_time : tiff_property_base< std::string, TIFFTAG_DATETIME > {};

struct tiff_artist : tiff_property_base< std::string, TIFFTAG_ARTIST > {};

struct tiff_host_computer : tiff_property_base< std::string, TIFFTAG_HOSTCOMPUTER > {};

struct tiff_color_map
{
   typedef uint16_t* red_t;
   typedef uint16_t* green_t;
   typedef uint16_t* blue_t;

   static const unsigned int tag = TIFFTAG_COLORMAP;
};

struct tiff_extra_samples : tiff_property_base< uint16_t*, TIFFTAG_EXTRASAMPLES > {};

struct tiff_copyright : tiff_property_base< std::string, TIFFTAG_COPYRIGHT > {};

// non-baseline tags

struct tiff_sample_format : tiff_property_base< uint16_t, TIFFTAG_SAMPLEFORMAT > {};

struct tiff_indexed : tiff_property_base< uint16_t, TIFFTAG_INDEXED > {};

template<>
struct image_read_info< tiff_tag >
{
   tiff_image_width::type  _width;
   tiff_image_height::type _height;

   tiff_compression::type _compression;

   tiff_bits_per_sample::type   _bits_per_sample;
   tiff_samples_per_pixel::type _samples_per_pixel;
   tiff_sample_format::type     _sample_format;

   tiff_planar_configuration::type _planar_configuration;

   tiff_photometric_interpretation::type _photometric_interpretation;
};

template<>
struct image_read_settings< tiff_tag > : public image_read_settings_base
{
    image_read_settings< tiff_tag >()
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

template< typename Log >
struct image_write_info< tiff_tag, Log >
{
    image_write_info()
    : _photometric_interpretation( PHOTOMETRIC_MINISBLACK )
    , _compression               ( COMPRESSION_NONE       )
    , _orientation               ( ORIENTATION_TOPLEFT    )
    , _planar_configuration      ( PLANARCONFIG_CONTIG    )
    {}

    tiff_photometric_interpretation::type _photometric_interpretation;
    tiff_compression::type                _compression;
    tiff_orientation::type                _orientation;
    tiff_planar_configuration::type       _planar_configuration;

    Log                                   _log;
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP
