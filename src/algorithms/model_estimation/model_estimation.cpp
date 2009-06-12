
// Model estimation, inliers estimation

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers
#include "model_estimation.hpp"

// extra implementation specific headers ---
#include "features_detection.hpp"
#include "kmeans.hpp"

// Boost http://boost.org
#include <boost/numeric/ublas/io.hpp>
#include <boost/tuple/tuple.hpp>

// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;

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
#include <vpgl/vpgl_fm_compute_ransac.h>
#include <vpgl/vpgl_fm_compute_8_point.h>



// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class FundamentalMatrixModel: IParametricModel< ScoredMatch<F> > methods implementation

template<typename F>
FundamentalMatrixModel<F>::FundamentalMatrixModel()
{
    parameters.resize( get_num_parameters() );
    return;
}

template<typename F>
FundamentalMatrixModel<F>::~FundamentalMatrixModel()
{
    return;
}


template<typename F>
unsigned int FundamentalMatrixModel<F>::get_num_parameters() const
{ // get the number of free parameters of the model
    return 9;
}

template<typename F>
unsigned int FundamentalMatrixModel<F>::get_num_points_to_estimate() const
{ // m: is the number of points required to estimate the parameters of the model
    return 8;
}


template<typename F>
void FundamentalMatrixModel<F>::estimate_from_minimal_set(const vector< ScoredMatch<F> > &data_points)
{ // given m points estimate the parameters vector

    // based on vxl rrel_fm_problem::fit_from_minimal_set
    if ( data_points.size() < get_num_points_to_estimate())
        throw runtime_error("Not enough points to estimate the FundamentalMatrixModel parameters");

    vcl_vector< vgl_homg_point_2d<double> > set_pr, set_pl;
    for ( int i = 0; i < 8; i++ )
    {
        const ScoredMatch<F> data_point = data_points[i];
        set_pr.push_back( vgl_homg_point_2d<double>(data_point.feature_b->x, data_point.feature_b->y) );
        set_pl.push_back( vgl_homg_point_2d<double>(data_point.feature_a->x, data_point.feature_a->y) );
    }

    vpgl_fundamental_matrix<double> fundamental_matrix;
    vpgl_fm_compute_8_point fmc8(true);
    bool success = fmc8.compute( set_pr, set_pl, fundamental_matrix );

    if ( success == false )
        throw runtime_error("FundamentalMatrixModel<F>::estimate_from_minimal_set failed");

    // fundamental_matrix_to_parameters( fundamental_matrix, parameters );
    parameters.resize(9);
    vnl_matrix_fixed<double,3,3> fm_vnl = fundamental_matrix.get_matrix();
    for ( int r = 0; r < 3; r++ )
        for ( int c = 0; c < 3; c++ )
            parameters[ 3*r + c ] = fm_vnl( r, c );

    if ( false ) cout << "FundamentalMatrixModel parameters: " << parameters << endl;

    return;
}

template<typename F>
void FundamentalMatrixModel<F>::estimate(const vector< ScoredMatch<F> > &data_points)
{ // given n>m points, estimate the parameters vector

    //throw runtime_error("FundamentalMatrixModel<F>::estimate is not yet implemented");
    // <<< can implement this based on FMatrixComputeLinear
    cout << "FundamentalMatrixModel<F>::estimate is not yet implemented" << endl;
    return;
}

template<typename F>
const ublas::vector<float>& FundamentalMatrixModel<F>::get_parameters() const
{ // get current estimate of the parameters
    return parameters;
}

template<typename F>
void FundamentalMatrixModel<F>::set_parameters(const ublas::vector<float> &_parameters)
{
    // set an initial guess of the parameters
    // (useful when the model use iterative methods to estimate his parameters)
    parameters = _parameters;
    return;
}

