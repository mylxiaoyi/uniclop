
#include "PROSAC.hpp"

#include "algorithms/features/ScoredMatch.hpp"
#include "algorithms/features/fast/FASTFeature.hpp"


namespace uniclop
{


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class PROSAC methods implementation

::boost::program_options::options_description PROSAC::get_options_description()
{

    args::options_description desc("PROSAC options");
    desc.add_options()

    /* ( "blur_sigma,b", args::value<double>()->default_value(1.0f),
       "defines a bluring level for images") */
    ;

    return desc;
}


PROSAC::PROSAC(args::variables_map &options, IParametricModel &)
{
    throw runtime_error("PROSAC model estimation is not yet implemented");
    return;
}

PROSAC::~PROSAC()
{
    return;
}

const ublas::vector<float> &PROSAC::estimate_model_parameters(const vector< ScoredMatch > &)
{
    return estimated_model_parameters;
}

const vector< bool > &  PROSAC::get_is_inlier()
{
    return is_inlier;
}


}