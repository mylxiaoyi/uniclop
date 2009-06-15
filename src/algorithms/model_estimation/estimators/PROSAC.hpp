
#if !defined(PROSAC_HEADER)
#define PROSAC_HEADER

namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{
namespace estimators
{

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
}
}

#endif // PROSAC_HEADER
