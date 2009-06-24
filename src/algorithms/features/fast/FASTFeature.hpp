

#if !defined(FAST_FEATURE_HEADER_INCLUDED)
#define FAST_FEATURE_HEADER_INCLUDED


#include "../IFeature.hpp"

// Features detection

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include <boost/cstdint.hpp>

namespace uniclop
{

using uniclop::algorithms::features::IFeature;

using namespace std;

using boost::uint8_t;


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition


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


}

#endif // FAST_FEATURE_HEADER_INCLUDED