template<typename F>
void FundamentalMatrixModel<F>::compute_residuals
(const vector< ScoredMatch<F> > &data_points, vector<float> &residuals) const
{
    // residuals -> errors
    // Compute the residuals relative to the given parameter vector.

    vpgl_fundamental_matrix<double> fundamental_matrix;
    // fill the fundamental_matrix
    vnl_matrix_fixed<double,3,3> fm_vnl;
    for ( int r = 0; r < 3; r++ )
        for ( int c = 0; c < 3; c++ )
            fm_vnl( r, c ) = parameters[ 3*r + c ];
    fundamental_matrix.set_matrix( fm_vnl );

    residuals.resize(data_points.size());

    // The residual for each correspondence is the sum of the squared distances from
    // the points to their epipolar lines.

    typename vector< ScoredMatch<F> >::const_iterator data_points_it;
    vector<float>::iterator residuals_it;

    for (data_points_it = data_points.begin(), residuals_it = residuals.begin();
            data_points_it != data_points.end() && residuals_it != residuals.end();
            ++data_points_it, ++residuals_it)
    {
        // left is feature a, right is feature b (because 'abcde..' )

        vgl_homg_point_2d<double> t_pl(data_points_it->feature_a->x, data_points_it->feature_a->y);
        vgl_homg_point_2d<double> t_pr(data_points_it->feature_b->x, data_points_it->feature_b->y);

        vgl_homg_line_2d<double> lr = fundamental_matrix.r_epipolar_line( t_pl );
        vgl_homg_line_2d<double> ll = fundamental_matrix.l_epipolar_line( t_pr );
        *residuals_it = vgl_homg_operators_2d<double>::perp_dist_squared( lr, t_pr )
                        + vgl_homg_operators_2d<double>::perp_dist_squared( ll, t_pl );
    } // end of 'for each data point'

    return;
} // end of method FundamentalMatrixModel<F>::compute_residuals


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Class HomographyModel: IParametricModel< ScoredMatch<F> > methods implementation

// based on vxl rrel_homography2d_est, HMatrix2D and HMatrix2DCompute

template<typename F>
HomographyModel<F>::HomographyModel()
{
    parameters.resize( get_num_parameters() );
    return;
}

template<typename F>
HomographyModel<F>::~HomographyModel()
{
    return;
}


template<typename F>
unsigned int HomographyModel<F>::get_num_parameters() const
{ // get the number of free parameters of the model
    return 9;
    // we have only 8 degrees of freedom but it is easier (laziest) to deal with 9 parameters
}

template<typename F>
unsigned int HomographyModel<F>::get_num_points_to_estimate() const
{ // m: is the number of points required to estimate the parameters of the model
    return 4;
}


