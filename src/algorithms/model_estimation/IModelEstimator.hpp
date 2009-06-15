

#if !defined(IMODEL_ESTIMATOR_HEADER)
#define IMODEL_ESTIMATOR_HEADER
// class IModelEstimator


#include <vector>
#include <boost/numeric/ublas/vector.hpp>

namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{

using namespace std;
namespace ublas = boost::numeric::ublas;


template<typename T>
class IModelEstimator // T is the input data type
{

public:
    virtual const ublas::vector<float> &estimate_model_parameters(const std::vector<T> &) = 0;
    // given a set of matches, returns an vector with the estimated parameters

    virtual const std::vector< bool > & get_is_inlier() = 0;
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


}
}
}

#endif // IMODEL_ESTIMATOR_HEADER
