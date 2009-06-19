
#if !defined(SIMPLE_FEATURES_MATCHER_HEADER)
#define SIMPLE_FEATURES_MATCHER_HEADER

#include "IFeaturesMatcher.hpp"

#include <vector>
#include <boost/program_options.hpp>

namespace uniclop
{
namespace algorithms
{
namespace features
{

using namespace std;
namespace args = boost::program_options;

template<typename F>
class SimpleFeaturesMatcher: public IFeaturesMatcher<F>
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

}
}
}


#endif // SIMPLE_FEATURES_MATCHER_HEADER

