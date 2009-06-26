

#if !defined(FAST_FEATURES_MATCHER_HEADER_INCLUDED)
#define FAST_FEATURES_MATCHER_HEADER_INCLUDED

// Noisy features matching

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include "../IFeaturesMatcher.hpp"

#include <vector>
#include <boost/program_options.hpp>

namespace uniclop
{
	
	using namespace std;
namespace args = boost::program_options;

class FASTFeature; // forward declaration

/**
 will return an ordered list of ScoredMatches
    implement the fast matching algorithm for FAST features as
    described in Edward Rosten thesis

*/
class FASTFeaturesMatcher : IFeaturesMatcher<FASTFeature>
{

    vector< ScoredMatch > matchings;
public:

    static args::options_description get_options_description();

    FASTFeaturesMatcher(args::variables_map &options);
    ~FASTFeaturesMatcher();

    vector< ScoredMatch >& match(
        const vector<FASTFeature>& features_list_a,
        const vector<FASTFeature>& features_list_b);
};


}

#endif // FAST_FEATURES_MATCHER_HEADER_INCLUDED

