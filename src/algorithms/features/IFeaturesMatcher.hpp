#if !defined(IFEATURES_MATCHER_HEADER_INCLUDED)
#define IFEATURES_MATCHER_HEADER_INCLUDED

// Noisy features matching

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers



#include "ScoredMatch.hpp"

#include <vector>

namespace uniclop
{

using namespace std;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Classes definition

template <typename F> // F is the feature type
class IFeaturesMatcher
{
    // will return a list of ScoredMatches
public:

    virtual vector< ScoredMatch >& match(const vector<F>& features_vector_a, const vector<F>& features_vector_b) = 0;
    ///< since the matcher stores pointers to the features we have to be extra carefull of not
    ///< changing the features_vector content could invalidate the pointers !
};

}

#endif // IFEATURES_MATCHER_HEADER_INCLUDED
