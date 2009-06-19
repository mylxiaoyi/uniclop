

#if !defined(HOMOGRAPHY_MODEL_HEADER)
#define HOMOGRAPHY_MODEL_HEADER


#include "../IParametricModel.hpp"


namespace uniclop
{
namespace algorithms
{
	namespace features {
		template<typename F>
class ScoredMatch;
	}
	
namespace model_estimation
{
namespace models
{

using namespace uniclop::algorithms::model_estimation;
using uniclop::algorithms::features::ScoredMatch;


// class HomographyModel
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

}
}
}
}

#endif // HOMOGRAPHY_MODEL_HEADER

