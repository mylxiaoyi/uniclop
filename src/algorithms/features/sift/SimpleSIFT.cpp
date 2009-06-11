

#include "../features_detection.hpp"

#include "SimpleSIFT.hpp"

// implementation specific headers
#include "sift/jift/JIFT.hpp"



// Cimg http://cimg.sf.net
#include <CImg/CImg.h>
using namespace cimg_library;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class SIFTFeature methods implementation

SIFTFeature::SIFTFeature()
{
    // nothing to do here
    return;
}

SIFTFeature::SIFTFeature(const SIFTFeature& f)
{
    x = f.x;
    y = f.y;
    feature_vector = f.feature_vector;
    return;
}

SIFTFeature::~SIFTFeature()
{
    // nothing to do here
    return;
}

// constructor used by JIFT code
SIFTFeature::SIFTFeature(float _x, float _y, const vector<double>& _feature_vector)
        : feature_vector(_feature_vector)
{
    x = static_cast<int>(_x);
    y = static_cast<int>(_y);
    return;
}



float SIFTFeature::distance(const SIFTFeature &f) const
{
    vector<double>::const_iterator v1_it, v2_it;

    float distance = 0.0f;
    for (v1_it = feature_vector.begin(), v2_it = f.feature_vector.begin();
            v1_it != feature_vector.end() && v2_it != f.feature_vector.end();
            ++v1_it, ++v2_it)
    {
        const float t_val = (*v1_it) - (*v2_it);
        distance += t_val*t_val;
    }

    return distance;
}





// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class SimpleSIFT methods implementation

args::options_description SimpleSIFT::get_options_description()
{
    args::options_description desc("SimpleSIFT options");
    desc.add_options()

    ( "sift.octaves", args::value<int>()->default_value(4),
      "number of levels used in the image pyramid")
    ( "sift.intervals", args::value<int>()->default_value(2),
      "??")
    ;

    return desc;
}

SimpleSIFT::SimpleSIFT(args::variables_map &options)
{

    int octaves = 4;
    if ( options.count("sift.octaves") )
        octaves = options["sift.octaves"].as<int>();

    int intervals = 2;
    if ( options.count("sift.intervals") )
        intervals = options["sift.intervals"].as<int>();

    jift_features_detector_p.reset(new JIFT::JIFT(octaves, intervals));
    return;
}

SimpleSIFT::~SimpleSIFT()
{
    // nothing to do here, scoped_ptr takes care of the dealocations
    return;
}

const vector<SIFTFeature> & SimpleSIFT::detect_features(const CImg<uint8_t> &image)
{
    vil_image_view<vxl_byte> image_view(image.ptr(), image.dimx(), image.dimy(), 1,
                                        1, image.dimx(), 1);
    jift_features_detector_p->doJIFT(image_view);

    return jift_features_detector_p->descriptor();
}
