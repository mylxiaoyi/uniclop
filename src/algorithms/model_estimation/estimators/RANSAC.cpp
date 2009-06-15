

#include "RANSAC.hpp"



// extra implementation specific headers ---
#include "algorithms/features/features_detection.hpp"


// Boost http://boost.org
#include <boost/numeric/ublas/io.hpp>
#include <boost/tuple/tuple.hpp>

// RANSAC implementation requires VXL installed
// with the RREL and GEL/vpgl contributions

#include <vxl/core/vnl/vnl_math.h>
#include <vxl/vcl/vcl_iostream.h>
#include <vxl/vcl/vcl_cassert.h>
#include <vxl/vcl/vcl_cmath.h>
#include <vxl/core/vgl/algo/vgl_homg_operators_2d.h>
#include <vxl/contrib/rpl/rrel/rrel_ran_sam_search.h>
#include <vxl/contrib/rpl/rrel/rrel_muset_obj.h>
#include <vxl/contrib/rpl/rrel/rrel_homography2d_est.h>
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



// Class RANSAC methods implementation

template<typename T>
args::options_description RANSAC<T>::get_options_description()
{

    args::options_description desc("RANSAC options");
    desc.add_options()

    ( "ransac.outliers_fraction", args::value<float>()->default_value(0.9f),
      "percent of outliers in the data")

    ( "ransac.trace_level", args::value<int>()->default_value(0),
      "debugging verbosity")
    ;

    return desc;
}


template<typename T>
RANSAC<T>::RANSAC(args::variables_map &options, IParametricModel<T> &model)
        :outlier_thresh_(1),max_outlier_frac_(0.9),
        desired_prob_good_(0.99), max_pops_(1), gen_all_(false), trace_level_(0)
{

    if (options.count("ransac.outliers_fraction"))
        max_outlier_frac_ = options["ransac.outliers_fraction"].as<float>();

    if (options.count("ransac.trace_level"))
        trace_level_ = options["ransac.trace_level"].as<int>();

    FundamentalMatrixModel< typename T::feature_type > *fundamental_matrix_p = NULL;
    fundamental_matrix_p = dynamic_cast< FundamentalMatrixModel< typename T::feature_type > * >(&model);
    use_fundamental_matrix_model = (fundamental_matrix_p != NULL);

    HomographyModel< typename T::feature_type > *homography_p = NULL;
    homography_p = dynamic_cast< HomographyModel< typename T::feature_type > * >(&model);
    use_homography_model = (homography_p != NULL);

    if (!use_fundamental_matrix_model && !use_homography_model)
        throw runtime_error("Current implementation of RANSAC<T> only "\
                            "supports the fundamental_matrix model and the homography model");

    return;
}

template<typename T>
RANSAC<T>::~RANSAC()
{
    return;
}


template<typename T>
const ublas::vector<float> &  RANSAC<T>::estimate_model_parameters(const vector< T > &matches)
{


    vcl_vector< vgl_point_2d<double> > from_features;
    vcl_vector< vgl_point_2d<double> > to_features;

    vcl_vector< vgl_homg_point_2d<double> > from_features_hp;
    vcl_vector< vgl_homg_point_2d<double> > to_features_hp;

    vgl_point_2d<double> t_vgl_point_2d;
    typename vector< T >::const_iterator matches_it;

    for (matches_it = matches.begin(); matches_it != matches.end(); ++matches_it)
    {
        // we suppose that the datatype T is a ScoredMatch<T>
        t_vgl_point_2d.set(matches_it->feature_a->x, matches_it->feature_a->y);
        from_features.push_back(t_vgl_point_2d);
        from_features_hp.push_back(vgl_homg_point_2d<double>(t_vgl_point_2d));

        t_vgl_point_2d.set(matches_it->feature_b->x, matches_it->feature_b->y);
        to_features.push_back(t_vgl_point_2d);
        to_features_hp.push_back(vgl_homg_point_2d<double>(t_vgl_point_2d));
    }

    const vcl_vector< vgl_point_2d<double> >& pr = from_features;
    const vcl_vector< vgl_point_2d<double> >& pl = to_features;


    // Check that there are at least 8 points.
    if ( pr.size() < 8 || pl.size() < 8 )
    {
        vcl_cerr << "vpgl_fm_compute_ransac: Need at least 8 point pairs.\n"
        << "Number in each set: " << pr.size() << ", " << pl.size() << vcl_endl;

        throw runtime_error("RANSAC<F>::estimate_model_parameters received not enough input data");
    }

    // Check that the correspondence lists are the same size.
    if ( pr.size() != pl.size() ) // sanity check
    {
        vcl_cerr << "vpgl_fm_compute_ransac: Need correspondence lists of same size.\n";
        throw runtime_error("RANSAC<F>::estimate_model_parameters internal bug");
    }

    // The following block is hacked from similar code in rrel_homography2d_est.
    rrel_estimation_problem* estimator = NULL;

    if (use_fundamental_matrix_model)
        estimator =  new rrel_fm_problem( pr, pl );
    else if (use_homography_model)
        estimator = new rrel_homography2d_est(from_features_hp, to_features_hp);
    else
        throw runtime_error("RANSAC<F>::estimate_model_parameters no model selected");

    rrel_muset_obj* ransac = new rrel_muset_obj((int)vcl_floor(pr.size()*.75));
    estimator->set_prior_scale( 1.0 );
    rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
    ransam->set_trace_level(trace_level_);

    if (!gen_all_)
        ransam->set_sampling_params( max_outlier_frac_,
                                     desired_prob_good_, max_pops_);
    else
        ransam->set_gen_all_samples();

    if (true)
    { // just for debugging
        printf("RANSAC starting sampling\n");
    }

    bool ransac_succeeded = ransam->estimate( estimator, ransac );

    if (true)
    { // just for debugging
        printf("RANSAC finished testing %i samples\n", ransam->samples_tested());
    }


    //vpgl_fundamental_matrix<double> fm; // fundamental matrix estimate
    //if ( ransac_succeeded )
    //estimator->params_to_fm( ransam->params(), fm );

    // Get a list of the outliers.
    vcl_vector< double > residuals;
    estimator->compute_residuals( ransam->params(), residuals );

    is_inlier.clear();
    for ( unsigned i = 0; i < pr.size(); i++ )
    {
        is_inlier.push_back( ( residuals[i] < outlier_thresh_ ) );
    }

    delete ransac;
    delete ransam;
    delete estimator;

    if (ransac_succeeded == false)
        cout << "RANSAC<>::estimate_model_parameters had a problem" << endl;

    if (is_inlier.size() != matches.size()) // sanity check
        throw runtime_error("RANSAC<>::estimate_model_parameters internal bug");


    // retrieve the estimated parameters --
    //const vnl_matrix_fixed<double,3,3> &fundamental_matrix =  fundamental_matrix_estimate.get_matrix();
    estimated_model_parameters.resize(9);
    int i;
    for (i=0;i<9;i+=1)
    {
        estimated_model_parameters[i] = ransam->params()[i];
        //estimated_model_parameters[i] = fundamental_matrix[i];
        // vnl_matrix_fixed[i] is a direct access to the data region, should be ok
    }

    return estimated_model_parameters;

} // end of 'RANSAC<>::estimate_model_parameters'

template<typename T>
const vector< bool > &  RANSAC<T>::get_is_inlier()
{
    return is_inlier;
}


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=



template class RANSAC< ScoredMatch<FASTFeature> >;
template class RANSAC< ScoredMatch<SIFTFeature> >;

}
}
}
}