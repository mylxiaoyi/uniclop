// Copyright 2009 Christian Henning.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_INDEXED_IMAGE_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_TOOLBOX_INDEXED_IMAGE_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Indexed Image extension
/// \author Christian Henning \n
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/gil/gil_all.hpp>

namespace boost
{
namespace gil
{

typedef boost::gil::point2< std::ptrdiff_t > point_t;

template< typename Locator >
struct get_pixel_type_locator : mpl::if_< typename is_bit_aligned< typename Locator::value_type >::type
            , typename Locator::reference
            , typename Locator::value_type
            > {};

// used for virtual locator
template< typename IndicesLoc
, typename PaletteLoc
>
struct indexed_image_deref_fn
{
    typedef IndicesLoc indices_locator_t;
    typedef PaletteLoc palette_locator_t;

    typedef typename channel_type< typename get_pixel_type_locator< indices_locator_t >::type >::type index_t;

    typedef indexed_image_deref_fn          const_t;
    typedef typename PaletteLoc::value_type value_type;
    typedef value_type                      reference;
    typedef value_type                      const_reference;
    typedef point_t                         argument_type;
    typedef reference                       result_type;

    static const bool is_mutable = false;

    indexed_image_deref_fn() {}

    indexed_image_deref_fn( const indices_locator_t& indices_loc
                            , const palette_locator_t& palette_loc
                          )
            : _indices_loc( indices_loc )
            , _palette_loc( palette_loc )
    {}

    result_type operator()( const point_t& p ) const
    {
        return _palette_loc[ at_c< 0 >( *_indices_loc.xy_at( p ) ) ];
    }

    void set_indices( const indices_locator_t& indices_loc )
    {
        _indices_loc = indices_loc;
    }
    void set_palette( const palette_locator_t& palette_loc )
    {
        _palette_loc = palette_loc;
    }

    const indices_locator_t& indices() const
    {
        return _indices_loc;
    }
    const palette_locator_t& palette() const
    {
        return _palette_loc;
    }

private:

    indices_locator_t _indices_loc;
    palette_locator_t _palette_loc;
};

template< typename IndicesLoc
, typename PaletteLoc
>
struct indexed_image_locator_type
{
    typedef virtual_2d_locator< indexed_image_deref_fn< IndicesLoc
    , PaletteLoc
    >
    , false
    > type;
};

template< typename Locator > // indexed_image_locator_type< ... >::type
class indexed_image_view : public image_view< Locator >
{
public:

    typedef typename Locator::deref_fn_t deref_fn_t;
    typedef typename deref_fn_t::indices_locator_t indices_locator_t;
    typedef typename deref_fn_t::palette_locator_t palette_locator_t;

    typedef indexed_image_view< Locator > const_t;

    typedef image_view< indices_locator_t > indices_view_t;
    typedef image_view< palette_locator_t > palette_view_t;

    indexed_image_view()
            : image_view()
            , _num_colors( 0 )
    {}

    indexed_image_view( const point_t& dimensions
                        , std::size_t    num_colors
                        , const Locator& locator
                      )
            : image_view( dimensions, locator )
            , _num_colors( num_colors )
    {}

    template< typename IndexedView >
    indexed_image_view( const IndexedView& iv )
            : image_view( iv )
            , _num_colors( iv._num_colors )
    {}

    const std::size_t num_colors() const
    {
        return _num_colors;
    }


    const indices_locator_t& indices() const
    {
        return get_deref_fn().indices();
    }
    const palette_locator_t& palette() const
    {
        return get_deref_fn().palette();
    }

    const indices_view_t get_indices_view() const
    {
        return indices_view_t( dimensions(), indices() );
    }
    const palette_view_t get_palette_view() const
    {
        return palette_view_t( point_t( num_colors(), 1 )
                               , palette()
                             );
    }

private:

    const deref_fn_t& get_deref_fn() const
    {
        return pixels().deref_fn();
    }

private:

    template< typename Locator2 > friend class indexed_image_view;

    std::size_t _num_colors;
};


template< typename Index
, typename Pixel
>
class indexed_image
{
public:

    typedef image< Index, false, std::allocator< unsigned char > > indices_t;
    typedef image< Pixel, false, std::allocator< unsigned char > > palette_t;

    typedef typename indices_t::view_t indices_view_t;
    typedef typename palette_t::view_t palette_view_t;

    typedef typename indices_t::const_view_t indices_const_view_t;
    typedef typename palette_t::const_view_t palette_const_view_t;

    typedef typename indices_view_t::locator indices_locator_t;
    typedef typename palette_view_t::locator palette_locator_t;

    typedef typename indexed_image_locator_type< indices_locator_t
    , palette_locator_t
    >::type locator_t;

    typedef indexed_image_view< locator_t > view_t;
    typedef typename view_t::const_t        const_view_t;

    indexed_image() {}

    indexed_image( const point_t& dimensions
                   , std::size_t    num_colors
                 )
            : _indices( dimensions    )
            , _palette( num_colors, 1 )
    {
        typedef typename indices_view_t::locator indices_loc_t;
        typedef typename palette_view_t::locator palette_loc_t;

        typedef indexed_image_deref_fn< indices_locator_t
        , palette_locator_t
        > defer_fn_t;

        defer_fn_t deref_fn( view( _indices ).xy_at( 0, 0 )
                             , view( _palette ).xy_at( 0, 0 )
                           );

        locator_t locator( point_t( 0, 0 )
                           , point_t( 1, 1 )
                           , deref_fn
                         );

        _view = view_t( dimensions
                        , num_colors
                        , locator
                      );
    }

    indexed_image( const indexed_image& img )
            : _indices( img._indices )
            , _palette( img._palette )
    {}

    template <typename Pixel2, typename Index2>
    indexed_image( const indexed_image< Pixel2, Index2 >& img )
    {
        _indices = img._indices;
        _palette = img._palette;
    }

    indexed_image& operator= ( const indexed_image& img )
    {
        _indices = img._indices;
        _palette = img._palette;

        return *this;
    }

    indices_const_view_t get_indices_const_view() const
    {
        return static_cast< indices_const_view_t >( _view.get_indices_view());
    }
    palette_const_view_t get_palette_const_view() const
    {
        return static_cast< palette_const_view_t >( _view.get_palette_view());
    }

    indices_view_t get_indices_view()
    {
        return _view.get_indices_view();
    }
    palette_view_t get_palette_view()
    {
        return _view.get_palette_view();
    }

public:

    view_t _view;

private:

    indices_t _indices;
    palette_t _palette;
};

template< typename Index
, typename Pixel
>
inline
const typename indexed_image< Index, Pixel >::view_t& view( indexed_image< Index, Pixel >& img )
{
    return img._view;
}

template< typename Index
, typename Pixel
>
inline
const typename indexed_image< Index, Pixel >::const_view_t const_view( indexed_image< Index, Pixel >& img )
{
    return static_cast< const typename indexed_image< Index, Pixel >::const_view_t>( img._view );
}

// Whole image has one color and all indices are set to 0.
template< typename Locator
, typename Value
>
void fill_pixels( const indexed_image_view< Locator >& view
                  , const Value&                         value
                )
{
    typedef indexed_image_view< Locator > view_t;

    fill_pixels( view.get_indices_view(), view_t::indices_view_t::value_type( 0 ));
    *view.get_palette_view().begin() = value;
}

} // gil
} // boost

#endif // BOOST_GIL_EXTENSION_TOOLBOX_INDEXED_IMAGE_HPP_INCLUDED
