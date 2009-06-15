
#include "FASTFeaturesMatcher.hpp"



namespace uniclop
{
namespace algorithms
{
namespace features
{
namespace fast
{

using namespace std;

// Class FASTFeaturesMatcher methods implementations
args::options_description FASTFeaturesMatcher::get_options_description()
{

    args::options_description desc("FASTFeaturesMatcher options");
    desc.add_options()

    /* ( "blur_sigma,b", args::value<double>()->default_value(1.0f),
       "defines a bluring level for images") */

    ;

    return desc;
}

FASTFeaturesMatcher::FASTFeaturesMatcher(args::variables_map &options)
{
    return;
}

FASTFeaturesMatcher::~FASTFeaturesMatcher()
{
    return;
}

vector< ScoredMatch<FASTFeature> >& FASTFeaturesMatcher::match(
    const vector<FASTFeature>& features_list_a,
    const vector<FASTFeature>& features_list_b)
{
    matchings.clear();

    return matchings;
}




}
}
}
}