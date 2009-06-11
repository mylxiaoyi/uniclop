
#if !defined(FEATURES_DETECTION_HEADER_INCLUDED)
#define FEATURES_DETECTION_HEADER_INCLUDED

// Features detection

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

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

#include <boost/gil/typedefs.hpp>
using boost::gil::gray8c_view_t;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition

// T is the class of the children
template<typename T>
class IFeature
{
public:
    int x,y; ///< position

    virtual float distance(const T& another_feature) const = 0;
    ///< measure distance to another feature of his own type

    IFeature()
    {
        return;
    }
    virtual ~IFeature()
    {
        return;
    }
};

// F is the type of feature used
template<typename F>
class IFeaturesDetector
{
    vector<F> detected_features;
public:
    typedef F features_type;

    virtual const vector<F> &detect_features(const CImg<uint8_t> &image) = 0;

    IFeaturesDetector()
    {
        return;
    }
    virtual ~IFeaturesDetector()
    {
        return;
    }
};


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
//  FAST features detection implementation
//  FAST features detection and matching by Edward Rosten and Tom Drummond

class FASTFeature : public IFeature<FASTFeature>
{

public:
    // int x,y herited from IFeature

    uint8_t circle_intensities[16];
    // see Edward Rosten PhD thesis chapter 2.4 "Efficient feature matching Features"
    // http://mi.eng.cam.ac.uk/~er258/work/rosten_2006_thesis.pdf

    FASTFeature();
    FASTFeature(const FASTFeature&);
    ~FASTFeature();


    ///@name IFeature interface
    ///@{
    float distance(const FASTFeature &f) const;
    ///@}
};


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


#endif // #if !defined(FEATURES_DETECTION_HEADER_INCLUDED)