template<typename F>
void HomographyModel<F>::estimate_from_minimal_set(const vector< ScoredMatch<F> > &data_points)
{ // given m points estimate the parameters vector

    // based on vxl rrel_homography2d_est :: fit_from_minimal_set
    if ( data_points.size() < get_num_points_to_estimate())
        throw runtime_error("Not enough points to estimate the HomographyModel parameters");

    vnl_matrix< double > A(9, 9, 0.0);
    for ( unsigned int i=0; i<get_num_points_to_estimate(); i+=1 )
    { // for i = 0,1,2,3
        vgl_homg_point_2d<double> from_point(data_points[i].feature_a->x, data_points[i].feature_a->y);
        vgl_homg_point_2d<double> to_point(data_points[i].feature_b->x, data_points[i].feature_b->y);

        if (false)
        { // just for debugging
            printf("from_points[%i] -> to_points[%i] ==", i,i);
            cout << from_point << " -> " << to_point << endl;
        }

        A( 2*i, 0 ) = A( 2*i+1, 3 ) = from_point.x() * to_point.w();
        A( 2*i, 1 ) = A( 2*i+1, 4 ) = from_point.y() * to_point.w();
        A( 2*i, 2 ) = A( 2*i+1, 5 ) = from_point.w() * to_point.w();
        A( 2*i, 6 ) = -1 * from_point.x() * to_point.x();
        A( 2*i, 7 ) = -1 * from_point.y() * to_point.x();
        A( 2*i, 8 ) = -1 * from_point.w() * to_point.x();
        A( 2*i+1, 6 ) = -1 * from_point.x() * to_point.y();
        A( 2*i+1, 7 ) = -1 * from_point.y() * to_point.y();
        A( 2*i+1, 8 ) = -1 * from_point.w() * to_point.y();
    }


    vnl_svd<double> svd( A, 1.0e-8 );

    if (false)
    { // just for debugging

        cout << "A == " << A << endl;
        cout << "svd(A) == " << svd << endl;
    }


    const unsigned int homog_dof_ = 8;
    if ( svd.rank() < homog_dof_ )
    {
        // singular fit
        if (true)
        { // just for debugging
            cout << "svd.rank() == " << svd.rank() << endl;

            for ( unsigned int i=0; i<get_num_points_to_estimate(); ++i )
            {
                vgl_homg_point_2d<double> from_point(data_points[i].feature_a->x, data_points[i].feature_a->y);
                vgl_homg_point_2d<double> to_point(data_points[i].feature_b->x, data_points[i].feature_b->y);

                cout << "from->to point[i]-> " << from_point << "->" << to_point << endl;
            }

        }

        throw runtime_error("HomographyModel<F>::estimate_from_minimal_set failed");
    }

    vnl_vector<double> params = svd.nullvector();
    parameters.resize(get_num_parameters());

    if (params.size() != parameters.size() )
    {
        throw runtime_error("HomographyModel<F>::estimate_from_minimal_set internal error");
    }

    for ( unsigned int i=0; i<get_num_parameters(); i+=1 )
    {
        parameters[i] = params[i]; // copy the result
    }

    if ( false ) cout << "HomographyModel parameters: " << parameters << endl;

    return;
} // end of 'HomographyModel<F>::estimate_from_minimal_set'


template<typename F>
void HomographyModel<F>::estimate(const vector< ScoredMatch<F> > &data_points)
{ // given n>m points, estimate the parameters vector

    //throw runtime_error("HomographyModel<F>::estimate is not yet implemented");
    // <<< can implement this based on HMatrix2DCompute
    cout << "HomographyModel<F>::estimate is not yet implemented" << endl;

    return;
}

template<typename F>
const ublas::vector<float>& HomographyModel<F>::get_parameters() const
{ // get current estimate of the parameters
    return parameters;
}

template<typename F>
void HomographyModel<F>::set_parameters(const ublas::vector<float> &_parameters)
{
    // set an initial guess of the parameters
    // (useful when the model use iterative methods to estimate his parameters)
    parameters = _parameters;
    return;
}

