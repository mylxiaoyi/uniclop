
#include "ARRSAC.hpp"

#include "algorithms/features/ScoredMatch.hpp"
#include "algorithms/features/fast/FASTFeature.hpp"

namespace uniclop
{


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class ARRSAC methods implementation

args::options_description ARRSAC::get_options_description()
{

    args::options_description desc("ARRSAC options");
    desc.add_options()

    /* ( "blur_sigma,b", args::value<double>()->default_value(1.0f),
       "defines a bluring level for images") */
    ;

    return desc;
}


ARRSAC::ARRSAC(args::variables_map &options, IParametricModel &)
{
    throw runtime_error("ARRSAC model estimation is not yet implemented");
    return;
}

ARRSAC::~ARRSAC()
{
    return;
}

const ublas::vector<float> &ARRSAC::estimate_model_parameters(const vector< ScoredMatch > &)
{
    return estimated_model_parameters;
}

const vector< bool > &  ARRSAC::get_is_inlier()
{
    return is_inlier;
}




}