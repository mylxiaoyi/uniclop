/*
    Copyright 2007-2008 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_DETAIL_READER_BASE_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_DETAIL_READER_BASE_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include "base.hpp"

namespace boost { namespace gil { namespace detail {

template< typename FormatTag
        , typename ConversionPolicy
        >
struct reader_base
{
public:

    template< typename Image >
    void init_image( Image&                                  img
                   , const image_read_settings< FormatTag >& settings
                   , const image_read_info< FormatTag >&     info
                   )
    {
        _settings = settings;
        _info     = info;

        setup( _settings._dim );

        img.recreate( _settings._dim.x
                    , _settings._dim.y
                    );
    }

    template< typename View >
    void init_view( const View&                             view
                  , const image_read_settings< FormatTag >& settings
                  , const image_read_info< FormatTag >&     info
                  )
    {
        _settings = settings;
        _info     = info;

        setup( view.dimensions() );
    }

protected:

    reader_base() {}
    reader_base( const typename ConversionPolicy::color_converter_type& cc )
    : _cc_policy( cc )
    {}

private:

    void setup( const point_t& dim )
    {
        check_coordinates( dim );

        if( dim == point_t( 0, 0 ))
        {
            _settings._dim.x = _info._width;
            _settings._dim.y = _info._height;
        }
        else
        {
            _settings._dim = dim;
        }
    }

    void check_coordinates( const point_t& dim )
    {
       typedef point_t::value_type int_t;

       int_t width  = static_cast< int_t >( _info._width  );
       int_t height = static_cast< int_t >( _info._height );

       io_error_if( (  _settings._top_left.x < 0
                    || _settings._top_left.y < 0
                    || dim.x < 0
                    || dim.y < 0
                    )
                 , "User provided view has incorrect size." );


       io_error_if( (  ( width  ) <  _settings._top_left.x
                    && ( width  ) <= dim.x
                    && ( height ) <  _settings._top_left.y
                    && ( height ) <= dim.y  )
                 , "User provided view has incorrect size."       );

       io_error_if( (  _settings._top_left.x > dim.x
                    && _settings._top_left.y > dim.y
                    )
                 , "User provided view has incorrect size." );

       io_error_if( (  ( _settings._top_left.x + dim.x ) > width
                    || ( _settings._top_left.y + dim.y ) > height
                   )
                 , "User provided view has incorrect size." );

    }

    template< typename Images
            , typename FormatChecker
            , typename Parent
            >
    void dyn_apply( any_image< Images >& images
                  , const FormatChecker& fc
                  )
    {
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

            dynamic_io_fnobj< bmp_read_is_supported
                            , Parent
                            > op( this );

            apply_operation( view( images )
                           , op
                           );
        }
    }

protected:

    image_read_settings< FormatTag > _settings;
    image_read_info< FormatTag >     _info;

    ConversionPolicy _cc_policy;
};

} // namespace detail
} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_DETAIL_READER_BASE_HPP_INCLUDED
