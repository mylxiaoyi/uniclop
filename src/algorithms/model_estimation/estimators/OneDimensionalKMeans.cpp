
// C++ implementation of  1-D kmeans
// an adaptation of the code wrote by Lucas Scharenbroich (2005)




/**
 * 1-D kmeans
 *
 * This is a very efficient version of kmeans that is specialized for
 * 1-dimensional data.  We can run very quickly by keeping only the
 * indices of class boundaries.  
 *
 * class 1 = 0
 * class 2 = n_1
 * class 3 = n_2
 * .
 * .
 * class k = n_k
 *
 * The data points which belong to a given class i are [n_(i-1) n_i-1].
 *
 * When the means are updated we can use a binary search to find the new
 * boundary.  By starting the search from the old boundary, we find the
 * new index *very* quickly.  Also, we need only update the means based on
 * the data points which change clusters.
 *
 * Overall, this algorithm runs in time ~O( i k log n )
 *
 * Copyright 2005
 * Lucas Scharenbroich
 */

#include "kmeans.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>
using namespace std;

// class OneDimensionalKMeans methods implementation

OneDimensionalKMeans::OneDimensionalKMeans()
{
    // initialize the pointers
    split_p = &partition1;
    prev_p  = &partition2;
    return;
}

OneDimensionalKMeans::~OneDimensionalKMeans()
{
    // nothing to do here
    return;
}
/**
 * This comparison function takes two integer values and compares the double
 * values stored in the array at the given indices.
 * This is Strict Weak Ordering is a Binary Predicate
 */
class my_compar : public binary_function<const unsigned int, const unsigned int, bool>
{
    const vector<double> &data;
public:
    my_compar(const vector<double> _data)
            : data(_data)
    {
        return;
    }

    bool operator()(const unsigned int i, const unsigned int j )
    {
        if ( data[i] < data[j] ) return true;
        else return false;
    }
};


/**
 * Finds the index of the datapoint in the array which is the midpoint
 * between the values of x and y.  This is done in logarithmic time using
 * a binary search.
 *
 * There are some subtle issues here.  First, when moving from a midpoint,
 * we need to keep the selected midpoint as the upper or lower bound, since
 * we might otherwise overstep the proper value.  At the end, we need to
 * check whether the upper or lower value is the best.
 */
unsigned int OneDimensionalKMeans::find_boundary(const vector<double> &data,
        const vector<unsigned int> &a,
        double x, double y )
{
    unsigned int lower, upper, mid;
    double w, z, lower_sum, upper_sum;

    lower = 0;
    upper = static_cast<unsigned int>(data.size() - 1);

    while ( lower < upper - 1 )
    {
        mid = ( upper + lower ) / 2;
        z   = data[a[mid]];

        if ( y - z <= z - x )
            upper = mid;
        else
            lower = mid;
    }

    /** Find the best one */
    w = data[a[lower]];
    z = data[a[upper]];

    lower_sum = (w - x) * (w - x) + (y - w) * (y - w);
    upper_sum = (z - x) * (z - x) + (y - z) * (y - z);

    return ( lower_sum < upper_sum ) ? lower : upper;
}


/**
 * Compare two partitionings of the data and return true if they differ
 * from each other or false if not.  This is the convergence criteria for
 * the kmeans clustering.
 */
bool OneDimensionalKMeans::changed( const vector<unsigned int> &a, const vector<unsigned int> &b)
{
    //vector<unsigned int>::const_iterator a_it, b_it;
    if(a.size() != b.size() )
        throw runtime_error("changed(...) arguments does not match expected sizes");

    unsigned int i;
    for ( i = 0; i < a.size(); i++ )
        if ( a[i] != b[i] )
            return true;

    return false;
}

/**
 * Perform k-means clustering on a one-dimensional data set.  Due to the
 * ordering imposed by scalar data, we can make the code much more
 * efficient.  Also, some tricks are used in order to keep the results
 * numerically robust.  There are certainly improvements to be made, though.
 */
void OneDimensionalKMeans::compute( vector<double> &data, const unsigned int k)
{
    unsigned int i, j, iteration;
    //vector<unsigned int> split, prev, perm; // class level variables
    //vector<double> mean; // class level variables
    double temp, mean_1, mean_2;
    int step;
    const unsigned int n = static_cast<unsigned int>(data.size());

    /**
     * Allocate an array in order to hold the sorted permutation of the data
     * without having to sort the actual data set.  Initialize with the
     * identity permutation.
     */

    perm.resize(n);

    for ( i = 0; i < n; i++ )
        perm[i] = i;




    /**
     * Sort the data via the permutation
     */
    sort(perm.begin(), perm.end(), my_compar(data) );

    /**
     * Allocate space for the means
     */
    mean.resize(k);
    partition1.resize(k+1);
    partition2.resize(k+1);

    /**
     * Initialize the split points
     */
    partition1[0] = partition2[0] = 0;
    partition1[k] = partition2[k] = n;

    for ( i = 1; i < k; i++ )
    {
        (*prev_p)[i]  = 0;
        (*split_p)[i] = ( i * n ) / k;
    }

    /**
     * Compute the initial mean sums
     */
    for ( i = 0; i < k; i++ )
    {
        mean[i] = 0.0;

        for ( j = (*split_p)[i]; j < (*split_p)[i+1]; j++ )
        {
            mean[i] += data[perm[j]];
        }
    }

    /**
     * Loop until the intervals do not change
     */
    iteration = 1;

    while ( changed( *split_p, *prev_p) )
    {
        /**
         * Make the current partition the previous
         */
        swap( split_p, prev_p);
        vector<unsigned int> &split = *split_p;
        vector<unsigned int> &prev = *prev_p;

        /**
         * Find the new partitions
         */
        for ( i = 1; i < k; i++ )
        {
            mean_1   = mean[i-1] / ((double) ( prev[i]   - prev[i-1] ));
            mean_2   = mean[i]   / ((double) ( prev[i+1] - prev[i]   ));

            split[i] = find_boundary(data, perm, mean_1, mean_2 );
        }

        /**
         * Update the mean sums.  Accumulate the difference in order to
         * slightly improve the numerical robusteness.  It also saves some
         * work.
         */
        for ( i = 1; i < k; i++ )
        {
            step = ( prev[i] < split[i] ) ?  1 : -1;
            temp = 0.0;

            for ( j = prev[i]; j != split[i]; j += step )
            {
                temp += data[perm[j]];
            }

            temp = ((double) step) * temp;

            mean[i-1] += temp;
            mean[i]   -= temp;
        }

        iteration++;
    } // end of 'while ( changed(prev, split, k) )'

    if(true) printf( "Converged in %d interations\n", iteration );

    /**
     * Divide the mean sums by the proper denominator
     */
    for ( i = 0; i < k; i++ )
    {
        mean[i] /= (double) ((*split_p)[i+1] - (*split_p)[i]);
    }

    return;
}

const vector<double> & OneDimensionalKMeans::get_kmeans()
{
    return mean;
}

const vector<unsigned int> &OneDimensionalKMeans::get_splits()
{
    return *split_p;
}

const vector<unsigned int> &OneDimensionalKMeans::get_permutations()
{
    return perm;
}

