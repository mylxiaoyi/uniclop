

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include "features_detection.hpp"

#include "fast/fast.hpp"



// Cimg http://cimg.sf.net
#include <CImg/CImg.h>


namespace uniclop
{
namespace algorithms
{
namespace features
{


using namespace cimg_library;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class FASTFeature methods implementation

FASTFeature::FASTFeature()
{
    return;
}

FASTFeature::FASTFeature(const FASTFeature& f) : IFeature<FASTFeature>()
{
    x = f.x;
    y = f.y;

    int i;
    for (i=0;i<16;i+=1)
        circle_intensities[i] = f.circle_intensities[i];

    return;
}

FASTFeature::~FASTFeature()
{
    return;
}


template <class R, class D, class T> struct SumSquaredDifferences
{
    static inline R sum_squared_differences(const T* a, const T* b, size_t count)
    {
        R ssd = 0;
        while (count--)
        {
            D d = *a++ - *b++;
            ssd += d*d;
        }
        return ssd;
    }
};

/// Compute sum of (a_i - b_i)^2 (the SSD)
/// This is accelerated using SIMD for some platforms and data types (alignment is checked at runtime)
/// Do not specify template parameters explicitly so that overloading can choose the right implementation
/// Look at LibCVD to get the SIMD accelerated versions
template <class T> inline float sum_squared_differences(const T* a, const T* b, size_t count)
{
    return SumSquaredDifferences<float,float,T>::sum_squared_differences(a,b,count);
}

float FASTFeature::distance(const FASTFeature &f) const
{
    return sum_squared_differences(circle_intensities, f.circle_intensities, 16);
}


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class SimpleFAST methods implementation

args::options_description SimpleFAST::get_options_description()
{

    args::options_description desc("SimpleFAST options");
    desc.add_options()

    ( "fast.barrier", args::value<int>()->default_value(20),
      "threshold used to detect FAST features")
    ;

    return desc;
}

SimpleFAST::SimpleFAST(args::variables_map &options)
{
    barrier = 20; // default value

    if ( options.count("fast.barrier") )
        barrier = options["fast.barrier"].as<int>();

    return;
}

SimpleFAST::~SimpleFAST()
{
    return;
}

const vector<FASTFeature> & SimpleFAST::detect_features(const gray8c_view_t& view)
{
    // no need to clear the vectors features it is done inside the functions

    // find corners
    FAST::corner_detect(view, barrier, detected_features);

    // keep the best ones
    FAST::nonmax(view, barrier, detected_features, best_features);

    return best_features;
}

}
}
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