template<typename F>
void HomographyModel<F>::compute_residuals
(const vector< ScoredMatch<F> > &data_points, vector<float> &residuals) const
{
    // residuals -> errors
    // Compute the residuals relative to the given parameter vector.

// based on rrel_homography2d_est :: compute_residuals

    vnl_matrix< double > H(3,3);
    int r,c;
    for ( r=0; r<3; ++r )
        for ( c=0; c<3; ++c )
            H( r, c ) = parameters[ 3*r + c ];

    vnl_svd< double > svd_H( H );
    if ( svd_H.rank() < 3 )
    {
        if (true) cout << "H == " << H << endl;
        //throw runtime_error("HomographyModel<F>::compute_residuals rank(H) < 3!!");
        cout << "HomographyModel<F>::compute_residuals rank(H) < 3!!" << endl;
    }
    vnl_matrix< double > H_inv( svd_H.inverse() );

    residuals.resize(data_points.size());

    // compute the residual of each data point
    typename vector< ScoredMatch<F> >::const_iterator data_points_it;
    vector<float>::iterator residuals_it;

    vnl_vector< double > from_point( 3 ), to_point( 3 );
    vnl_vector< double > trans_pt( 3 ), inv_trans_pt( 3 );
    double del_x, del_y, inv_del_x, inv_del_y;

    for (data_points_it = data_points.begin(), residuals_it = residuals.begin();
            data_points_it != data_points.end() && residuals_it != residuals.end();
            ++data_points_it, ++residuals_it)
    {
        // from feature a to feature b
        from_point[0] = data_points_it->feature_a->x;
        from_point[1] = data_points_it->feature_a->y;
        from_point[2] = 1.0;

        to_point[0] = data_points_it->feature_b->x;
        to_point[1] = data_points_it->feature_b->y;
        to_point[2] = 1.0;

        trans_pt = H * from_point;
        inv_trans_pt = H_inv * to_point;

        if ( from_point[ 2 ] == 0 || to_point[ 2 ] == 0
                || trans_pt[ 2 ] == 0 || inv_trans_pt[ 2 ] == 0 )
        {
            *residuals_it = 1e10;
        }
        else
        {
            del_x = trans_pt[ 0 ] / trans_pt[ 2 ] - to_point[ 0 ] /to_point[ 2 ];
            del_y = trans_pt[ 1 ] / trans_pt[ 2 ] - to_point[ 1 ] / to_point[ 2 ];
            inv_del_x = inv_trans_pt[ 0 ] / inv_trans_pt[ 2 ] -from_point[ 0 ] / from_point[ 2 ];
            inv_del_y = inv_trans_pt[ 1 ] / inv_trans_pt[ 2 ] - from_point[ 1 ] / from_point[ 2 ];
            *residuals_it = vcl_sqrt( vnl_math_sqr(del_x)     + vnl_math_sqr(del_y)
                                      + vnl_math_sqr(inv_del_x) + vnl_math_sqr(inv_del_y) );
        }


    } // end of 'for each data point'

    return;
} // end of method FundamentalMatrixModel<F>::compute_residuals

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
// Class EnsembleMethod methods implementation

// implementation of the W. Zhang and J. Kosecka "Ensemble method for robust estimation"
// http://www.cs.gmu.edu/%7Ekosecka/Publications/EnsembleMethod.pdf




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

    histogram_image.draw_text(50,5,blue,NULL,font_size,1.0,"kurtosis == %f", kurtosis);

    image_display.resize(histogram_image);
    image_display.display(histogram_image);

    return;
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Force the compilation of the following types
// for some strange reason, in linux, this has to be at the end of the defitions (?!)

class FASTFeature; // forward declaration
class SIFTFeature; // forward declaration

template class FundamentalMatrixModel<FASTFeature>;
template class FundamentalMatrixModel<SIFTFeature>;

template class HomographyModel<FASTFeature>;
template class HomographyModel<SIFTFeature>;

template void HistogramKurtosis<double>::display_histogram
(const float, const vector<int> &, const int , CImgDisplay &);

template<typename F> class ScoredMatch; // forward declaration

template<>
void EnsembleMethod< ScoredMatch<FASTFeature> >::retrieve_random_indexes
(const vector< ScoredMatch<FASTFeature> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}

template<>
void EnsembleMethod< ScoredMatch<SIFTFeature> >::retrieve_random_indexes
(const vector< ScoredMatch<SIFTFeature> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}

template<>
void EnsembleMethod< boost::tuple<float, float> >::retrieve_random_indexes
(const vector< boost::tuple<float, float> > &data, const unsigned int num_indexes,
 vector<int> &indexes)
{
    return retrieve_random_matches_indexes(data, num_indexes, indexes);
}

template class EnsembleMethod< ScoredMatch<FASTFeature> >;
template class EnsembleMethod< ScoredMatch<SIFTFeature> >;
template class EnsembleMethod< boost::tuple<float, float> >;

template class PROSAC< ScoredMatch<FASTFeature> >;
template class PROSAC< ScoredMatch<SIFTFeature> >;

template class RANSAC< ScoredMatch<FASTFeature> >;
template class RANSAC< ScoredMatch<SIFTFeature> >;
// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=



