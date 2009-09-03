
#if !defined(ESSENTIAL_MATRIX_HEADER)
#define ESSENTIAL_MATRIX_HEADER

namespace uniclop {
    
#include <Eigen/Core>
    
class EssentialMatrix : public Eigen:Matrix<float, 3,3>
{
public:
    EssentialMatrix();
    ~EssentialMatrix();
    
    // void compute_rotation_translation(vector match, matrix R, vector t);
};

} // end of namespace uniclop


#endif //  ESSENTIAL_MATRIX_HEADER


