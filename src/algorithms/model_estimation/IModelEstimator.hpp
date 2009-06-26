

#if !defined(IMODEL_ESTIMATOR_HEADER)
#define IMODEL_ESTIMATOR_HEADER
// class IModelEstimator


#include <vector>
#include <boost/numeric/ublas/vector.hpp>

namespace uniclop
{

namespace ublas = boost::numeric::ublas;

class ScoredMatch;

class IModelEstimator // T is the input data type
{

public:
    virtual const ublas::vector<float> &estimate_model_parameters(const std::vector< ScoredMatch > &) = 0;
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

#endif // IMODEL_ESTIMATOR_HEADER
