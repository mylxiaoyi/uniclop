


/*
Simple application that shows on screen tracks of detected feature points
*/

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Headers

#include "FeaturesTrackingApplication.hpp"

#include "devices/video/GstVideoInput.hpp"

#include "algorithms/features/fast/FASTFeaturesMatcher.hpp"
#include "algorithms/features/SimpleFeaturesMatcher.hpp"

#include <CImg/CImg.h>

#include <boost/scoped_ptr.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "algorithms/model_estimation/model_estimation.hpp"


#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "helpers/rgb8_cimg_t.hpp"
#include "helpers/for_each.hpp"

namespace uniclop
{

using namespace std;
using namespace cimg_library;

using boost::gil::color_converted_view;
using boost::gil::copy_and_convert_pixels;
using boost::gil::const_view;
using boost::gil::gray8_image_t;


string FeaturesTrackingApplication::get_application_title() const
{
    return "Features tracking. A simple test application. Uniclop 2009";

}
args::options_description FeaturesTrackingApplication::get_command_line_options(void) const
{
    args::options_description desc;

    desc.add(GstVideoInput::get_options_description());
    desc.add(SimpleFAST::get_options_description());
    desc.add(FASTFeaturesMatcher::get_options_description());
    desc.add(SimpleFeaturesMatcher<features_type>::get_options_description());

    return desc;
}

int FeaturesTrackingApplication::main_loop(args::variables_map &options)
{

    printf("FeaturesTrackingApplication::main_loop says hello world !\n");

   // initialization ---
    gst_video_input_p.reset(new GstVideoInput(options));
	features_detector_p.reset(new SimpleFAST(options));

	features_matcher_p.reset(new  SimpleFeaturesMatcher<features_type>(options));
    //features_matcher_p.reset(new FASTFeaturesMatcher(options));

	// video output ---
    GstVideoInput::const_view_t new_image_view = gst_video_input_p->get_new_image();
    rgb8_cimg_t current_image(new_image_view.dimensions());
    current_image = new_image_view; // copy the data

    CImgDisplay video_display(current_image.dimx(), current_image.dimy(), get_application_title().c_str());
    video_display.show();
    video_display.display(current_image);

    // intermediary image --
    gray8_image_t gray_image(new_image_view.dimensions());
  
    // main loop ---
	vector<FASTFeature> previous_features; 

    do
    {
        // get new image --
        gst_video_input_p->get_new_image();
        current_image = new_image_view; // copy the data

        // color to gray_image
        copy_and_convert_pixels(current_image.view, boost::gil::view(gray_image));

        // compute features
        const vector<FASTFeature> &current_features =
                    features_detector_p->detect_features((const_view(gray_image)));
           
        // match with previous features 
        const  vector< ScoredMatch > &matches =
	        features_matcher_p->match(previous_features, current_features);
        
        previous_features = current_features; // copy for next iteration
        
        // update the tracks 
       	printf("Found %i matches\n", matches.size());
        // features_tracks.add_new_matches(matches);
        
        
        // plot the tracks on output image
       draw_tracks(features_tracks, current_image);

        video_display.display(current_image);

        // add a delay ---
        wait_some_seconds(0.1); // [seconds]
        
    }
    while (video_display.is_closed == false);

    return 0;
}


void FeaturesTrackingApplication::draw_tracks(const FeaturesTracks &tracks, rgb8_cimg_t &rgb_image_) {
	
	
	  printf("Currently managing %i tracks\n", tracks.size());

    const uint8_t point_color[3] = {255, 155, 0}; // red
   
    rgb8_cimg_t::cimg_t &rgb_image = * static_cast<rgb8_cimg_t::cimg_t *>(&rgb_image_);

    FeaturesTracks::const_iterator tracks_it;
    for_each(tracks_it, tracks)
    {
	    FeatureTrack::const_iterator track_items_it;
	    for_each(track_items_it, (*tracks_it))
	    {
		    // FIXME should draw connecting lines
	    	rgb_image.draw_point(track_items_it->x, track_items_it->y, point_color);
	    }
 
    }


    /*
       // draw results --
        if (show_matching_result)
        {

            // draw the two images --
            const int y_offset = current_image.dimy();
            {
                CImg<uint8_t> t_color_image(template_image.dimx(), template_image.dimy(), 1, 3);
                cimg_forXY(template_image, x,y)
                {
                    t_color_image(x,y, 0, 0) =  template_image(x,y);
                    t_color_image(x,y, 0, 1) =  template_image(x,y);
                    t_color_image(x,y, 0, 2) =  template_image(x,y);
                }
                matchings_image.draw_image(t_color_image, 0, 0);
            }

            {
                CImg<uint8_t> t_color_image(current_image.dimx(), current_image.dimy(), 1, 3);
                cimg_forXY(current_image, x,y)
                {
                    t_color_image(x,y, 0, 0) =  current_image(x,y);
                    t_color_image(x,y, 0, 1) =  current_image(x,y);
                    t_color_image(x,y, 0, 2) =  current_image(x,y);
                }
                matchings_image.draw_image(t_color_image, 0, y_offset);
            }



            // draw the matching lines --

            //const uint8_t outliers_color = 0; // black lines...
            //const uint8_t inliers_color = 255; // white lines...
            const uint8_t outliers_color[3] = {255, 155, 0}; // red lines...
            const uint8_t inliers_color[3] = {0, 155, 255}; // blue lines...


            vector< bool > dummy_is_inlier;
            const vector< bool >  *is_inlier_p = NULL;
            if (&estimator != NULL)
                is_inlier_p = &estimator.get_is_inlier();
            else
            {
                dummy_is_inlier.assign(matches.size(), false);
                // fill with false values
                is_inlier_p = &dummy_is_inlier;
            }

            const vector< bool >  &is_inlier = *is_inlier_p;
            vector< bool >::const_iterator is_inlier_it;

            // draw inliers and outliers -
            // first outliers
            typename vector< MatchType >::const_iterator matches_it;
            for (matches_it = matches.begin(), is_inlier_it = is_inlier.begin();
                    matches_it != matches.end() && is_inlier_it != is_inlier.end();
                    ++matches_it, ++is_inlier_it)
            {
                const int x0 = matches_it->feature_a->x;
                const int y0 = matches_it->feature_a->y;
                const int x1 = matches_it->feature_b->x;
                const int y1 = matches_it->feature_b->y + y_offset;

                if (*is_inlier_it)
                    continue; // skip inliers
                else
                    matchings_image.draw_line(x0, y0, x1, y1, outliers_color);
            }

            // then inliers
            for (matches_it = matches.begin(), is_inlier_it = is_inlier.begin();
                    matches_it != matches.end() && is_inlier_it != is_inlier.end();
                    ++matches_it, ++is_inlier_it)
            {
                const int x0 = matches_it->feature_a->x;
                const int y0 = matches_it->feature_a->y;
                const int x1 = matches_it->feature_b->x;
                const int y1 = matches_it->feature_b->y + y_offset;

                if (*is_inlier_it)
                    matchings_image.draw_line(x0, y0, x1, y1, inliers_color);
                else
                    continue; // skip outliers
            }

            if ( show_features_points )
            { // if required overlap the features points found
                typename vector<FeatureType>::const_iterator features_it;

                for (features_it = current_features.begin();
                        features_it != current_features.end();
                        ++features_it)
                {
                    //const uint8_t feature_color = 255; // white dots...
                    const uint8_t feature_color[3] = {255, 255, 255}; // white dots...
                    matchings_image.draw_point(features_it->x, features_it->y + y_offset, feature_color);
                }
            }

            // show the result
            matching_display.display(matchings_image);
        }
        */
        
	return;
}



// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

/*

template<typename F>
int main_loop(args::variables_map &options, IFeaturesDetector<F> &features_detector);
// part of the code needs to be templated...

int features_matching_main(int argc, char *argv[])
{
    // demonstration application search features of a template into a changing image
    try
    {
        cout << endl << PROGRAM_TITLE << endl << endl;

        // input arguments --
        args::options_description desc("Application options");

        desc.add_options()
        ("help", "produce help message")
        ("cimg_info", args::value< vector<string> >()->zero_tokens(),
         "print some informations about the CImg lbirary and exit")

        ("features_detection_method", args::value<string>()->default_value("FAST"),
         "choose the features detection method: FAST, Harris or SIFT")

        ("model", args::value<string>()->default_value("homography"),
         "choose the model to use: homography or fundamental_matrix")

        ("estimation_method", args::value<string>()->default_value("none"),
         "choose the robust estimation method: none, RANSAC, PROSAC, Ensemble or DenseEnsemble")

        ("show_features_points", args::value<bool>()->default_value(false),
         "show the detected features")

        ("show_matching_result", args::value<bool>()->default_value(false),
         "show the matching result (inliers and outliers matches)")
        ;

        desc.add( ImagesInput<uint8_t>::get_options_description() );
        desc.add( SimpleFAST::get_options_description() );
        desc.add( SimpleSIFT::get_options_description() );
        desc.add( FASTFeaturesMatcher::get_options_description() );
        desc.add( SimpleFeaturesMatcher<FASTFeature>::get_options_description() );
        desc.add( EnsembleMethod< ScoredMatch<FASTFeature> >::get_options_description() );
        desc.add( PROSAC< ScoredMatch<FASTFeature> >::get_options_description() );
        desc.add( RANSAC< ScoredMatch<FASTFeature> >::get_options_description() );

        args::variables_map options;
        args::store(args::parse_command_line(argc, argv, desc), options);
        args::notify(options);

        if ( options.count("help") )
        {
            cout << desc << endl;
            return 0;
        }

        if ( options.count("cimg_info") )
        {
            cimg::info(); // print some information about the environment configuration
            return 0;
        }



        // select the features detection method ---
        // the features type will determinate the features type required
        // to specialize the rest of the code

        string features_detection_method = "none";
        if (options.count("features_detection_method"))
            features_detection_method = options["features_detection_method"].as<string>();

        if (features_detection_method == "FAST")
        {
            boost::scoped_ptr< IFeaturesDetector<SimpleFAST::features_type> > features_detector_p;
            features_detector_p.reset( new SimpleFAST(options) );
            return main_loop(options, *features_detector_p);
        }
        else if (features_detection_method == "Harris")
        {
            throw runtime_error("Harris features detector not yet implemented");
        }
        else if (features_detection_method == "SIFT")
        {
            boost::scoped_ptr< IFeaturesDetector<SimpleSIFT::features_type> > features_detector_p;
            features_detector_p.reset( new SimpleSIFT(options) );
            return main_loop(options, *features_detector_p);
        }
        else
        {
            throw runtime_error("No known features detector selected");
        }

    } // end of initial 'try'
    catch (exception &e)
    {
        cout << "An exception was raised: " << e.what() << endl;
    }

    return 0;
}

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

template<typename FeatureType>
int main_loop(args::variables_map &options, IFeaturesDetector<FeatureType> &features_detector)
{
    // part of the code needs to be templated...

    // get display options --
    bool show_features_points = false;
    if ( options.count( "show_features_points" ) )
        show_features_points = options["show_features_points"].as<bool>();

    bool show_matching_result = false;
    if ( options.count( "show_matching_result" ) )
        show_matching_result = options["show_matching_result"].as<bool>();


    // get template image --
    ImagesInput<uint8_t> images_input(options);
    const CImg<uint8_t> &current_image = images_input.get_new_image();

    const CImg<uint8_t> template_image( current_image ); // copy

    // obtain features to match with...
    const vector<FeatureType> template_features =
        features_detector.detect_features(template_image); // copy the results

    if (true)
    {
        cout << "template_features.size() == " << template_features.size() << endl;
    }

    // create the features matcher, and model estimator objects --

    SimpleFeaturesMatcher<FeatureType> simple_features_matcher(options);

    string estimation_method;
    if (options.count("estimation_method"))
        estimation_method = options["estimation_method"].as<string>();

    //FundamentalMatrixModel fundamental_matrix_model(options);
    typedef ScoredMatch<FeatureType> MatchType;

    FundamentalMatrixModel< FeatureType > fundamental_matrix_model;
    HomographyModel<FeatureType> homography_model;

    IParametricModel< MatchType > *model_p = NULL;
    string model_name;
    if (options.count("model"))
        model_name = options["model"].as<string>();

    if (model_name == "homography")
        model_p = &homography_model;
    else if (model_name == "fundamental_matrix")
        model_p = &fundamental_matrix_model;

    if (model_p == NULL)
        throw runtime_error("No model was specified");

    IParametricModel< MatchType > &model = *model_p;

    boost::scoped_ptr< IModelEstimator< MatchType > > estimator_p;

    if (estimation_method == "RANSAC")
        estimator_p.reset( new RANSAC< MatchType >(options, model) );

    else if (estimation_method == "PROSAC")
        estimator_p.reset( new PROSAC< MatchType >(options, model) );

    else if (estimation_method == "Ensemble")
    {
        EnsembleMethod< MatchType > *t_ensemble_method_p = new EnsembleMethod< MatchType >(options, model);

        // Homography and FundamentalMatrix models compute the residuals
        // as distances in pixels
        const double min_error_value  = 0.1; // smaller than a pixel of distance
        const double max_error_value  = max(current_image.dimx(), current_image.dimy()) / 1; // 2;
        t_ensemble_method_p->set_min_max_values(min_error_value, max_error_value);

        estimator_p.reset(  t_ensemble_method_p ); // the smart pointer is in charge of deallication
    }
    else if (estimation_method == "DenseEnsemble")
        estimator_p.reset( new DenseEnsembleMethod< MatchType >(options, model) );

    if ( estimator_p == NULL && estimation_method != "none")
        throw runtime_error("No valid estimation method available");

    IModelEstimator< MatchType > &estimator = *estimator_p;
    // create the output displays --

    CImgDisplay video_display(current_image.dimx(), current_image.dimy(), "Video stream");
    video_display.show();
    video_display.display(current_image);

    CImg<uint8_t> matchings_image(current_image.dimx(), 2*current_image.dimy(), 1, 3);
    CImgDisplay matching_display(matchings_image.dimx(), matchings_image.dimy(), "Matching results");
    matching_display.close();
    if (show_matching_result)
    {
        matching_display.show();
        video_display.close(); // not used
    }

    // video input, main loop --
    do
    {
        // retrieve new image -
        CImg<uint8_t> &current_image = images_input.get_new_image();

        // compute features -
        const vector<FeatureType> &current_features = features_detector.detect_features(current_image);

        // obtain features matches candidates -
        vector< MatchType > & matches =
            simple_features_matcher.match(template_features, current_features);

        sort(matches.begin(), matches.end());
        // introsort C++ standard algorithm O(NlogN)

        // estimate model parameters -
        //const ublas::vector<double> & parameters =
        if (&estimator != NULL)
        {
            { // DenseEnsembleMethod special code

                DenseEnsembleMethod<MatchType> * dense_ensemble_method_estimator_p = NULL;
                dense_ensemble_method_estimator_p = dynamic_cast< DenseEnsembleMethod<MatchType> * >(&estimator);
                if ( dense_ensemble_method_estimator_p != NULL)
                {
                    float blur_sigma = -1.0f; // negative value indicates no blurring
                    if (options.count("blur_sigma"))
                    {

                        blur_sigma = options["blur_sigma"].as<double>();
                        cout << "Blurring the images for dense estimation with sigma value "
                             << blur_sigma << endl;
                    }
                    dense_ensemble_method_estimator_p->set_images_pair(template_image, current_image, blur_sigma);
                }
            }

            // estimate the parameters
            const ublas::vector<float> &p = estimator.estimate_model_parameters(matches);
            if (false)
            {
                cout << "estimator.estimate_model_parameters(matches) == " << p << endl;
            }

        }
        // draw results --
        if (show_matching_result)
        {

            // draw the two images --
            const int y_offset = current_image.dimy();
            {
                CImg<uint8_t> t_color_image(template_image.dimx(), template_image.dimy(), 1, 3);
                cimg_forXY(template_image, x,y)
                {
                    t_color_image(x,y, 0, 0) =  template_image(x,y);
                    t_color_image(x,y, 0, 1) =  template_image(x,y);
                    t_color_image(x,y, 0, 2) =  template_image(x,y);
                }
                matchings_image.draw_image(t_color_image, 0, 0);
            }

            {
                CImg<uint8_t> t_color_image(current_image.dimx(), current_image.dimy(), 1, 3);
                cimg_forXY(current_image, x,y)
                {
                    t_color_image(x,y, 0, 0) =  current_image(x,y);
                    t_color_image(x,y, 0, 1) =  current_image(x,y);
                    t_color_image(x,y, 0, 2) =  current_image(x,y);
                }
                matchings_image.draw_image(t_color_image, 0, y_offset);
            }



            // draw the matching lines --

            //const uint8_t outliers_color = 0; // black lines...
            //const uint8_t inliers_color = 255; // white lines...
            const uint8_t outliers_color[3] = {255, 155, 0}; // red lines...
            const uint8_t inliers_color[3] = {0, 155, 255}; // blue lines...


            vector< bool > dummy_is_inlier;
            const vector< bool >  *is_inlier_p = NULL;
            if (&estimator != NULL)
                is_inlier_p = &estimator.get_is_inlier();
            else
            {
                dummy_is_inlier.assign(matches.size(), false);
                // fill with false values
                is_inlier_p = &dummy_is_inlier;
            }

            const vector< bool >  &is_inlier = *is_inlier_p;
            vector< bool >::const_iterator is_inlier_it;

            // draw inliers and outliers -
            // first outliers
            typename vector< MatchType >::const_iterator matches_it;
            for (matches_it = matches.begin(), is_inlier_it = is_inlier.begin();
                    matches_it != matches.end() && is_inlier_it != is_inlier.end();
                    ++matches_it, ++is_inlier_it)
            {
                const int x0 = matches_it->feature_a->x;
                const int y0 = matches_it->feature_a->y;
                const int x1 = matches_it->feature_b->x;
                const int y1 = matches_it->feature_b->y + y_offset;

                if (*is_inlier_it)
                    continue; // skip inliers
                else
                    matchings_image.draw_line(x0, y0, x1, y1, outliers_color);
            }

            // then inliers
            for (matches_it = matches.begin(), is_inlier_it = is_inlier.begin();
                    matches_it != matches.end() && is_inlier_it != is_inlier.end();
                    ++matches_it, ++is_inlier_it)
            {
                const int x0 = matches_it->feature_a->x;
                const int y0 = matches_it->feature_a->y;
                const int x1 = matches_it->feature_b->x;
                const int y1 = matches_it->feature_b->y + y_offset;

                if (*is_inlier_it)
                    matchings_image.draw_line(x0, y0, x1, y1, inliers_color);
                else
                    continue; // skip outliers
            }

            if ( show_features_points )
            { // if required overlap the features points found
                typename vector<FeatureType>::const_iterator features_it;

                for (features_it = current_features.begin();
                        features_it != current_features.end();
                        ++features_it)
                {
                    //const uint8_t feature_color = 255; // white dots...
                    const uint8_t feature_color[3] = {255, 255, 255}; // white dots...
                    matchings_image.draw_point(features_it->x, features_it->y + y_offset, feature_color);
                }
            }

            // show the result
            matching_display.display(matchings_image);
        }
        else if ( show_features_points )
        {
            typename vector<FeatureType>::const_iterator features_it;

            for (features_it = current_features.begin();
                    features_it != current_features.end();
                    ++features_it)
            {
                const uint8_t feature_color = 255; // white dots...
                current_image.draw_point(features_it->x, features_it->y, &feature_color);
            }

            video_display.display(current_image); // show the image with dots !
        }
        else
        {
            // nothing special to show
            video_display.display(current_image);
        }

        // estimated models
        // inlier and outlier matches

    }
    while ( (images_input.reached_last_image() == false)
            && (video_display.is_closed == false || matching_display.is_closed == false));

    if (estimation_method != "Ensemble")
    {
        // show the last frame obtained
        while ( video_display.is_closed == false || matching_display.is_closed == false )
        {
            CImgDisplay::wait(video_display, matching_display);
        }
    }
    else
    { // show the last frame AND the kurtosis estimates sources

        EnsembleMethod<MatchType> * ensemble_method_estimator_p = NULL;
        ensemble_method_estimator_p = dynamic_cast< EnsembleMethod<MatchType> * >(&estimator);
        if ( ensemble_method_estimator_p == NULL)
            throw runtime_error("Estimation method does not compute histograms");

        const vector< HistogramKurtosis<double> > &hk_estimators =
            ensemble_method_estimator_p->histogram_kurtosis_estimators;

        const vector<double> &kurtosis_values =
            ensemble_method_estimator_p->kurtosis_values;

        if ( hk_estimators.empty() == false )
        {


            unsigned int h_index = 0; // index to one of the histograms
            int graphs_max_val = 0;

            const vector<int> &t_bins = hk_estimators[h_index].get_bins();
            graphs_max_val = max(*max_element(t_bins.begin(), t_bins.end()), graphs_max_val);
            const float t_kurtosis = static_cast<float>(kurtosis_values[h_index]);

            CImgDisplay histograms_display;
            HistogramKurtosis<>::display_histogram(t_kurtosis,
                                                   t_bins, graphs_max_val, histograms_display);
            histograms_display.set_title("Generated histogram");
            histograms_display.show();



            // show the last frame obtained
            while ( video_display.is_closed == false
                    || matching_display.is_closed == false
                    || histograms_display.is_closed == false )
            {

                if (histograms_display.button)
                {
                    h_index += 1;
                    if (h_index == hk_estimators.size() ) h_index = 0;

                    const vector<int> &t_bins = hk_estimators[h_index].get_bins();
                    graphs_max_val = max(*max_element(t_bins.begin(), t_bins.end()), graphs_max_val);
                    const float t_kurtosis = static_cast<float>(kurtosis_values[h_index]);

                    HistogramKurtosis<>::display_histogram(t_kurtosis,
                                                           t_bins, graphs_max_val, histograms_display);

                } // end of 'if histograms_display.button'


                CImgDisplay::wait(video_display, matching_display, histograms_display);
            } // end of 'while windows are open'

        }
        else
        { // hk_estimators.empty() == true

            //throw runtime_error("No histograms to show");
            cout << "No histograms to show" << endl;
        }

    } // end of 'if (estimation_method != "Ensemble") '

    return 0;
}
*/

} // end of namespace uniclop





