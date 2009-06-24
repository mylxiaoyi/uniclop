
#include "SimpleFAST.hpp"


#include "FASTFeature.hpp"

#include "fast.hpp"


namespace uniclop
{


// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// class SimpleFAST methods implementation

args::options_description SimpleFAST::get_options_description()
{

    args::options_description desc("SimpleFAST options");
    desc.add_options()

    ( "fast.barrier", args::value<int>()->default_value(20),
      "threshold used to detect FAST features")
    ;

    return desc;
}

SimpleFAST::SimpleFAST(args::variables_map &options)
{
    barrier = 20; // default value

    if ( options.count("fast.barrier") )
        barrier = options["fast.barrier"].as<int>();

    return;
}

SimpleFAST::~SimpleFAST()
{
    return;
}

const vector<FASTFeature> & SimpleFAST::detect_features(const gray8c_view_t& view)
{
    // no need to clear the vectors features it is done inside the functions

    // find corners
    fast::corner_detect(view, barrier, detected_features);

    // keep the best ones
    fast::nonmax(view, barrier, detected_features, best_features);

    return best_features;
}



}