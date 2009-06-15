
#include "PROSAC.hpp"


namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{
namespace estimators
{


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class PROSAC methods implementation



template<typename T>
args::options_description PROSAC<T>::get_options_description()
{

    args::options_description desc("PROSAC options");
    desc.add_options()

    /* ( "blur_sigma,b", args::value<double>()->default_value(1.0f),
       "defines a bluring level for images") */
    ;

    return desc;
}


template<typename T>
PROSAC<T>::PROSAC(args::variables_map &options, IParametricModel<T> &)
{
    throw runtime_error("PROSAC model estimation is not yet implemented");
    return;
}

template<typename T>
PROSAC<T>::~PROSAC()
{
    return;
}

template<typename T>
const ublas::vector<float> &PROSAC<T>::estimate_model_parameters(const vector< T > &)
{
    return estimated_model_parameters;
}

template<typename T>
const vector< bool > &  PROSAC<T>::get_is_inlier()
{
    return is_inlier;
}


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

template class PROSAC< ScoredMatch<FASTFeature> >;
template class PROSAC< ScoredMatch<SIFTFeature> >;


}
}
}
}