#include "FeaturesDetectionApplication.hpp"

#include "devices/video/GstVideoInput.hpp"

#include "algorithms/features/features_detection.hpp"


#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>


#include <cstdio>
#include <iostream>
#include <stdexcept>

#include <CImg/CImg.h>
#include "helpers/rgb8_cimg_t.hpp"
#include "helpers/for_each.hpp"

namespace uniclop
{
namespace applications
{
namespace features_detection
{

using namespace uniclop::devices::video;
using namespace cimg_library;
using boost::gil::color_converted_view;
using boost::gil::copy_and_convert_pixels;
using boost::gil::const_view;
using boost::gil::gray8_image_t;

using uniclop::algorithms::features::FASTFeature;


string FeaturesDetectionApplication::get_application_title() const
{

    return "Features detection input. A simple test application. Uniclop 2009";
}

args::options_description FeaturesDetectionApplication::get_command_line_options(void) const
{

    program_options::options_description desc;

    desc.add(GstVideoInput::get_options_description());
    desc.add(SimpleFAST::get_options_description());

    return desc;
}


int FeaturesDetectionApplication::main_loop(program_options::variables_map &options)
{
    printf("FeaturesDetectionApplication::main_loop says hello world !\n");


    //init_gui(options);
    //run_gui();

// initialization ---
    gst_video_input_p.reset(new GstVideoInput(options));
    features_detector_p.reset(new SimpleFAST(options));



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

    do
    {
        // get new image --
        gst_video_input_p->get_new_image();
        current_image = new_image_view; // copy the data

        // color to gray_image
        copy_and_convert_pixels(current_image.view, boost::gil::view(gray_image));

        // compute features
        const vector<FASTFeature> &features =
            features_detector_p->detect_features((const_view(gray_image)));

        // plot features on output image
        draw_features(features, current_image);

        video_display.display(current_image);

        // add a delay ---
        const float seconds_to_wait = 0.1; // [seconds]
        boost::xtime t_time;
        boost::xtime_get(&t_time, boost::TIME_UTC);
        t_time.nsec += (1000 * 1000) * seconds_to_wait;
        boost::thread::sleep(t_time);

    }
    while (video_display.is_closed == false);

    return 0;

}

void FeaturesDetectionApplication::draw_features(const vector<FASTFeature> &features, rgb8_cimg_t &rgb_image)
{

    printf("Found %i FASTFeatures\n", features.size());

    const uint8_t point_color[3] = {255, 155, 0}; // red

    vector<FASTFeature>::const_iterator features_it;
    for_each(features_it, features)
    {

        static_cast<rgb8_cimg_t::cimg_t>(rgb_image).draw_point(features_it->x, features_it->y, point_color);

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

}
}
}
