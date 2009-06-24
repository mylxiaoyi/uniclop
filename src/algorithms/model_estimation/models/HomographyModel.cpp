
#include "HomographyModel.hpp"
#include "algorithms/features/ScoredMatch.hpp"

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

#include <boost/numeric/ublas/io.hpp>

namespace uniclop
{


// Class HomographyModel:IParametricModel< ScoredMatch<F> > methods implementation

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
    for ( int i=0; i < get_num_points_to_estimate(); i+=1 )
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

   if ( false ) {
	    cout << "HomographyModel parameters: " << parameters << endl;
   }
   
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
            const double t_value = vnl_math_sqr(del_x) + vnl_math_sqr(del_y)
                                      + vnl_math_sqr(inv_del_x) + vnl_math_sqr(inv_del_y);
            *residuals_it = vcl_sqrt( t_value );
        }


    } // end of 'for each data point'

    return;
} // end of method FundamentalMatrixModel<F>::compute_residuals





}