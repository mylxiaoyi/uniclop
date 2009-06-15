

#include "FundamentalMatrixModel.hpp"

// RANSAC implementation requires VXL installed
// with the RREL and GEL/vpgl contributions

//#include <vxl/core/vnl/vnl_math.h>
//#include <vxl/vcl/vcl_iostream.h>
//#include <vxl/vcl/vcl_cassert.h>
//#include <vxl/vcl/vcl_cmath.h>
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
namespace models
{



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




}
}
}
}