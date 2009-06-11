
#if !defined(SIMPLE_SIFT_HEADER)
#define SIMPLE_SIFT_HEADER

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

// forward declaration to avoid including directly JIFT.h
namespace JIFT
{
class JIFT;
}

// forward declaration to avoid including directly CImg.h
namespace cimg_library
{
template<typename T> struct CImg;
}
using namespace cimg_library;

#include <vector>
using namespace std;

// Boost http://boost.org
#include <boost/cstdint.hpp>
using boost::uint8_t;

#include <boost/program_options.hpp>
namespace args = boost::program_options;

#include <boost/scoped_ptr.hpp>
// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// SIFT Features detection

class SIFTFeature : public IFeature<SIFTFeature>
{

    // its feature vector
    vector<double> feature_vector;

public:
    // int x,y herited from IFeature

    SIFTFeature();
    SIFTFeature(const SIFTFeature&);
    ~SIFTFeature();

    // constructor used by JIFT code
    SIFTFeature(float _x, float _y, const vector<double>& _feature_vector);

    ///@name IFeature interface
    ///@{
    float distance(const SIFTFeature &f) const;
    ///@}
};


class SimpleSIFT : public IFeaturesDetector<SIFTFeature>
{ // simple non optimized implementation of SIFT, based on the implementation of Jun Liu

    boost::scoped_ptr< JIFT::JIFT > jift_features_detector_p;

public:
    static args::options_description get_options_description();

    SimpleSIFT(args::variables_map &options);
    ~SimpleSIFT();

    const vector<SIFTFeature> &detect_features(const CImg<uint8_t> &image);
};
