
#if !defined(MODEL_ESTIMATION_HEADER_INCLUDED)
#define MODEL_ESTIMATION_HEADER_INCLUDED


#include "estimators/RANSAC.hpp"
#include "estimators/PROSAC.hpp"
#include "estimators/Ensemble.hpp"


// Model estimation, inliers estimation

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

// C++ standard
#include <limits> // to use numeric_limits<float>::max() and similars
#include <vector>
#include <algorithm>

// Boost http://boost.org
#include <boost/program_options.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/random.hpp>

// CImg http://cimg.sf.net
// forward declaration to avoid including directly CImg.h
namespace cimg_library
{
template<typename T> struct CImg;
struct CImgDisplay;
}

#include "algorithms/features/features_matching.hpp"

namespace uniclop
{
namespace algorithms
{
namespace model_estimation
{

using namespace cimg_library;
using namespace std;
namespace args = boost::program_options;
namespace ublas = boost::numeric::ublas;

using namespace uniclop::algorithms::features;

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition



// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Models classes definition


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Estimator classes definition





/

}
}
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

#endif // #if !defined(MODEL_ESTIMATION_HEADER_INCLUDED)
