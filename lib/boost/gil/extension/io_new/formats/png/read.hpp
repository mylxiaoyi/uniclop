/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_DETAIL_PNG_IO_READ_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_DETAIL_PNG_IO_READ_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/gil/extension/io_new/png_tags.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/io_new/detail/base.hpp>
#include <boost/gil/extension/io_new/detail/reader_base.hpp>
#include <boost/gil/extension/io_new/detail/conversion_policies.hpp>
#include <boost/gil/extension/io_new/detail/io_device.hpp>
#include <boost/gil/extension/io_new/detail/typedefs.hpp>
#include <boost/gil/extension/io_new/detail/row_buffer_helper.hpp>

#include "base.hpp"
#include "is_allowed.hpp"

namespace boost { namespace gil { namespace detail {

template< typename Device
        , typename ConversionPolicy
        >
class reader< Device
            , png_tag
            , ConversionPolicy
            >
    : public png_io_base< Device >
    , public reader_base< png_tag
                        , ConversionPolicy >
{
public:

    reader( Device& io_dev )
    : png_io_base< Device >( io_dev )
    , _png_ptr ( NULL )
    , _info_ptr( NULL )
    {
        this->check();
        init_reader();
    }

    reader( Device& io_dev
          , const typename ConversionPolicy::color_converter_type& cc )
    : png_io_base< Device >( io_dev )
    , reader_base< png_tag
                 , ConversionPolicy >( cc )
    , _png_ptr ( 0 )
    , _info_ptr( 0 )
    {
        this->check();
        init_reader();
    }

    ~reader()
    {
        png_destroy_read_struct( &_png_ptr
                               , &_info_ptr
                               , png_infopp_NULL
                               );
    }

    image_read_info<png_tag> get_info() const
    {
        image_read_info< png_tag > ret = { 0 };

        png_get_IHDR( _png_ptr
                    , _info_ptr
                    , &ret._width
                    , &ret._height
                    , &ret._bit_depth
                    , &ret._color_type
                    , &ret._interlace_method
                    , &ret._compression_method
                    , &ret._filter_method
                    );

        ret._num_channels = png_get_channels( _png_ptr
                                            , _info_ptr
                                            );

        int res_unit = PNG_RESOLUTION_METER;
        png_get_pHYs(_png_ptr, _info_ptr, &ret._x_res, &ret._y_res, &res_unit);
        png_get_sBIT(_png_ptr, _info_ptr, &ret._sbits );

#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED 
        if( !png_get_gAMA(_png_ptr, _info_ptr, &ret._gamma ))
        {
            ret._gamma = 1.0;
        }
#else
        png_get_gAMA_fixed(_png_ptr, _info_ptr, &ret._gamma );
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED
        return ret;
    }

    template<typename View>
    void apply( const View& view )
    {
        if( little_endian() )
        {   
            if( this->_info._bit_depth == 16 )
                png_set_swap(_png_ptr);
            if( this->_info._bit_depth < 8 )
                png_set_packswap(_png_ptr);
        }

        png_bitdepth::type bit_depth = this->_info._bit_depth;
        png_color_type::type color_type = this->_info._color_type;

        if(color_type == PNG_COLOR_TYPE_PALETTE)
        {
            color_type = PNG_COLOR_TYPE_RGB;
            bit_depth = 8;
            png_set_palette_to_rgb(_png_ptr);
        }

        if( png_get_valid( _png_ptr
                         , _info_ptr
                         , PNG_INFO_tRNS
                         )
          ) 
        {
            if( color_type == PNG_COLOR_TYPE_RGB )
            {
                color_type = PNG_COLOR_TYPE_RGBA;
                this->_info._num_channels = 4;
            }
            else if( color_type== PNG_COLOR_TYPE_GRAY )
            {
                color_type = PNG_COLOR_TYPE_GA;
                this->_info._num_channels = 2;
            }

            png_set_tRNS_to_alpha( _png_ptr );
        }

#ifdef BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED 
        png_set_gamma( _png_ptr
                     , this->_settings._gamma
                     , this->_info._gamma
                     );
#endif // BOOST_GIL_IO_PNG_FLOATING_POINT_SUPPORTED

        switch( color_type )
        {
            case PNG_COLOR_TYPE_GRAY:
            {
                if( bit_depth < 8 )
                {
                    png_set_gray_1_2_4_to_8( _png_ptr );
                }

                switch( bit_depth )
                {
                    case 1:  
                    case 2:  
                    case 4:  
                    case 8:  read_rows< gray8_pixel_t > ( view ); break;
                    case 16: read_rows< gray16_pixel_t >( view ); break;
                    default: io_error("png_reader::read_data(): unknown combination of color type and bit depth");
                }

                break;
            }
            case PNG_COLOR_TYPE_GA:
            {
                #ifdef BOOST_GIL_IO_ENABLE_GRAY_ALPHA
                switch( bit_depth )
                {
                    case 8: read_rows< gray_alpha8_pixel_t > ( view ); break;
                    case 16:read_rows< gray_alpha16_pixel_t >( view ); break;
                    default: io_error("png_reader::read_data(): unknown combination of color type and bit depth");
                }
                #else
                    io_error("gray_alpha isn't enabled. Use ENABLE_GRAY_ALPHA when building application.");
                #endif // BOOST_GIL_IO_ENABLE_GRAY_ALPHA


                break;
            }
            case PNG_COLOR_TYPE_RGB:
            {
                switch (bit_depth)
                {
                    case 8:  read_rows< rgb8_pixel_t > ( view ); break;
                    case 16: read_rows< rgb16_pixel_t >( view ); break;
                    default: io_error("png_reader::read_data(): unknown combination of color type and bit depth");
                }

                break;
            }
            case PNG_COLOR_TYPE_RGBA:
            {
                switch( bit_depth )
                {
                    case 8 : read_rows< rgba8_pixel_t > ( view ); break;
                    case 16: read_rows< rgba16_pixel_t >( view ); break;
                    default: io_error("png_reader_color_convert::read_data(): unknown combination of color type and bit depth");
                }

                break;
            }
            default: io_error("png_reader_color_convert::read_data(): unknown color type");
        }

        png_read_end( _png_ptr
                    , NULL
                    );
    }

private:

    template< typename ImagePixel
            , typename View
            >
    void read_rows( const View& view )
    {
        typedef row_buffer_helper_view< ImagePixel > row_buffer_helper_t;

        typedef typename row_buffer_helper_t::buffer_t   buffer_t;
        typedef typename row_buffer_helper_t::iterator_t it_t;

        typedef typename is_same< ConversionPolicy
                                , read_and_no_convert
                                >::type is_read_and_convert_t;

        io_error_if( !is_allowed< View >( this->_info
                                        , is_read_and_convert_t()
                                        )
                   , "Image types aren't compatible."
                   );

        bool interlaced = this->_info._interlace_method != PNG_INTERLACE_NONE;

        std::ptrdiff_t width = this->_info._width;
        std::ptrdiff_t height = this->_info._height;

        row_buffer_helper_t buffer( static_cast<int>( interlaced ? width * height : width )
                                  , false
                                  );

        if( interlaced )
        {
            std::vector< png_bytep > row_ptrs( height );
            for( int y = 0; y < height; ++y )
            {
                row_ptrs[y] = reinterpret_cast< png_bytep >( buffer.data() + y * width );
            }

            png_read_image( _png_ptr
                          , &row_ptrs.front()
                          );

            for( int y = 0; y < view.height(); ++y )
            {
                it_t begin = buffer.begin() + y * width;

                it_t first = begin + this->_settings._top_left.x;
                it_t last  = begin + this->_settings._dim.x; // one after last element

                this->_cc_policy.read( first
                                     , last
                                     , view.row_begin( y ));
            }
        }
        else
        {
            it_t begin = buffer.begin();

            it_t first = begin + this->_settings._top_left.x;
            it_t last  = begin + this->_settings._dim.x; // one after last element

            // skip rows
            for( int y = 0; y < this->_settings._top_left.y; ++y )
            {
                png_read_row( _png_ptr
                            , reinterpret_cast< png_bytep >( buffer.data() )
                            , 0
                            );
            }

            for( int y = 0; y < view.height(); ++y )
            {
                png_read_row( _png_ptr
                            , reinterpret_cast< png_bytep >( buffer.data() )
                            , 0
                            );

                 this->_cc_policy.read( first
                                      , last
                                      , view.row_begin( y ));
            }
        }
    }

    void init_reader()
    {
        _png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
        io_error_if(_png_ptr==NULL,"png_reader: fail to call png_create_write_struct()");
        // allocate/initialize the image information data
        _info_ptr = png_create_info_struct(_png_ptr);

        if (_info_ptr == NULL) {
            png_destroy_read_struct(&_png_ptr,png_infopp_NULL,png_infopp_NULL);
            io_error("png_reader: fail to call png_create_info_struct()");
        }
        if (setjmp(png_jmpbuf(_png_ptr))) {
            //free all of the memory associated with the png_ptr and info_ptr
            png_destroy_read_struct(&_png_ptr, &_info_ptr, png_infopp_NULL);
            io_error("png_reader: fail to call setjmp()");
        }

        init_io( _png_ptr );

        png_set_sig_bytes(_png_ptr,PNG_BYTES_TO_CHECK);
        png_read_info(_png_ptr, _info_ptr);
    }

    void init_io( png_structp png_ptr )
    {
        png_set_read_fn(png_ptr, 
                static_cast<void*>(&this->_io_dev), 
                static_cast<void(*)(png_structp, png_bytep, png_size_t)>(&png_io_base<Device>::read_data));
    }

    png_structp _png_ptr;
    png_infop _info_ptr;
};


struct png_type_format_checker
{
    png_type_format_checker( png_bitdepth::type   bit_depth
                           , png_color_type::type color_type
                           )
    : _bit_depth ( bit_depth  )
    , _color_type( color_type )
    {}

