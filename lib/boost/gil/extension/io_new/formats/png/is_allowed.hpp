/*
    Copyright 2008 Christian Henning, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_PNG_IO_IS_ALLOWED_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_PNG_IO_IS_ALLOWED_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/for_each.hpp>

namespace boost { namespace gil { namespace detail {

namespace png {

typedef std::vector< png_bitdepth::type > channel_sizes_t;

// The following two functions look the same but are different since one is using
// a pixel_t as template parameter whereas the other is using reference_t.
template< typename View >
bool compare_channel_sizes( const channel_sizes_t& channel_sizes // in bits
                          , mpl::false_                          // is_bit_aligned
                          , mpl::true_                           // is_homogeneous
                          )
{
    typedef typename View::value_type pixel_t;
    typedef typename channel_traits<
                typename element_type< pixel_t >::type >::value_type channel_t;

    unsigned int s = detail::unsigned_integral_num_bits< channel_t >::value;

    return ( s == channel_sizes[0] );
}


template< typename View >
bool compare_channel_sizes( const channel_sizes_t& channel_sizes // in bits
                          , mpl::true_                           // is_bit_aligned
                          , mpl::true_                           // is_homogeneous
                          )
{
    typedef typename View::reference ref_t;

    typedef typename channel_traits< typename element_type< ref_t >::type >::value_type channel_t;
    channel_t c;

    unsigned int s = detail::unsigned_integral_num_bits< channel_t >::value;

    return ( s == channel_sizes[0] );
}

struct compare_channel_sizes_fn
{
    compare_channel_sizes_fn( png_bitdepth::type* a )
    : _a( a )
    , _b( true )
    {}

    template< typename ChannelSize >
    void operator()( ChannelSize x)
    {
        if( x != *_a++ )
        {
            _b = false;
        }
    }

    png_bitdepth::type* _a;
    bool _b;
};

template< typename T >
struct channel_sizes_type {};

template< typename B, typename C, typename L, bool M >
struct channel_sizes_type< bit_aligned_pixel_reference< B, C, L, M > > { typedef C type; };

template< typename B, typename C, typename L, bool M >
struct channel_sizes_type< const bit_aligned_pixel_reference< B, C, L, M > > { typedef C type; };

template< typename View >
bool compare_channel_sizes( const channel_sizes_t& channel_sizes // in bits
                          , mpl::true_                           // is_bit_aligned
                          , mpl::false_                          // is_homogeneous
                          )
{
    // loop through all channels and compare

    typedef typename View::reference ref_t;
    typedef typename channel_sizes_type< ref_t >::type cs_t;

    compare_channel_sizes_fn fn( &channel_sizes.front() );
    mpl::for_each< cs_t >( fn );

    return fn._b;
}

} // namespace png


template< typename View >
bool is_allowed( const image_read_info< png_tag >& info
               , mpl::true_   // is read_and_no_convert
               )
{
    if( info._color_type == PNG_COLOR_TYPE_PALETTE )
    {
        return false;
    }

    png::channel_sizes_t channel_sizes( info._num_channels
                                      , info._bit_depth
                                      );

    typedef typename View::value_type pixel_t;
    typedef typename View::reference  ref_t;
    typedef typename num_channels< pixel_t >::value_type num_channel_t;

    const num_channel_t dst_n = num_channels< pixel_t >::value;

    return (  static_cast< unsigned int >( dst_n ) == info._num_channels
           && png::compare_channel_sizes< View >( channel_sizes
                                                , typename is_bit_aligned< ref_t >::type()
                                                , typename is_homogeneous< ref_t >::type()
                                                )
           );
}

template< typename View >
bool is_allowed( const image_read_info< png_tag >& info
               , mpl::false_  // is read_and_convert
               )
{
    return true;
}

} // namespace detail
} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_PNG_IO_IS_ALLOWED_HPP_INCLUDED
