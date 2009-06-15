
#if !defined(SIMPLE_FAST_HEADER_INCLUDED)
#define SIMPLE_FAST_HEADER_INCLUDED

// Features detection

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include "../IFeaturesDetector.hpp"
#include "FASTFeature.hpp"

#include <vector>

#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>
#include <boost/gil/typedefs.hpp>


namespace uniclop
{
namespace algorithms
{
namespace features
{
namespace fast
{


using uniclop::algorithms::features::IFeaturesDetector;

using namespace std;

namespace args = boost::program_options;
using boost::uint8_t;
using boost::gil::gray8c_view_t;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition


class SimpleFAST : public IFeaturesDetector<FASTFeature>
{ // simplest implementation of FAST, based on the implementation of Edward Rosten
    // FAST features detection and matching by Edward Rosten and Tom Drummond

    vector<FASTFeature> detected_features, best_features;
    int barrier;
public:
    static args::options_description get_options_description();

    SimpleFAST(args::variables_map &options);
    ~SimpleFAST();

    const vector<FASTFeature> &detect_features(const gray8c_view_t& view);
};


}
}
}
}

#endif // #if !defined(SIMPLE_FAST_HEADER_INCLUDED)
