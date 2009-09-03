/* 
 *  Copyright (c) 2008  Noah Snavely (snavely (at) cs.washington.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* 5point.h */
/* Solve the 5-point relative pose problem */

#ifndef 5POINT_HEADER
#define 5POINT_HEADER

#include <Eigen/Core>

typedef Eigen:Matrix<float, 3,3> RotationMatrix;
typedef Eigen:Matrix<float, 3,1> TranslationVector;

int compute_pose_ransac(const vector< ScoredMatch> &data_points, 
                        const CalibrationMatrix &K1, const CalibrationMatrix &K2, 
                        double ransac_threshold, int ransac_rounds, 
                        RotationMatrix &R_out,  TranslationVector &t_out);

#endif /* 5POINT_HEADER */
