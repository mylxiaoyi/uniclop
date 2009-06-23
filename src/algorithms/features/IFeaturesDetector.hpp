
#if !defined(IFEATURES_DETECTOR_HEADER_INCLUDED)
#define IFEATURES_DETECTOR_HEADER_INCLUDED

// Features detection

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include <vector>

#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/typedefs.hpp>



namespace uniclop
{
namespace algorithms
{
namespace features
{

using namespace std;

namespace args = boost::program_options;
using boost::uint8_t;
using boost::gil::gray8c_view_t;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition


// F is the type of feature used
template<typename F>
class IFeaturesDetector
{
    vector<F> detected_features;
public:
    typedef F features_type;

	template<typename ImageView>
    virtual const vector<F> &detect_features(const ImageView& image) = 0;

    IFeaturesDetector()
    {
        return;
    }
    virtual ~IFeaturesDetector()
    {
        return;
    }
};


}
}
}

#endif // IFEATURES_DETECTOR_HEADER_INCLUDED

