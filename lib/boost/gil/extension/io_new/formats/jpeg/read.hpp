/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_JPEG_IO_READ_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_JPEG_IO_READ_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <boost/gil/extension/io_new/jpeg_tags.hpp>

#include <boost/gil/extension/io_new/detail/base.hpp>
#include <boost/gil/extension/io_new/detail/conversion_policies.hpp>
#include <boost/gil/extension/io_new/detail/reader_base.hpp>
#include <boost/gil/extension/io_new/detail/io_device.hpp>
#include <boost/gil/extension/io_new/detail/typedefs.hpp>

#include "is_allowed.hpp"

namespace boost { namespace gil { namespace detail {

template<typename Device>
struct jpeg_decompress_mgr
{
    jpeg_decompress_mgr( Device & file )
        : in(file)
    {
        _cinfo.err = jpeg_std_error( &_jerr );

        _src._jsrc.bytes_in_buffer = 0;
        _src._jsrc.next_input_byte = buffer;
        _src._jsrc.init_source = reinterpret_cast<void(*)(j_decompress_ptr)>(&jpeg_decompress_mgr<Device>::init_device);
        _src._jsrc.fill_input_buffer = reinterpret_cast<boolean(*)(j_decompress_ptr)>(&jpeg_decompress_mgr<Device>::fill_buffer);
        _src._jsrc.skip_input_data = reinterpret_cast<void(*)(j_decompress_ptr, long num_bytes)>(&jpeg_decompress_mgr<Device>::skip_input_data);
        _src._jsrc.resync_to_restart= jpeg_resync_to_restart;
        _src._jsrc.term_source= reinterpret_cast<void(*)(j_decompress_ptr)>(&jpeg_decompress_mgr<Device>::close_device);
        _src._this = this;


        jpeg_create_decompress( &_cinfo );

        _cinfo.src = &_src._jsrc;

        jpeg_read_header( &_cinfo
                        , TRUE    );

        io_error_if( _cinfo.data_precision != 8, "Image file is not supported." );
    }

    ~jpeg_decompress_mgr()
    {
        jpeg_destroy_decompress( &_cinfo );
    }

    const jpeg_decompress_struct& get() const
    {
        return _cinfo;
    }

    jpeg_decompress_struct& get()
    {
        return _cinfo;
    }

protected:

    void start_decompress()
    {
        if( jpeg_start_decompress( &_cinfo ) == false )
        {
            io_error( "Cannot start decompression." );
        }
    }

    void finish_decompress()
    {
        jpeg_finish_decompress ( &_cinfo );
    }

private:

    static void init_device( jpeg_decompress_struct * cinfo )
    {
        gil_jpeg_source_mgr * src = reinterpret_cast<gil_jpeg_source_mgr*>(cinfo->src);
        src->_jsrc.bytes_in_buffer = 0;
        src->_jsrc.next_input_byte = src->_this->buffer;
    }
    static boolean fill_buffer( jpeg_decompress_struct * cinfo )
    {
        gil_jpeg_source_mgr * src = reinterpret_cast<gil_jpeg_source_mgr*>(cinfo->src);
        size_t count= src->_this->in.read(src->_this->buffer, sizeof(src->_this->buffer) );
        if( count <= 0 ) 
        {
            // libjpeg does that: adding an EOF marker 
            src->_this->buffer[0] = (JOCTET) 0xFF;
            src->_this->buffer[1] = (JOCTET) JPEG_EOI;
            count = 2;
        }

        src->_jsrc.next_input_byte = src->_this->buffer;
        src->_jsrc.bytes_in_buffer = count;

        return TRUE;
    }
    static void skip_input_data( jpeg_decompress_struct * cinfo, long num_bytes  )
    {
        gil_jpeg_source_mgr * src = reinterpret_cast<gil_jpeg_source_mgr*>(cinfo->src);

        if (num_bytes > 0) 
        {
            if( num_bytes > long(src->_jsrc.bytes_in_buffer) )
            {
                src->_jsrc.bytes_in_buffer = 0;
                src->_this->in.seek( num_bytes, SEEK_CUR);
                num_bytes = 0;
            }
            src->_jsrc.next_input_byte += num_bytes;
            src->_jsrc.bytes_in_buffer -= num_bytes;
        }
    }

    static void close_device( jpeg_decompress_struct * cinfo )
    {
    }

protected:
    jpeg_decompress_struct _cinfo;
    jpeg_error_mgr         _jerr;

private:
    Device &in;

    struct gil_jpeg_source_mgr
    {
        jpeg_source_mgr      _jsrc;
        jpeg_decompress_mgr* _this;
    };

