/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include "base.hpp"
#include "dynamic_io_new.hpp"
#include "io_device.hpp"
#include "path_spec.hpp"
#include "conversion_policies.hpp"

namespace boost{ namespace gil {

// ------ Image --- Reader -------- no conversion -----------

/// \ingroup IO
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              , is_read_supported      < typename get_pixel_type< typename Image::view_t >::type
                                                                       , FormatTag
                                                                       >
                                              >
                                   >::type* ptr = 0
               )
{
    detail::reader< Device
                  , FormatTag
                  , detail::read_and_no_convert
                  > reader( file );

    reader.init_image( img
                     , settings
                     , reader.get_info()
                     );

    reader.apply( view( img ));
}

template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* ptr = 0
                      )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;

    device_type dev(file);
    detail::reader< device_type
                  , FormatTag
                  , detail::read_and_no_convert
                  > reader( dev );

    reader.init_image( img
                     , settings
                     , reader.get_info()
                     );

    reader.apply( view( img ));
}

template < typename String
         , typename Image
         , typename FormatTag
         >
inline
void read_image( const String&                           file_name
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* ptr = 0
               )
{
    typedef typename detail::file_stream_device<FormatTag>::read_tag read_tag;

    detail::file_stream_device<FormatTag> device( detail::convert_to_string( file_name )
                                                , read_tag()
                                                );

    read_image( device
              , img
              , settings
              );
}

/// \ingroup IO
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&          file
               , Image&           img
               , const FormatTag& tag
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              , is_read_supported      < typename get_pixel_type< typename Image::view_t >::type
                                                                       , FormatTag
                                                                       >
                                              >
                                   >::type* ptr = 0
               )
{
    read_image( file
              , img
              , image_read_settings< FormatTag >()
              );
}

/// \ingroup IO
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&          file
               , Image&           img
               , const FormatTag& tag
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* ptr = 0
               )
{
    typedef typename detail::is_adaptable_input_device< FormatTag, Device >::device_type device_type;
    device_type dev( file );

    read_image( dev
              , img
              , image_read_settings< FormatTag >()
              );
}

/// \ingroup IO
template < typename String
         , typename Image
         , typename FormatTag
         >
inline
void read_image( const String&    file_name
               , Image&           img
               , const FormatTag& tag
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* ptr = 0
               )
{
    detail::file_stream_device< FormatTag > device( detail::convert_to_string( file_name )
                                                  , typename detail::file_stream_device< FormatTag >::read_tag()
                                                  );

    read_image( device
              , img
              , image_read_settings< FormatTag >()
              );
}

///////////////////////////// dynamic images

template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              >
                                   >::type* ptr = 0
               )
{
    detail::dynamic_image_reader< Device
                                , FormatTag
                                > dyn_reader( file );

    dyn_reader.init_image( images
                         , settings
                         , dyn_reader.get_info()
                         );

    dyn_reader.apply( images );
}

template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* ptr = 0
                      )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;
    device_type dev( file );

    detail::reader< device_type
                  , FormatTag
                  , detail::read_and_no_convert
                  > reader( dev );

    reader.init_image( images
                     , settings
                     , reader.get_info
                     );

    reader.apply( view( img ));
}

template < typename String
         , typename Images
         , typename FormatTag
         >
inline
void read_image( const String&                           file_name
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* ptr = 0
               )
{
    typedef typename detail::file_stream_device< FormatTag >::read_tag read_tag;

    detail::file_stream_device<FormatTag> device( detail::convert_to_string( file_name )
                                                , read_tag()
                                                );

    read_image( device
              , images
              , settings
              );
}

template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&              file
               , any_image< Images >& images
               , const FormatTag&     tag
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* ptr = 0
               )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;
    device_type dev( file );

    read_image( dev
              , images
              , image_read_settings< FormatTag >()
              );
}

/// \ingroup IO
template < typename String
         , typename Images
         , typename FormatTag
         >
inline
void read_image( const String&        file_name
               , any_image< Images >& images
               , const FormatTag&     tag
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* ptr = 0
               )
{
    detail::file_stream_device< FormatTag > device( detail::convert_to_string( file_name )
                                                  , typename detail::file_stream_device< FormatTag >::read_tag()
                                                  );

    read_image( device
              , images
              , image_read_settings< FormatTag >()
              );
}

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP_INCLUDED
