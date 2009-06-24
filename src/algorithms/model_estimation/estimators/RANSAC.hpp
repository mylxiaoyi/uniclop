#if !defined(RANSAC_HEADER)
#define RANSAC_HEADER

// RANSAC estimator


#include "../IModelEstimator.hpp"
#include "../IParametricModel.hpp"

#include <boost/program_options.hpp>


namespace uniclop
{

using namespace uniclop::algorithms::model_estimation;
namespace args = boost::program_options;

template<typename T> // T is the data type
class RANSAC: public IModelEstimator<T>
{ // given a model and list of scorematches will estimate the best parameters of the model

    ublas::vector<float> estimated_model_parameters;
    vector<bool> is_inlier;

    bool use_fundamental_matrix_model,  use_homography_model;

    // ransac algorithm parameters
    double outlier_thresh_;
    double max_outlier_frac_;
    double desired_prob_good_;
    int max_pops_;
    bool gen_all_;
    int trace_level_;

public:

    static args::options_description get_options_description();

    RANSAC(args::variables_map &options, IParametricModel<T> &);

    ~RANSAC();

    const ublas::vector<float> &estimate_model_parameters(const vector< T > &);

    const vector< bool > & get_is_inlier();
};

}

#endif // RANSAC_HEADER
