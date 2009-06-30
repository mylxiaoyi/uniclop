
#if !defined(ENSEMBLE_HEADER)
#define ENSEMBLE_HEADER

#include "../IParametricModel.hpp"
#include "../IModelEstimator.hpp"

#include <boost/random.hpp>
#include <boost/program_options.hpp>

namespace cimg_library {
	class CImgDisplay; 
 }
 
namespace uniclop
{

namespace args = ::boost::program_options;

class ScoredMatch;

template<typename T> class KurtosisIncrementalEstimator; // forward declaration
template<typename T> class HistogramKurtosis; // forward declaration

class EnsembleMethod: public  IModelEstimator
{ // implementation of the W. Zhang and J. Kosecka "Ensemble method for robust estimation"
    // http://www.cs.gmu.edu/%7Ekosecka/Publications/EnsembleMethod.pdf

protected:
    vector<bool> is_inlier;

public:

    vector< KurtosisIncrementalEstimator<double> > kurtosis_estimators;
    vector< HistogramKurtosis<double> > histogram_kurtosis_estimators;
    vector<double> kurtosis_values;

    static args::options_description get_options_description();
	
    EnsembleMethod(args::variables_map &options, IParametricModel &model);

    ~EnsembleMethod();

    void set_min_max_values(double min_error_value, double max_error_value);
    // range of residual values used to estimate the kurtosis

    const ublas::vector<float> &estimate_model_parameters(const vector< ScoredMatch > &);

    const vector< bool > & get_is_inlier();

protected:
    double min_error_value, max_error_value;

    int num_samples;
    IParametricModel  *model_p;
    boost::mt19937 random_generator; // pseudo-random number generators

    void retrieve_random_indexes(const vector< ScoredMatch > &data,
                                 const unsigned int num_indexes, vector<int> &indexes);

    void retrieve_random_matches_indexes(const vector<ScoredMatch> &data, const unsigned int num_indexes,
                                         vector<int> &indexes);
};


// Helper classes KurtosisIncrementalEstimator and HistogramKurtosis

// T: double or float
template<typename T = double>
class HistogramKurtosis
{
    // Do an histogram and compute the kurtosis based on all but the first bin

    T min_value, max_value, delta_value;
    vector<int> bins;

    int n;
    long double mean, central_moment2, central_moment4;
    // central_moment2 is also knowns as variance

public:
    HistogramKurtosis(float _min_value, float _max_value, int num_bins);

    HistogramKurtosis(const HistogramKurtosis &hk);
    ~HistogramKurtosis();
    
    void add_value(const T x);
    T get_kurtosis(); //const
    
    const vector<int> &get_bins() const;

    static void display_histogram(const float kurtosis,
                                  const vector<int> &bins, const int bins_max_value, cimg_library::CImgDisplay &image_display);
    // helper method that draws an histogram

}
; // end helper class HistogramKurtosis<>



}

#endif // ENSEMBLE_HEADER
