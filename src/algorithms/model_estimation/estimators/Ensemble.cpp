
#include "Ensemble.hpp"
#include "algorithms/features/ScoredMatch.hpp"
#include "algorithms/features/fast/FASTFeature.hpp"

#include "algorithms/model_estimation/models/HomographyModel.hpp"
#include "algorithms/model_estimation/models/FundamentalMatrixModel.hpp"

#include "OneDimensionalKMeans.hpp"

// Boost http://boost.org
#include <boost/numeric/ublas/io.hpp>
#include <boost/tuple/tuple.hpp>

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>

// RANSAC implementation requires VXL installed
// with the RREL and GEL/vpgl contributions

#include <vxl/vcl/vcl_iostream.h>

#include <vxl/core/vnl/vnl_math.h>

#include <vxl/core/vgl/algo/vgl_homg_operators_2d.h>

#include <vxl/contrib/rpl/rrel/rrel_muset_obj.h>
#include <vxl/contrib/rpl/rrel/rrel_ran_sam_search.h>
#include <vxl/contrib/rpl/rrel/rrel_homography2d_est.h>

#include <vxl/contrib/gel/mrc/vpgl/vpgl_fundamental_matrix.h>
#include <vxl/contrib/gel/mrc/vpgl/algo/vpgl_fm_compute_ransac.h>
#include <vxl/contrib/gel/mrc/vpgl/algo/vpgl_fm_compute_8_point.h>



namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{
namespace estimators
{

using uniclop::algorithms::features::ScoredMatch;
using namespace uniclop::algorithms::model_estimation::models;

using namespace cimg_library;

// Class EnsembleMethod methods implementation

// implementation of the W. Zhang and J. Kosecka "Ensemble method for robust estimation"
// http://www.cs.gmu.edu/%7Ekosecka/Publications/EnsembleMethod.pdf


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Helper classes KurtosisIncrementalEstimator and HistogramKurtosis

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
; // end helper class HistogramKurtosis<>


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

template<typename T>
args::options_description EnsembleMethod<T>::get_options_description()
{

    args::options_description desc("EnsembleMethod options");
    desc.add_options()

    ( "ensemble_method.num_samples", args::value<int>()->default_value(500),
      "define the number of samples used to estimate the kurtosis of the error distribution")
    ;

    return desc;
}


template<typename T>
EnsembleMethod<T>::EnsembleMethod(args::variables_map &options, IParametricModel<T> &model)
{

    // default min/max values
    min_error_value = 0.0;
    max_error_value = numeric_limits<double>::max();

    num_samples = 500;
    if (options.count("ensemble_method.num_samples"))
        num_samples = options["ensemble_method.num_samples"].as<int>();

    model_p = &model;
    if (model_p == NULL)
        throw runtime_error("EnsembleMethod input parametric model can not be NULL");

    return;
}

template<typename T>
EnsembleMethod<T>::~EnsembleMethod()
{
    // nothing to do here
    return;
}

template<typename T>
void EnsembleMethod<T>::set_min_max_values(double _min_error_value, double _max_error_value)
{
    min_error_value = _min_error_value;
    max_error_value = _max_error_value;
    return;
}

template<typename T>
const ublas::vector<float> &EnsembleMethod<T>::estimate_model_parameters(const vector< T > &matches)
{
    // estimate the kurtosis of the each data point ---
    vector<int> indexes;
    vector<int>::const_iterator indexes_it;
    vector< T > sample_set(model_p->get_num_points_to_estimate());
    typename vector< T >::iterator sample_set_it;

    typename vector< T >::const_iterator matches_it;

    vector<float> residuals(matches.size());
    vector<float>::const_iterator residuals_it;

    kurtosis_estimators.clear();
    kurtosis_estimators.resize(matches.size(),
                               KurtosisIncrementalEstimator<double>(min_error_value, max_error_value));

    histogram_kurtosis_estimators.clear();
    histogram_kurtosis_estimators.resize(matches.size(),
                                         HistogramKurtosis<double>(min_error_value, max_error_value, 50));

    // for each sample we will estimate the kurtosis of his residuals of each estimated model
    vector< KurtosisIncrementalEstimator<double> >::iterator kurtosis_estimators_it;
    vector< KurtosisIncrementalEstimator<double> >::const_iterator const_kurtosis_estimators_it;

    vector< HistogramKurtosis<double> >::iterator histogram_kurtosis_estimators_it;
    //vector< HistogramKurtosis<double> >::const_iterator const_histogram_kurtosis_estimators_it;
    vector< HistogramKurtosis<double> >::iterator const_histogram_kurtosis_estimators_it;

    kurtosis_values.resize(matches.size());
    vector<double>::iterator kurtosis_values_it;


    int c;
    for (c=0;c<num_samples;c+=1)
    {
        // grab randomly "num_points_to_estimate" samples --
        retrieve_random_indexes(matches, model_p->get_num_points_to_estimate(), indexes);

        // estimate the model parameters --
        for (sample_set_it = sample_set.begin(), indexes_it = indexes.begin();
                sample_set_it != sample_set.end() && indexes_it != indexes.end();
                ++sample_set_it, ++indexes_it)
        {
            *sample_set_it = matches[*indexes_it]; // copy
        }

        model_p->estimate_from_minimal_set(sample_set);

        // evaluate the error of each sample --
        model_p->compute_residuals(matches, residuals);

        // update the kurtosis of the error distribution of each sample --
        for (residuals_it = residuals.begin(),
                kurtosis_estimators_it = kurtosis_estimators.begin(),
                histogram_kurtosis_estimators_it = histogram_kurtosis_estimators.begin();
                residuals_it != residuals.end()
                && kurtosis_estimators_it != kurtosis_estimators.end()
                && histogram_kurtosis_estimators_it != histogram_kurtosis_estimators.end();
                ++residuals_it, ++kurtosis_estimators_it, ++histogram_kurtosis_estimators_it)
        {
            kurtosis_estimators_it->add_value( *residuals_it );
            histogram_kurtosis_estimators_it->add_value( *residuals_it );
        }

    } // end of 'for c in [0, num_samples)

    // classifier the kurtosis values distribution into inliers and outliers samples ---
    // (using rank or k-means)

    // retrieve the estimated values -
    const bool print_kurtosis_values = true;
    if (print_kurtosis_values) cout << "kurtosis values == ";
    for (const_kurtosis_estimators_it = kurtosis_estimators.begin(),
            const_histogram_kurtosis_estimators_it = histogram_kurtosis_estimators.begin(),
            kurtosis_values_it = kurtosis_values.begin();
            const_kurtosis_estimators_it != kurtosis_estimators.end()
            && const_histogram_kurtosis_estimators_it != histogram_kurtosis_estimators.end()
            && kurtosis_values_it != kurtosis_values.end();
            ++const_kurtosis_estimators_it,
            ++const_histogram_kurtosis_estimators_it,
            ++kurtosis_values_it)
    {
        //*kurtosis_values_it = const_kurtosis_estimators_it->get_kurtosis();
        *kurtosis_values_it = const_histogram_kurtosis_estimators_it->get_kurtosis();
        if (print_kurtosis_values) cout << *kurtosis_values_it << " ";
    }

    if (print_kurtosis_values) cout << endl;

    // do k-means -
    OneDimensionalKMeans kmeans;
    kmeans.compute(kurtosis_values, 2);
    const vector<unsigned int> &permutations = kmeans.get_permutations();
    const vector<unsigned int> &splits = kmeans.get_splits();
    const vector<double> &kmeans_result = kmeans.get_kmeans();
    unsigned int split_point = splits[1];
    // permutation is a list of indexes into the data
    // splits are indexes in the permutations list (?)


    // tag inliers and outliers -
    is_inlier.resize(kurtosis_values.size());
    vector< T > inliers_subset;

    vector<unsigned int>::const_iterator permutations_it;
    unsigned int permutations_index;
    for (permutations_it = permutations.begin(), permutations_index = 0;
            permutations_it != permutations.end();
            ++permutations_it, permutations_index+=1)
    {
        // the lower set of kurtosis are the inliers, the rest are outliers
        bool point_is_inlier = false;
        if (permutations_index >= split_point)
            point_is_inlier = false;
        else if (permutations_index < split_point)
            point_is_inlier = true;
        else
        {
            throw runtime_error("debugging");
            const float t_val = kurtosis_values[permutations_index];
            const float delta_up = abs(t_val - kmeans_result[1]);
            const float delta_down = abs(t_val - kmeans_result[0]);
            point_is_inlier = (delta_up < delta_down); // nearer to the upper set
        }
        is_inlier[*permutations_it] = point_is_inlier;
        if (point_is_inlier)
            inliers_subset.push_back( matches[*permutations_it] ); // keep a copy
    }

    if (true)
    {

        vector<double> set_one, set_two;
        for (unsigned int i=0; i < permutations.size(); i+=1)
        {
            if (is_inlier[permutations[i]])
                set_one.push_back(kurtosis_values[permutations[i]]);
            else
                set_two.push_back(kurtosis_values[permutations[i]]);
        }

        cout << "kmeans.get_splits() == " ;
        for (unsigned int i=0; i < splits.size(); i+=1) cout << " " << splits[i];
        cout << endl;

        cout << "kmeans.get_permutations() == " ;
        for (unsigned int i=0; i < permutations.size(); i+=1) cout << " " << permutations[i];
        cout << endl;

        cout << "kmeans.get_kmeans() == " ;
        for (unsigned int i=0; i < kmeans_result.size(); i+=1) cout << " " << kmeans_result[i];
        cout << endl;

        cout << "set_one == " ;
        for (unsigned int i=0; i < set_one.size(); i+=1) cout << " " << set_one[i];
        cout << endl;

        cout << "set_two == " ;
        for (unsigned int i=0; i < set_two.size(); i+=1) cout << " " << set_two[i];
        cout << endl;

    }

    // from the inliers obtain the final model parameters
    model_p->estimate(inliers_subset);

    return model_p->get_parameters();
}

template<typename T>
const vector< bool > &  EnsembleMethod<T>::get_is_inlier()
{
    return is_inlier;
}


// helper search class
template<typename T>
class compare_matches_unary_predicate // : UnaryPredicate<int>
{
    const vector< T > &data;
    const T &match;

public:
    compare_matches_unary_predicate(const vector< T > &_data, const unsigned int match_index)
            : data(_data), match(_data[match_index])
    {
        return;
    }

    bool operator()(int index) const;

};

template<typename T>
bool compare_matches_unary_predicate<T>::operator()(int index) const
{ // we want to ensure that we have different

    if (index >= static_cast<int>(data.size()) )
        throw runtime_error("compare_matches_unary_predicate::operator() index out of range");

    const T &m = this->data[index];
    return (match.feature_a == m.feature_a) || (match.feature_b == m.feature_b);
}


template<>
bool compare_matches_unary_predicate< boost::tuple<float, float> >::operator()(int index) const
{
// in order to ensure numerical stability we have to avoid using pairs of points where
// the x values are too similar

    const boost::tuple<float, float> &m = this->data[index];
    const float x0 = match.get<0>();
    const float x1 = m.get<0>();
    return abs(x0 - x1) < 1e-10;
}

template<>
bool compare_matches_unary_predicate<double>::operator()(int index) const
{
    throw runtime_error("This method should never be called");
}

template<>
bool compare_matches_unary_predicate<float>::operator()(int index) const
{
    throw runtime_error("This method should never be called");
}



// helper function used for
// partial specialization of retrieve_random_indexes when comparing scored matches
template<typename T>
void EnsembleMethod<T>::retrieve_random_matches_indexes
(const vector< T > &data, const unsigned int num_indexes, vector<int> &indexes)
{
    // we expect T to be ScoredMatch<F>

    const int max_index_value = data.size() - 1;
    indexes.clear(); // erase previous indexes

    boost::variate_generator<boost::mt19937&, boost::uniform_int<int> >
    get_random_index(random_generator, boost::uniform_int<int>(0, max_index_value));

    int counter = 0;

    while (indexes.size() != num_indexes)
    {
        // sample an index
        const unsigned int t_index = get_random_index();

        if (t_index >= data.size() ) // sanity check
            throw runtime_error("EnsembleMethod<T>::retrieve_random_matches_indexes index out of range");

        if ( std::find_if(indexes.begin(), indexes.end(),
                          compare_matches_unary_predicate<T>(data, t_index) ) == indexes.end())
        { // the index was not listed, we can add it
            indexes.push_back(t_index);
        }

        counter+=1;
        if ( counter > 10*max_index_value)
            throw runtime_error("EnsembleMethod< ScoredMatch<F> >::retrieve_random_indexes dataset ill conditioned");
    } // end of 'while not enough indexes have been retrieved'

    return;
}

// generic case
template<typename T>
void EnsembleMethod<T>::retrieve_random_indexes(const vector< T > &data, const unsigned int num_indexes,
        vector<int> &indexes)
{

    indexes.clear(); // erase previous indexes

    const int max_index_value = data.size();
    boost::variate_generator<boost::mt19937&, boost::uniform_int<int> >
    get_random_index(random_generator, boost::uniform_int<int>(0, max_index_value));
    int counter = 0;

    while (indexes.size() != num_indexes)
    {
        // sample an index
        int t_index = get_random_index();

        if ( find(indexes.begin(), indexes.end(), t_index) == indexes.end())
        { // the index was not listed, we can add it
            indexes.push_back(t_index);
        }

        counter+=1;
        if ( counter > 10*max_index_value)
            throw runtime_error("EnsembleMethod<T>::retrieve_random_indexes dataset ill conditioned");
    } // end of 'while not enough indexes have been retrieved'

    return;
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Force the compilation of the following types
// for some strange reason, in linux, this has to be at the end of the defitions (?!)


template void HistogramKurtosis<double>::display_histogram
(const float, const vector<int> &, const int , CImgDisplay &);

template<typename F> class ScoredMatch; // forward declaration

template<>
void EnsembleMethod< ScoredMatch<features::fast::FASTFeature> >::retrieve_random_indexes
(const vector< ScoredMatch<features::fast::FASTFeature> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}

/*template<>
void EnsembleMethod< ScoredMatch<features::sift::SIFTFeature> >::retrieve_random_indexes
(const vector< ScoredMatch<features::sift::SIFTFeature> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}*/

template<>
void EnsembleMethod< boost::tuple<float, float> >::retrieve_random_indexes
(const vector< boost::tuple<float, float> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}

template class EnsembleMethod< ScoredMatch<features::fast::FASTFeature> >;
// template class EnsembleMethod< ScoredMatch<SIFTFeature> >;
template class EnsembleMethod< boost::tuple<float, float> >;




// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class HistogramKurtosis methods implementation

template<typename T>
void HistogramKurtosis<T>::display_histogram(const float kurtosis,
        const vector<int> &bins, const int bins_max_value, CImgDisplay &image_display)
{ // helper method that draws an histogram

    const int image_width = 500, image_height = 200;
    const int font_size = 24;
    CImg<uint8_t> histogram_image(image_width, image_height, 1, 3);
    histogram_image.fill(0);

    const uint8_t yellow[3] =
    {
        255,255,0
    };
    const uint8_t blue[3]=
    {
        0,155,255
    };
    //const uint8_t blue2[3]={0,0,255}, blue3[3]={0,0,155}, white[3]={255,255,255};

    CImg<int> histogram_data(bins.size() + 1, 1, 1, 1);
    cimg_forX(histogram_data, x)
    {
        histogram_data(x) = bins[x];
    }

    histogram_data(bins.size()) = bins_max_value;

    histogram_image.draw_graph(histogram_data, yellow, 1);

    histogram_image.draw_text(50,5,"kurtosis == %f",blue,NULL,font_size,1.0, kurtosis);

    image_display.resize(histogram_image);
    image_display.display(histogram_image);

    return;
}


}
}
}
}

