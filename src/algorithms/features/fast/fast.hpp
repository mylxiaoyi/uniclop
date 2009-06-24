#if !defined(FAST_HEADER_INCLUDED)
#define FAST_HEADER_INCLUDED

// this code is strictly based on the work of Edward Rosten
// available at http://mi.eng.cam.ac.uk/~er258/work/fast.html

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include "FASTFeature.hpp"


#include <vector>
#include <boost/cstdint.hpp>
#include <boost/gil/typedefs.hpp>

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Function prototypes declaration

namespace uniclop
{
namespace fast
{

using namespace std;
using boost::uint8_t;
using boost::gil::gray8c_view_t;


/** Perform a FAST feature detect on an image. The FAST feature detector
    is described in  Fusing Points and Lines for High Performance Tracking,
    E. Rosten and T. Drummond.

	This performs 9 point FAST corner detection using the algorithm as originally described.

    @param im 		The input image
    @param corners	The resulting container of corner locations
    @param barrier	Corner detection threshold (20~30 is an usual value)
**/
void corner_detect(const gray8c_view_t& view, const int barrier, vector<FASTFeature>& corners);

/** Perform non-maximal suppression on a set of FAST features. This cleans up
areas where there are multiple adjacent features, using a computed score
function to leave only the 'best' features. This function is typically called
immediately after a call to fast_corner_detect() (or one of its variants).

Will also fill the feature.circle_intensities vector (used to match or not two features).
(this will be present both in the corners and in the nonmax_corners vectors)

@param im The image used to generate the FAST features
@param corners The FAST features previously detected (e.g. by calling fast_corner_detect())
@param  barrier The barrier used to calculate the score, which should be the same as that passed to fast_corner_detect()
@param nonmax_corners Vector to be filled with the new list of non-maximally-suppressed corners
*/
void nonmax(const gray8c_view_t& view,  const int barrier, vector<FASTFeature>& corners, vector<FASTFeature>& nonmax_corners);

/*
Usage example:
FAST::corner_detect_9(image, 30, corners);
FAST::nonmax(image, 30, corners, good_corners);
*/

} // end of namespace FAST
} // end of namespace uniclop
// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

#endif // #if !defined(FAST_HEADER_INCLUDED)
