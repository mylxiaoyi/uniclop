
#if !defined(ENSEMBLE_HEADER)
#define ENSEMBLE_HEADER

#include "../IParametricModel.hpp"
#include "../IModelEstimator.hpp"

#include <boost/random.hpp>
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

template<typename T> class KurtosisIncrementalEstimator; // forward declaration
template<typename T> class HistogramKurtosis; // forward declaration

template<typename T> // T is the data type
class EnsembleMethod: public  IModelEstimator<T>
{ // implementation of the W. Zhang and J. Kosecka "Ensemble method for robust estimation"
    // http://www.cs.gmu.edu/%7Ekosecka/Publications/EnsembleMethod.pdf

protected:
    vector<bool> is_inlier;

public:

    vector< KurtosisIncrementalEstimator<double> > kurtosis_estimators;
    vector< HistogramKurtosis<double> > histogram_kurtosis_estimators;
    vector<double> kurtosis_values;

    static args::options_description get_options_description();

    EnsembleMethod(args::variables_map &options, IParametricModel<T> &model);

    ~EnsembleMethod();

    void set_min_max_values(double min_error_value, double max_error_value);
    // range of residual values used to estimate the kurtosis

    const ublas::vector<float> &estimate_model_parameters(const vector< T > &);

    const vector< bool > & get_is_inlier();

protected:
    double min_error_value, max_error_value;

    int num_samples;
    IParametricModel<T>  *model_p;
    boost::mt19937 random_generator; // pseudo-random number generators

    void retrieve_random_indexes(const vector< T > &data,
                                 const unsigned int num_indexes, vector<int> &indexes);

    void retrieve_random_matches_indexes(const vector<T> &data, const unsigned int num_indexes,
                                         vector<int> &indexes);
};

}
}
}
}

#endif // ENSEMBLE_HEADER
