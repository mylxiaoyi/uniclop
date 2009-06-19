
#if !defined(ARRSAC_HEADER)
#define ARRSAC_HEADER

/**
Method published in
A Comparative Analysis of RANSAC. Techniques Leading to Adaptive Real-Time. Random Sample Consensus
Rahul Raguram, Jan-Michael Frahm, Marc Pollefeys

provides 50x faster results than RANSAC and about 2x~3x faster results than PROSAC
*/

#include "../IModelEstimator.hpp"
#include "../IParametricModel.hpp"

#include <boost/program_options.hpp>


namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{
namespace estimators
{

using namespace uniclop::algorithms::model_estimation;
namespace args = ::boost::program_options;

template<typename T> // T is the data type
class ARRSAC: public IModelEstimator<T>
{ // given a model and list of scorematches will estimate the best parameters of the model

    ublas::vector<float> estimated_model_parameters;
    vector<bool> is_inlier;
public:

    static args::options_description get_options_description();

    ARRSAC(args::variables_map &options, IParametricModel<T> &);

    ~ARRSAC();

    const ublas::vector<float> &estimate_model_parameters(const vector< T > &);

    const vector< bool > & get_is_inlier();
};

}
}
}
}



#endif // ARRSAC_HEADER
