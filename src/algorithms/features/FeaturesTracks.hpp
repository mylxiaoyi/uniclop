
#if !defined(FEATURES_TRACKS_HEADER)
#define FEATURES_TRACKS_HEADER



#include <boost/gil/utilities.hpp>
// gil/utilities.hpp defines point2<>

#include <list>

namespace uniclop
{
namespace algorithms
{
namespace features
{

class FeatureTrackItem;

/**
Container class that keeps the features tracks
(sequence of matched feature points on different images)
We do not suppose that detection is consecutive
*/
class FeaturesTracks: public list<FeatureTrack>
{

public:

    typedef list<FeatureTrackItem> FeatureTrack;

    FeaturesTracks();
    ~FeaturesTracks();

};

/**
The track of a single feature.
Herits from point2<float> since the feature localization may be subpixelic.
*/
class FeatureTrackItem : public boost::gil::point2<float>
{

public:
    int image_index;

};


}
}
}


#endif // !defined(FEATURES_TRACKS_HEADER)

