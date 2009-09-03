


#include "algorithms/model_estimation/models/5point.hpp"

#include <boost/scoped_ptr.hpp>

int main(int argc, char *argv[])
{
	
	
	int ret = compute_pose_ransac(const vector< ScoredMatch> &data_points, 
                        const CalibrationMatrix &K1, const CalibrationMatrix &K2, 
                        double ransac_threshold, int ransac_rounds, 
                        RotationMatrix &R_out,  TranslationVector &t_out);
              
              
                        
                        
 //   using uniclop::FeaturesTrackingApplication;
   // using uniclop::AbstractApplication;

//    boost::scoped_ptr<AbstractApplication> application_p(new FeaturesTrackingApplication());
 //   return application_p->main(argc, argv);
}