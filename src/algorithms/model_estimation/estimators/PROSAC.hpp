
#if !defined(PROSAC_HEADER)
#define PROSAC_HEADER

/**
 PROSAC - Progressive Sample Consensus

PROSAC is a simple algorithm that provides a good improvement (10x~20x faster) over RANSAC, while being simpler to implement than ARRSAC

Abstract
A new robust matching method is proposed. The Progressive Sample Consensus (PROSAC) algorithm exploits the linear ordering defined on the set of correspondences by a similarity function used in establishing tentative correspondences. Unlike RANSAC, which treats all correspondences equally and draws random samples uniformly from the full set, PROSAC samples are drawn from progressively larger sets of top-ranked correspondences. Under the mild assumption that the similarity measure predicts correctness of a match better than random guessing, we show that PROSAC achieves large computational savings. Experiments demonstrate it is often significantly faster (up to more than hundred times) than RANSAC. For the derived size of the sampled set of correspondences as a function of the number of samples already drawn, PROSAC converges towards RANSAC in the worst case. The power of the method is demonstrated on widebaseline matching problems.

*/

#include "../IModelEstimator.hpp"
#include "../IParametricModel.hpp"

#include <boost/program_options.hpp>


namespace uniclop
{
	
namespace args = ::boost::program_options;


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

}

#endif // PROSAC_HEADER
