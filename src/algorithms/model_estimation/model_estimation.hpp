
#if !defined(MODEL_ESTIMATION_HEADER_INCLUDED)
#define MODEL_ESTIMATION_HEADER_INCLUDED

// Model estimation, inliers estimation

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

// C++ standard
#include <limits> // to use numeric_limits<float>::max() and similars
#include <vector>
#include <algorithm>
using namespace std;

// Boost http://boost.org
#include <boost/program_options.hpp>
namespace args = boost::program_options;

#include <boost/numeric/ublas/vector.hpp>
namespace ublas = boost::numeric::ublas;

#include <boost/random.hpp>

// CImg http://cimg.sf.net
// forward declaration to avoid including directly CImg.h
namespace cimg_library
{
template<typename T> struct CImg;
struct CImgDisplay;
}
using namespace cimg_library;

#include "features_matching.hpp"

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition

template<typename T> // T the data type (int, double, vector<double>, IFeature<>, etc... )
class IParametricModel
{

public:

    virtual unsigned int get_num_parameters() const = 0;
    // get the number of free parameters of the model

    virtual unsigned int get_num_points_to_estimate() const = 0;
    // m: is the number of points required to estimate the parameters of the model

    virtual  void estimate_from_minimal_set(const vector<T> &data_points) = 0;
    // given m points estimate the parameters vector

    virtual void estimate(const vector<T> &data_points) = 0; // given n>m points, estimate the parameters vector

    virtual const ublas::vector<float>& get_parameters() const = 0;
    // get current estimate of the parameters

    virtual void set_parameters(const ublas::vector<float> &) = 0;
    // set an initial guess of the parameters
    // (useful when the model use iterative methods to estimate his parameters)

    virtual void compute_residuals (const vector<T> &data_points, vector<float> &residuals) const = 0;
    // residuals -> errors
    // Compute the residuals relative to the given parameter vector.


    IParametricModel()
    {
        return;
    }
    virtual ~IParametricModel()
    {
        return;
    }
};

template<typename T>
class IModelEstimator // T is the input data type
{

public:
    virtual const ublas::vector<float> &estimate_model_parameters(const vector<T> &) = 0;
    // given a set of matches, returns an vector with the estimated parameters

    virtual const vector< bool > & get_is_inlier() = 0;
    // returns a vector of the same size of the list of matches, indicating
    // for each index is the match was an inlier or not

    IModelEstimator()
    {
        return;
    }
    virtual ~IModelEstimator()
    {
        return;
    }
};

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Models classes definition

// need a model for fundamental matrix and for homography projections (at least)

template<typename F> // F feature type
class FundamentalMatrixModel: public IParametricModel< ScoredMatch<F> >
{
    // based on code from VXL RREL rrel_fm_problem

    ublas::vector<float> parameters;
public:
    FundamentalMatrixModel();
    ~FundamentalMatrixModel();

    ///@name IParametricModel interface
    ///@{
    unsigned int get_num_parameters() const;
    // get the number of free parameters of the model

    unsigned int get_num_points_to_estimate() const;
    // m: is the number of points required to estimate the parameters of the model

    void estimate_from_minimal_set(const vector< ScoredMatch<F> > &data_points);
    // given m points estimate the parameters vector

    void estimate(const vector< ScoredMatch<F> > &data_points); // given n>m points, estimate the parameters vector

    const ublas::vector<float>& get_parameters() const;
    // get current estimate of the parameters

    void set_parameters(const ublas::vector<float> &);
    // set an initial guess of the parameters
    // (useful when the model use iterative methods to estimate his parameters)

    void compute_residuals (const vector< ScoredMatch<F> > &data_points, vector<float> &residuals) const;
    // residuals -> errors
    // Compute the residuals relative to the given parameter vector.

    ///@}

}
; // end of class FundamentalMatrixModel declaration

template<typename F> // F feature type
class HomographyModel: public IParametricModel< ScoredMatch<F> >
{
    // based on code from VXL RREL rrel_homography2d_est

    ublas::vector<float> parameters;
public:
    HomographyModel();
    ~HomographyModel();

    ///@name IParametricModel interface
    ///@{
    unsigned int get_num_parameters() const;
    // get the number of free parameters of the model

    unsigned int get_num_points_to_estimate() const;
    // m: is the number of points required to estimate the parameters of the model

    void estimate_from_minimal_set(const vector< ScoredMatch<F> > &data_points);
    // given m points estimate the parameters vector

    void estimate(const vector< ScoredMatch<F> > &data_points); // given n>m points, estimate the parameters vector

    const ublas::vector<float>& get_parameters() const;
    // get current estimate of the parameters

    void set_parameters(const ublas::vector<float> &);
    // set an initial guess of the parameters
    // (useful when the model use iterative methods to estimate his parameters)

    void compute_residuals (const vector< ScoredMatch<F> > &data_points, vector<float> &residuals) const;
    // residuals -> errors
    // Compute the residuals relative to the given parameter vector.

    ///@}

}
; // end of class HomographyModel declaration


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Estimator classes definition


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


template<typename T> // T is the data type
class PROSAC: public IModelEstimator<T>
{ // given a model and list of scorematches will estimate the best parameters of the model

    ublas::vector<float> estimated_model_parameters;
    vector<bool> is_inlier;
public:

    static args::options_description get_options_description();

    PROSAC(args::variables_map &options, IParametricModel<T> &);

    ~PROSAC();

    const ublas::vector<float> &estimate_model_parameters(const vector< T > &);

    const vector< bool > & get_is_inlier();
};


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

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Helper class

