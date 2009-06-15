

#if !defined(IPARAMETRIC_MODEL_HEADER_INCLUDED)
#define IPARAMETRIC_MODEL_HEADER_INCLUDED

// IParametricModel


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

// C++ standard
#include <vector>

// Boost http://boost.org
#include <boost/numeric/ublas/vector.hpp>


namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{

using namespace std;
namespace ublas = boost::numeric::ublas;
	


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

}}}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

#endif // #if !defined(IPARAMETRIC_MODEL_HEADER_INCLUDED)
