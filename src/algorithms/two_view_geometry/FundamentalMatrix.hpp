
#if !defined(FUNDAMENTAL_MATRIX_HEADER)
#define FUNDAMENTAL_MATRIX_HEADER

namespace uniclop {
    
#include <Eigen/Core>

class FundamentalMatrix : public Eigen:Matrix<float, 3,3>
{
public:
    FundamentalMatrix();
    ~FundamentalMatrix();
    
};

} // end of namespace uniclop


#endif //  FUNDAMENTAL_MATRIX_HEADER