    gil_jpeg_source_mgr _src;
    JOCTET buffer[1024];
};

template< typename Device
        , typename ConversionPolicy
        >
class reader< Device
            , jpeg_tag
            , ConversionPolicy
            > 
    : public jpeg_decompress_mgr< Device >
    , public reader_base< jpeg_tag
                        , ConversionPolicy >
{
public:
    reader( Device& device )
    : jpeg_decompress_mgr<Device>( device )
    {}

    reader( Device& device
          , const typename ConversionPolicy::color_converter_type& cc
          )
    : jpeg_decompress_mgr< Device >( device )
    , reader_base< jpeg_tag
                 , ConversionPolicy >( cc )
    {}

    image_read_info<jpeg_tag> get_info()
    {
        image_read_info<jpeg_tag> ret;
        ret._width          = this->_cinfo.image_width;
        ret._height         = this->_cinfo.image_height;
        ret._num_components = this->_cinfo.num_components;
        ret._color_space    = this->_cinfo.jpeg_color_space;
        ret._data_precision = this->_cinfo.data_precision;

        return ret;
    }

    template<typename View>
    void apply( const View& view )
    {
        jpeg_decompress_struct& cinfo = this->get();
        cinfo.dct_method = this->_settings._dct_method;

        typedef typename is_same< ConversionPolicy
                                , read_and_no_convert
                                >::type is_read_and_convert_t;

        io_error_if( !is_allowed< View >( this->_info
                                        , is_read_and_convert_t()
                                        )
                   , "Image types aren't compatible."
                   );

        this->start_decompress();

        switch( this->_info._color_space )
        {
        case JCS_GRAYSCALE:
            io_error_if(this->_info._num_components!=1,"reader<jpeg>: error in image data");
            read_rows<gray8_pixel_t>( view );
            break;
        case JCS_RGB:
            io_error_if(this->_info._num_components!=3,"reader<jpeg>: error in image data");
            read_rows<rgb8_pixel_t>( view );
        case JCS_YCbCr:
            io_error_if(this->_info._num_components!=3,"reader<jpeg>: error in image data");
            //!\todo add Y'CbCr? We loose image quality when reading JCS_YCbCr as JCS_RGB
            read_rows<rgb8_pixel_t>( view );
            break;
        case JCS_CMYK:
            io_error_if(this->_info._num_components!=4,"reader<jpeg>: error in image data");
            read_rows<cmyk8_pixel_t>( view );
            break;
        case JCS_YCCK:
            io_error_if(this->_info._num_components!=4,"reader<jpeg>: error in image data");
            //!\todo add Y'CbCrK? We loose image quality when reading JCS_YCCK as JCS_CMYK
            this->_cinfo.out_color_space = JCS_CMYK;
            read_rows<cmyk8_pixel_t>( view );
            break;
        default:
            break;
            // unknown
        }

        this->finish_decompress();
    }

private:

    template< typename ImagePixel
            , typename View
            >
    void read_rows( const View& view )
    {
        typedef std::vector<ImagePixel> buffer_t;

        buffer_t buffer( this->_info._width );

        JSAMPLE *row_adr = reinterpret_cast< JSAMPLE* >( &buffer[0] );

        //Skip scanlines if necessary.
        for( int y = 0; y <  this->_settings._top_left.y; ++y )
        {
            io_error_if( jpeg_read_scanlines( &this->_cinfo
                                         , &row_adr
                                         , 1
                                         ) !=1
                       , "jpeg_read_scanlines: fail to read JPEG file"
                       );
        }

        // Read data.
        for( int y = 0; y < view.height(); ++y )
        {
            io_error_if( jpeg_read_scanlines( &this->_cinfo
                                         , &row_adr
                                         , 1
                                         ) !=1
                       , "jpeg_read_scanlines: fail to read JPEG file"
                       );

            typename buffer_t::iterator beg = buffer.begin() + this->_settings._top_left.x;
            typename buffer_t::iterator end = beg + this->_settings._dim.x;

            this->_cc_policy.read( beg
                                 , end
                                 , view.row_begin( y )
                                 );
        }

        //@todo: Finish up. There might be a better way to do that.
        while( this->_cinfo.output_scanline <  this->_cinfo.image_height )
        {
            io_error_if( jpeg_read_scanlines( &this->_cinfo
                                            , &row_adr
                                            , 1
                                            ) !=1
                       , "jpeg_read_scanlines: fail to read JPEG file"
                       );
        }

    }
};

struct jpeg_type_format_checker
{
    jpeg_type_format_checker( jpeg_color_space::type color_space )
    : _color_space( color_space )
    {}

    template< typename Image >
    bool apply()
    {
        return is_read_supported< typename get_pixel_type< Image::view_t >::type
                                , jpeg_tag
                                >::_color_space == _color_space;
    }

private:

    jpeg_color_space::type _color_space;
};

struct jpeg_read_is_supported
{
    template< typename View > 
    struct apply : public is_read_supported< typename get_pixel_type< View >::type
                                           , jpeg_tag
                                           >
    {};
};

template< typename Device
        >
class dynamic_image_reader< Device
                          , jpeg_tag
                          > 
    : public reader< Device
                   , jpeg_tag
                   , detail::read_and_no_convert
                   >
{
    typedef reader< Device
                  , jpeg_tag
                  , detail::read_and_no_convert
                  > parent_t;

public:

    dynamic_image_reader( Device& device )
    : reader( device )
    {}    

    template< typename Images >
    void apply( any_image< Images >& images )
    {
        jpeg_type_format_checker format_checker( _info._color_space != JCS_YCbCr 
                                               ? _info._color_space
                                               : JCS_RGB
                                               );

        if( !construct_matched( images
                              , format_checker
                              ))
        {
            io_error( "No matching image type between those of the given any_image and that of the file" );
        }
        else
        {
            init_image( images
                      , _settings
                      , _info
                      );

            dynamic_io_fnobj< jpeg_read_is_supported
                            , parent_t
                            > op( this );

            apply_operation( view( images )
                           , op
                           );
        }
    }
};

} // detail
} // gil
} // boost

#endif // BOOST_GIL_EXTENSION_IO_JPEG_IO_READ_HPP_INCLUDED
