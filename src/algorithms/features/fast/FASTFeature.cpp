

#include "FASTFeature.hpp"

#include <stdexcept>

namespace uniclop
{

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class FASTFeature methods implementation

FASTFeature::FASTFeature()
{
    return;
}

FASTFeature::FASTFeature(const FASTFeature& f) : IFeature()
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

float FASTFeature::distance(const IFeature &f) const {

	const FASTFeature *feature_p = dynamic_cast<const FASTFeature *>(&f);
	
	if(feature_p == NULL) {
		std::runtime_error("FASTFeature::distance received an unexpected feature type");
	}
		
	return distance(*feature_p);
}

   
float FASTFeature::distance(const FASTFeature &f) const
{
    return sum_squared_differences(circle_intensities, f.circle_intensities, 16);
}


}

