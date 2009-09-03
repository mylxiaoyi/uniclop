
#if !defined(CALIBRATION_MATRIX_HEADER)
#define CALIBRATION_MATRIX_HEADER

namespace uniclop {
    
    
#include <Eigen/Core>

class CalibrationMatrix : public Eigen:Matrix<float, 3,3>
{
public:
    CalibrationMatrix();
    ~CalibrationMatrix();
    
};

} // end of namespace uniclop


#endif //  CALIBRATION_MATRIX_HEADER


