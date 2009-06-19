
#include "ARRSAC.hpp"

#include "algorithms/features/ScoredMatch.hpp"
#include "algorithms/features/fast/FASTFeature.hpp"

namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{
namespace estimators
{

using namespace uniclop::algorithms::features;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class ARRSAC methods implementation



template<typename T>
args::options_description ARRSAC<T>::get_options_description()
{

    args::options_description desc("ARRSAC options");
    desc.add_options()

    /* ( "blur_sigma,b", args::value<double>()->default_value(1.0f),
       "defines a bluring level for images") */
    ;

    return desc;
}


template<typename T>
ARRSAC<T>::ARRSAC(args::variables_map &options, IParametricModel<T> &)
{
    throw runtime_error("ARRSAC model estimation is not yet implemented");
    return;
}

template<typename T>
ARRSAC<T>::~ARRSAC()
{
    return;
}

template<typename T>
const ublas::vector<float> &ARRSAC<T>::estimate_model_parameters(const vector< T > &)
{
    return estimated_model_parameters;
}

template<typename T>
const vector< bool > &  ARRSAC<T>::get_is_inlier()
{
    return is_inlier;
}


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

template class ARRSAC< ScoredMatch< fast::FASTFeature >  >;
//template class ARRSAC< ScoredMatch<SIFTFeature> >;


}
}
}
}