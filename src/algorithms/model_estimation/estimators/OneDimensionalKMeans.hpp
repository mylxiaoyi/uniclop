

#if !defined(ONE_DIMENSIONAL_KMEANS_HEADER)
#define ONE_DIMENSIONAL_KMEANS_HEADER

// C++ implementation of  1-D kmeans
// an adaptation of the code wrote by Lucas Scharenbroich (2005)

#include <vector>

namespace uniclop
{
	
	
using namespace std;


class OneDimensionalKMeans
{

public:

    OneDimensionalKMeans();
    ~OneDimensionalKMeans();

    void compute(vector<double> &data, const unsigned int num_clusters);
    // input data is non const because the data will be reordered
    const vector<double> & get_kmeans();
    const vector<unsigned int> &get_splits();
    const vector<unsigned int> &get_permutations();

private:

    vector<unsigned int> *split_p, *prev_p;
    vector<unsigned int> partition1, partition2, perm;
    vector<double> mean;

    bool changed( const vector<unsigned int> &a, const vector<unsigned int> &b);
    unsigned int find_boundary(const vector<double> &data,
                               const vector<unsigned int> &a,
                               double x, double y );

};

}

//
//#include <stdio.h>
//#include <stdlib.h>
//
///** Pointer to the data array */
//static double *data = NULL;
//
///**
// * This comparison function takes two integer values and compares the double
// * values stored in the array at the given indices.
// */
//int my_compar( const void *a, const void *b )
//{
//  unsigned int i = *( (unsigned int *) a );
//  unsigned int j = *( (unsigned int *) b );
//
//  if ( data[i] < data[j] ) return -1;
//  if ( data[i] > data[j] ) return  1;
//  return 0;
//}
//
///**
// * Finds the index of the datapoint in the array which is the midpoint
// * between the values of x and y.  This is done in logarithmic time using
// * a binary search.
// *
// * There are some subtle issues here.  First, when moving from a midpoint,
// * we need to keep the selected midpoint as the upper or lower bound, since
// * we might otherwise overstep the proper value.  At the end, we need to
// * check whether the upper or lower value is the best.
// */
//unsigned int find_boundary( unsigned int *a, unsigned int n, double x, double y )
//{
//  unsigned int lower, upper, mid;
//  double w, z, lower_sum, upper_sum;
//
//  lower = 0;
//  upper = n - 1;
//
//  while ( lower < upper - 1 )
//  {
//    mid = ( upper + lower ) / 2;
//    z   = data[a[mid]];
//
//    if ( y - z <= z - x )
//      upper = mid;
//    else
//      lower = mid;
//  }
//
//  /** Find the best one */
//  w = data[a[lower]];
//  z = data[a[upper]];
//
//  lower_sum = (w - x) * (w - x) + (y - w) * (y - w);
//  upper_sum = (z - x) * (z - x) + (y - z) * (y - z);
//
//  return ( lower_sum < upper_sum ) ? lower : upper;
//}
//
//static void swap( unsigned int **a, unsigned int **b )
//{
//  unsigned int *c = *a;
//
//  *a = *b;
//  *b = c;
//}
//
///**
// * Compare two partitionings of the data and return true if they differ
// * from each other or false if not.  This is the convergence criteria for
// * the kmeans clustering.
// */
//static int changed( unsigned int *a, unsigned int *b, unsigned int n )
//{
//  unsigned int i;
//
//  for ( i = 0; i < n; i++ )
//    if ( a[i] != b[i] )
//      return 1;
//
//  return 0;
//}
//
///**
// * Perform k-means clustering on a one-dimensional data set.  Due to the
// * ordering imposed by scalar data, we can make the code much more
// * efficient.  Also, some tricks are used in order to keep the results
// * numerically robust.  There are certainly improvements to be made, though.
// */
//double *kmeans( unsigned int n, double *a, unsigned int k )
//{
//  unsigned int i, j, *split, *prev, *perm, iteration;
//  double *mean, temp, mean_1, mean_2;
//  int step;
//
//  /**
//   * Allocate an array in order to hold the sorted permutation of the data
//   * without having to sort the actual data set.  Initialize with the
//   * identity permutation.
//   */
//  perm = ( unsigned int * ) malloc( sizeof( unsigned int ) * n );
//
//  for ( i = 0; i < n; i++ )
//    perm[i] = i;
//
//  /**
//   * Set a global pointer to the data
//   */
//  data = a;
//
//  /**
//   * Sort the data via the permutation
//   */
//  qsort( perm, n, sizeof( unsigned int ), my_compar );
//
//  /**
//   * Allocate space for the means
//   */
//  mean  = ( double * ) malloc( sizeof( double ) * k );
//  split = ( unsigned int * ) malloc( sizeof( unsigned int ) * ( k + 1 ));
//  prev  = ( unsigned int * ) malloc( sizeof( unsigned int ) * ( k + 1 ));
//
//  /**
//   * Initialize the split points
//   */
//  split[0] = prev[0] = 0;
//  split[k] = prev[k] = n;
//
//  for ( i = 1; i < k; i++ )
//  {
//    prev[i]  = 0;
//    split[i] = ( i * n ) / k;
//  }
//
//  /**
//   * Compute the initial mean sums
//   */
//  for ( i = 0; i < k; i++ )
//  {
//    mean[i] = 0.0;
//
//    for ( j = split[i]; j < split[i+1]; j++ )
//    {
//      mean[i] += data[perm[j]];
//    }
//  }
//
//  /**
//   * Loop until the intervals do not change
//   */
//  iteration = 1;
//
//  while ( changed( split, prev, k ))
//  {
//    /**
//     * Make the current partition the previous
//     */
//    swap( &prev, &split );
//
//    /**
//     * Find the new partitions
//     */
//    for ( i = 1; i < k; i++ )
//    {
//      mean_1   = mean[i-1] / ((double) ( prev[i]   - prev[i-1] ));
//      mean_2   = mean[i]   / ((double) ( prev[i+1] - prev[i]   ));
//
//      split[i] = find_boundary( perm, n, mean_1, mean_2 );
//    }
//
//    /**
//     * Update the mean sums.  Accumulate the difference in order to
//     * slightly improve the numerical robusteness.  It also saves some
//     * work.
//     */
//    for ( i = 1; i < k; i++ )
//    {
//      step = ( prev[i] < split[i] ) ?  1 : -1;
//      temp = 0.0;
//
//      for ( j = prev[i]; j != split[i]; j += step )
//      {
//        temp += data[perm[j]];
//      }
//
//      temp = ((double) step) * temp;
//
//      mean[i-1] += temp;
//      mean[i]   -= temp;
//    }
//
//    iteration++;
//  }
//
//  printf( "Converged in %d interations\n", iteration );
//
//  /**
//   * Divide the mean sums by the proper denominator
//   */
//  for ( i = 0; i < k; i++ )
//  {
//    mean[i] /= (double) (split[i+1] - split[i]);
//  }
//
//  free( prev );
//  free( split );
//  free( perm );
//
//  return mean;
//}


#endif // ONE_DIMENSIONAL_KMEANS_HEADER


