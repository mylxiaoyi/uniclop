
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

namespace args = ::boost::program_options;
class ScoredMatch;

class ARRSAC: public IModelEstimator
{ // given a model and list of scorematches will estimate the best parameters of the model

    ublas::vector<float> estimated_model_parameters;
    vector<bool> is_inlier;
public:

    static args::options_description get_options_description();

    ARRSAC(args::variables_map &options, IParametricModel &);

    ~ARRSAC();

    const ublas::vector<float> &estimate_model_parameters(const vector< ScoredMatch > &);

    const vector< bool > & get_is_inlier();
};

}



#endif // ARRSAC_HEADER