template<typename T = double> // T: double or float
class KurtosisIncrementalEstimator
{
    // based on information available at
    // http://en.wikipedia.org/wiki/Kurtosis
    // http://en.wikipedia.org/wiki/Moment_about_the_mean
    // http://mathcentral.uregina.ca/QQ/database/QQ.09.02/carlos1.html

    T min_value, max_value;

    int n;
    long double mean, central_moment2, central_moment4;
    // kurtosis computation is numerically unstable
    // so we would like to use the largest representation available
    // central_moment2 is also knowns as variance

public:
    KurtosisIncrementalEstimator(T _min_value = 0, T _max_value = numeric_limits<T>::max())
    {
        // min_value and max_value define the bound of the values that will be
        // actually used to compute the Kurtosis
        // when using the Ensemble method we want to reject the too low errors, and the too large errors

        min_value = _min_value;
        max_value = _max_value;

        n = 0;
        mean = 0.0;
        central_moment2= 0.0;
        central_moment4 = 0.0;
        return;
    }

    KurtosisIncrementalEstimator(const KurtosisIncrementalEstimator<T> &k)
    {
        min_value = k.min_value;
        max_value = k.max_value;

        n = 0;
        mean = 0.0;
        central_moment2= 0.0;
        central_moment4 = 0.0;
        return;
    }

    ~KurtosisIncrementalEstimator()
    {
        return;
    }

    void add_value(T x)
    {
        if (x < min_value || x > max_value) return;
        // we omit values out of the range of interest

        T delta1 = (x - mean);
        T delta2 = delta1 / static_cast<T>(n + 1);
        mean += delta2;
        central_moment2 += n*delta2*delta2 + delta1*delta1;
        central_moment4 += n*delta2*delta2*delta2*delta2 + delta1*delta1*delta1*delta1;
        n += 1;
        return;
    }

    T get_kurtosis() const
    {

        T kurtosis = -3.0;
        if (n > 0)
            kurtosis += static_cast<T>((n*central_moment4/(central_moment2*central_moment2)));
        return kurtosis;
    }

}
; // end helper class KurtosisIncrementalEstimator<>


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
    HistogramKurtosis(float _min_value, float _max_value, int num_bins)
    {
        min_value = _min_value;
        max_value = _max_value;
        delta_value = (max_value - min_value) / num_bins;
        bins.resize(num_bins);
        fill(bins.begin(), bins.end(), 0);

        n = 0;
        mean = 0.0;
        central_moment2= 0.0;
        central_moment4 = 0.0;
        return;
    }

    HistogramKurtosis(const HistogramKurtosis &hk)
    {
        min_value = hk.min_value;
        max_value = hk.max_value;
        delta_value = hk.delta_value;
        bins.resize(hk.bins.size());
        fill(bins.begin(), bins.end(), 0);

        n = 0;
        mean = 0.0;
        central_moment2= 0.0;
        central_moment4 = 0.0;
    }

    ~HistogramKurtosis()
    {
        return;
    }

    void add_value(T x)
    {

        if (x < min_value || x >= max_value) return;
        // we omit values out of the range of interest


        const int num_bin = static_cast<int>(floor((x - min_value) / delta_value));
        if (num_bin < 0)
        { // this should neve happen, but still is a safety check
            throw runtime_error("HistogramKurtosis::add_value internal error");
        }

        if (num_bin >= static_cast<int>(bins.size()) ) return; // safety check
        bins[num_bin] += 1;

        T delta1 = (x - mean);
        T delta2 = delta1 / static_cast<T>(n + 1);
        mean += delta2;
        central_moment2 += n*delta2*delta2 + delta1*delta1;
        central_moment4 += n*delta2*delta2*delta2*delta2 + delta1*delta1*delta1*delta1;
        n += 1;
        return;
    }

    T get_kurtosis() //const
    {

        if (false)
        {
            // iterate over the bins to compute, mean, second central moment and fourth central moment

            // incremental mean estimation is stable
            // the central moments are not, so we recompute them
            central_moment2 = 0.0;
            central_moment4 = 0.0;

            vector<int>::const_iterator bins_it;
            T t_value = 0.0;
            for (bins_it = (bins.begin() + 1), t_value = delta_value + delta_value*0.5;
                    bins_it != bins.end();
                    ++bins_it, t_value += delta_value)
            {
                const float weight = static_cast<float>(*bins_it) / n;
                //long double delta = t_value - mean;
                long double delta = t_value; // <<< just for testing
                central_moment2 += weight*delta*delta;
                central_moment4 += weight*delta*delta*delta*delta;
            }

            return (central_moment4/(central_moment2*central_moment2)) - 3.0;
        }
        else if (true)
        {
            // test hack for the LineModel scenario
            // <<< works quite fine
            int max_delta = 0;

            unsigned int i;
            for (i = 0; i < (bins.size() - 1); i+=1)
            {
                const int delta = bins[i + 1] - bins[i];
                if (delta > max_delta)
                    max_delta = delta;
            }

            return static_cast<T>(max_delta);
        }
        else
        {
            // (ultra dirty) test hack for the HomographyModel scenario
            //return static_cast<T>(mean);

            int max_bin_value = 0;
            unsigned int max_bin_index = 0;

            unsigned int i;
            for (i = 0; i < (bins.size() - 1); i+=1)
            {
                if (bins[i] > max_bin_value)
                {
                    max_bin_value = bins[i];
                    max_bin_index = i;
                }
            }
            return static_cast<T>(max_bin_index);
        }

    }

    const vector<int> &get_bins() const
    {
        return bins;
    }


    static void display_histogram(const float kurtosis,
                                  const vector<int> &bins, const int bins_max_value, CImgDisplay &image_display);
    // helper method that draws an histogram

}
; // end helper class KurtosisIncrementalEstimator<>


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

#endif // #if !defined(MODEL_ESTIMATION_HEADER_INCLUDED)
