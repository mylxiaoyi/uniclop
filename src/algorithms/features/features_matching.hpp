
#if !defined(FEATURES_MATCHING_HEADER_INCLUDED)
#define FEATURES_MATCHING_HEADER_INCLUDED

// Noisy features matching

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

// #include "features_detection.hpp" // do we need IFeature ?

#include <vector>
using namespace std;

#include <boost/program_options.hpp>
namespace args = boost::program_options;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Classes definition

template<typename F>
class ScoredMatch
{
public:

    typedef F feature_type;

    const F *feature_a, *feature_b;
    ///< warning: keeping pointers to a vector element is dangereous since
    ///< the vector memory area will change as his content grows

    float distance;
    ///< distance between features A and B (given a metric)

    ScoredMatch()
    {
        feature_a = NULL;
        feature_b = NULL;
        distance = -1;
        return;
    }

    ScoredMatch(const F *a, const F *b)
    {
        feature_a = a;
        feature_b = b;
        distance = a->distance(*b);
        return;
    }

    ScoredMatch(const ScoredMatch &m)
    {
        feature_a = m.feature_a;
        feature_b = m.feature_b;
        distance = m.distance;
        return;
    }

    ~ScoredMatch()
    { // nothing to do
        return;
    }


    bool operator<(const ScoredMatch &m) const
    {
        if ( distance < 0 || m.distance < 0)
            throw runtime_error("Comparing an uninitialized ScoredMatch::distance");

        return distance < m.distance;
    }

};

template <typename F> // F is the feature type
class IFeaturesMatcher
{
    // will return a list of ScoredMatches
public:

    vector< ScoredMatch<F> >& match(const vector<F>& features_vector_a, const vector<F>& features_vector_b);
    ///< since the matcher stores pointers to the features we have to be extra carefull of not
    ///< changing the features_vector content could invalidate the pointers !
};

class FASTFeature; // forward declaration

class FASTFeaturesMatcher : IFeaturesMatcher<FASTFeature>
{
    // will return an ordered list of ScoredMatches
    // implement the fast matching algorithm for FAST features as
    // described in Edward Rosten thesis

    vector< ScoredMatch<FASTFeature> > matchings;
public:

    static args::options_description get_options_description();

    FASTFeaturesMatcher(args::variables_map &options);
    ~FASTFeaturesMatcher();

    vector< ScoredMatch<FASTFeature> >& match(
        const vector<FASTFeature>& features_list_a,
        const vector<FASTFeature>& features_list_b);
};

template<typename F>
class SimpleFeaturesMatcher: IFeaturesMatcher<F>
{
    // will return an ordered list of ScoredMatches

    // dead simple implementation O(N**2) that takes the num_near_features nearest features

    vector< ScoredMatch<F> > matchings;

    float _max_distance;
    int _num_near_features;
    // for each feature, search the num_near_features nearest with respect to a distance threshold
public:

    static args::options_description get_options_description();

    SimpleFeaturesMatcher(args::variables_map &options);
    ~SimpleFeaturesMatcher();

    vector< ScoredMatch<F> >& match(const vector<F>& features_list_a, const vector<F>& features_list_b);
};


#endif // #if !defined(FEATURES_MATCHING_HEADER_INCLUDED)