    template< typename Image >
    bool apply()
    {
        typedef is_read_supported< typename get_pixel_type< Image::view_t >::type
                                 , png_tag
                                 > is_supported_t;

        return is_supported_t::_bit_depth  == _bit_depth
            && is_supported_t::_color_type == _color_type;
    }

private:

    png_bitdepth::type   _bit_depth;
    png_color_type::type _color_type;
};

struct png_read_is_supported
{
    template< typename View >
    struct apply : public is_read_supported< typename get_pixel_type< View >::type
                                           , png_tag
                                           >
    {};
};

template< typename Device
        >
class dynamic_image_reader< Device
                          , png_tag
                          > 
    : public reader< Device
                   , png_tag
                   , detail::read_and_no_convert
                   >
{
    typedef reader< Device
                  , png_tag
                  , detail::read_and_no_convert
                  > parent_t;

public:

    dynamic_image_reader( Device& device )
    : reader( device )
    {}    

    template< typename Images >
    void apply( any_image< Images >& images )
    {
        png_type_format_checker format_checker( _info._bit_depth
                                              , _info._color_type
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

            dynamic_io_fnobj< png_read_is_supported
                            , parent_t
                            > op( this );

            apply_operation( view( images )
                           , op
                           );
        }
    }
};

} // namespace detail
} // namespace gil
} // namespace boost



#endif // BOOST_GIL_EXTENSION_IO_DETAIL_PNG_IO_READ_HPP_INCLUDED
