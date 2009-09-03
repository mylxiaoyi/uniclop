

#define Program_title "Precompute disparities. R.Benenson. 2007-2008."

// Headers ---

// standard C++
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <algorithm>
#include <utility> // defines class pair
//#include <hash_set>
#include <set>
#include<complex>
using namespace std;

#if defined(_MSC_VER)
#define cimg_imagemagick_path "C:\\ImageMagick-6.3.5-Q16\\convert.exe"
#endif

#include <CImg/CImg.h>
using namespace cimg_library;


#include <boost/program_options.hpp>
namespace args = boost::program_options;

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/scoped_ptr.hpp>

// Utility macros --
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

#ifndef foreach
#define foreach(the_iterator, the_list)\
for (the_iterator = the_list.begin(); the_iterator != the_list.end(); ++the_iterator)
#endif

#include "openvis3d/Openvis3d.hpp"
#include "openvis3d/CImgAdapter.hpp"



using namespace Openvis3d;

// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~

template<typename T>
void testStereo( 
		 const CImg<T> &left_image, 
		 const CImg<T> &right_image,
		 const double minshift, const double maxshift)
{
  // read input images ---

  // show the input images while computation takes place
  CImgDisplay left_image_display(left_image, "left image");
  CImgDisplay right_image_display(right_image, "right image");
  
  // create output images --
  
  const int 
    dimx = left_image.dimx(), 
    dimy = left_image.dimy();
  //const int dimz = left_image.dimz(), dimv = left_image.dimv();
  
  if(  left_image.is_sameXYZV(right_image) == false )
    throw runtime_error("Dimensions of left and right images does not match");

  CImg<double> 
    left_disparity(dimx, dimy), left_occlusion(dimx, dimy),
    right_disparity(dimx, dimy), right_occlusion(dimx, dimy);
  
  // wrap all the input and output images in OpenCVImageAdapter, 
  // so that they can be accessed by OpenVis3D
  // ova stands for "OpenVisAdapater"
    
  CImgAdapter< T > 
    ova_left_image(left_image), ova_right_image(right_image);

  CImgAdapter<double> 
    ova_left_disparity(left_disparity), ova_right_disparity(right_disparity),
    ova_left_occlusion(left_occlusion), ova_right_occlusion(right_occlusion);

  // create Birchfield-Tomasi local matcher and set its default parameter alpha to 20.0
  BTLocalMatcherT<double> btmatcher;
  double alpha[] = {20.0};
  btmatcher.setParams(1,alpha);

  // create global diffusion-based stereo algorithm instance
  StereoDiffuseMatcherT<double> stereoDiffuseMatcher;

  // create general stereo algorithm execution manager instance
  StereoT<double> stereoManager;
  stereoManager.setLocalImageMatcher(btmatcher); //set local matcher to Birchfield-Tomasi
  stereoManager.setGlobalMatcher(stereoDiffuseMatcher); //set global stereo algorithm

    
  // execute stereo matching ---
  printf("\nRunning stereo ...\n");
    
  stereoManager.doStereoMatching(
				 ova_left_image, ova_right_image,
				 minshift, maxshift,
				 ova_left_disparity, ova_right_disparity,
				 ova_left_occlusion, ova_right_occlusion);


  // display results ---	
  CImg<double> left_result(dimx, dimy, 1, 3), right_result(dimx, dimy, 1, 3); 
  // fill with HSV 
  const int S = 100;
  const int V = 100;
  const int occluted_value = 0;
  
  // H is an angle in degrees, S and V are in [0, 100]
  const int max_H = 250; // H in [0, 360], 0 is red, 250 is near blue
  
  left_disparity *= -1; // left and right disparities are inversed, this compensates
  left_disparity.normalize(max_H,  0); // low disparity, far distance (blue) 
  right_disparity.normalize(max_H, 0); // hi disparity, near distance (red)
 
  cimg_forXY(left_result, x,y)
  {
  	if(left_occlusion(x,y) == 0.0)
  	{
  		const double disparity = left_disparity(x,y);
  		left_result(x,y,0) = disparity;
  		left_result(x,y,1) = S;
  		left_result(x,y,2) = V;
  	}
  	else
  	{   // HSV
  		left_result(x,y,0) = 180; 
  		left_result(x,y,1) = V;
  		left_result(x,y,2) = occluted_value;
  	}
  }
  
    cimg_forXY(right_result, x,y)
  {

  	if(right_occlusion(x,y) == 0.0)
  	{
  		const double disparity = right_disparity(x,y);
  		right_result(x,y,0) = disparity;
  		right_result(x,y,1) = S;
  		right_result(x,y,2) = V;
  	}
  	else
  	{   // HSV
  		right_result(x,y,0) = 180; 
  		right_result(x,y,1) = V;
  		right_result(x,y,2) = occluted_value;
  	}
  }
  
  left_result.HSVtoRGB(); // for display
  right_result.HSVtoRGB(); // for display
  
  
  if(false)
 {
	  CImgDisplay right_occlusion_display(right_occlusion, "right occlusion");
	  CImgDisplay left_occlusion_display(left_occlusion, "left occlusion");
	  
	  CImgDisplay right_disparity_display(right_disparity, "right disparity");
	  CImgDisplay left_disparity_display(left_disparity, "left disparity");
		
	  left_image_display.show(); right_image_display.show();
	  left_occlusion_display.show(); right_occlusion_display.show();
	  left_disparity_display.show(); right_disparity_display.show();
	  
	    // keep the results open
  		cout << "Done, close any window to continue..." << endl;
        
  while( 	   !left_image_display.is_closed
		   && !right_image_display.is_closed
		   && !left_disparity_display.is_closed
		   && !right_disparity_display.is_closed
		   && !left_occlusion_display.is_closed
		   && !right_occlusion_display.is_closed )
    {
      CImgDisplay::wait_all();
    }
    
 }
 else
 {
 	 CImgDisplay righ_result_display(right_result, "right disparity and occlusion");
	 CImgDisplay left_result_display(left_result, "left disparity and occlusion");
	  		
	  left_image_display.show(); right_image_display.show();
	  left_result_display.show(); righ_result_display.show();
	  
	    // keep the results open
  		cout << "Done, close any window to continue..." << endl;
        
  while( 	   !left_image_display.is_closed
		   && !right_image_display.is_closed
		   && !left_result_display.is_closed
		   && !righ_result_display.is_closed)
    {
      CImgDisplay::wait_all();
    }
    
 }  		



  return;
} // end of function testStereo


// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~

template<typename ImageType> 
class IStereoImagesIterator
{
	public:
		// methods to be implemented by childs
		
		virtual const IStereoImagesIterator<ImageType> & operator++() = 0;
		///< prefix operator
		
		virtual const ImageType& right_image() const = 0;
		virtual const ImageType& left_image()  const = 0;
		
	public:
		// base methods
		const IStereoImagesIterator<ImageType> *operator->() const
		{
			return this;
		}
	
		IStereoImagesIterator()
		{ return; }
			
		virtual ~IStereoImagesIterator()
		{ return; }
};

// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
 using namespace boost::filesystem;

class LeftRightDirectoriesIterator: public IStereoImagesIterator< CImg<uint8_t> >
{
  
      
	public:
		typedef CImg<uint8_t> image_type;
	
		LeftRightDirectoriesIterator();
	
		LeftRightDirectoriesIterator(
			const directory_iterator &_left_itr, 
			const directory_iterator &_right_itr);
			
		LeftRightDirectoriesIterator(const LeftRightDirectoriesIterator& it);
			
		~LeftRightDirectoriesIterator();
		
		///@name IStereoImagesIterator interface implementation		
		///{
		bool operator==(const LeftRightDirectoriesIterator &it);
		bool operator!=(const LeftRightDirectoriesIterator &it);
		
		const IStereoImagesIterator<image_type> & operator++();
			
		const image_type& left_image() const;
		const image_type& right_image() const;
		///}
		
	private:
		void read_first_images();
		void read_images();
	
		directory_iterator left_itr, right_itr, end_itr;
		image_type _left_image, _right_image;
		bool at_end;
	
};

void LeftRightDirectoriesIterator::read_first_images()
{
	// search for the first valid files ---
	bool valid_files = true;
	do
	{
		valid_files = true;
		if(at_end) return; // reached the end, nothing to do
		
		if( is_directory( *left_itr ) || is_directory( *right_itr ) )
		{
			cout << "Omiting subdirectories " 
				 <<  left_itr->string() << " and " 
				 << right_itr->string() << endl;
			valid_files = false;
		}	
		else
			break;		
		
		++left_itr; ++right_itr;
		at_end = (left_itr == end_itr) || (right_itr == end_itr);
		
	}while(valid_files == false);
	
	// got valid files, open the images --
	read_images();
	return;
}

LeftRightDirectoriesIterator::LeftRightDirectoriesIterator()
{
	at_end = (left_itr == end_itr) || (right_itr == end_itr);
	assert(at_end == true);
	return;
}

LeftRightDirectoriesIterator::LeftRightDirectoriesIterator(
	const directory_iterator &_left_itr, 
	const directory_iterator &_right_itr)
{
	left_itr = _left_itr;
	right_itr = _right_itr;
	at_end = (left_itr == end_itr) || (right_itr == end_itr);
	
		
	read_first_images();
	return;
}
			
LeftRightDirectoriesIterator::LeftRightDirectoriesIterator(
		const LeftRightDirectoriesIterator& it)
		: 	IStereoImagesIterator<image_type>()
{

	left_itr = it.left_itr;
	right_itr = it.right_itr;
	
	at_end = (left_itr == end_itr) || (right_itr == end_itr);
	
	read_first_images();

	return;
}
		
		
LeftRightDirectoriesIterator::~LeftRightDirectoriesIterator()
{
	// nothing to do here
	return;
}

bool LeftRightDirectoriesIterator::operator==(const LeftRightDirectoriesIterator &it)
{
	if(it.at_end && this->at_end)
		return true;
	else
		return (it.left_itr == left_itr) && (it.right_itr == right_itr);
}


bool LeftRightDirectoriesIterator::operator!=(const LeftRightDirectoriesIterator &it)
{
	if(it.at_end && this->at_end)
		return false;
	else
		return (it.left_itr != left_itr) || (it.right_itr != right_itr);
}

void LeftRightDirectoriesIterator::read_images()
{
		if(true)
		{
			cout << "Reading "  << "\t" 
				 << left_itr->string() << "\t" 
				 << right_itr->string() << endl;
		}
		_left_image.assign(left_itr->string().c_str());
		_right_image.assign(right_itr->string().c_str());
			
	return;
}

const IStereoImagesIterator<LeftRightDirectoriesIterator::image_type> & 
LeftRightDirectoriesIterator::operator++()
{
	
	if(at_end)
		throw std::runtime_error(
			"LeftRightDirectoriesIterator::operator++ called over "\
			"an iterator already at the end of one of the directories");
	else
	{
		bool valid_files = true;
		do
		{
			valid_files = true;
			++left_itr; ++right_itr;
			at_end = (left_itr == end_itr) || (right_itr == end_itr);
			if(at_end) return *this; // reached the end, nothing to do
			
			if( is_directory( *left_itr ) || is_directory( *right_itr ) )
	  		{
				cout << "Omiting subdirectories " 
					 <<  left_itr->string() << " and " 
					 << right_itr->string() << endl;
				valid_files = false;
	  		}			
		}while(valid_files == false);
		
		// got valid files, open the images --
		read_images();
	}
	return *this;
}

	
const LeftRightDirectoriesIterator::image_type & 
LeftRightDirectoriesIterator::left_image() const

{
	return _left_image;
}

const LeftRightDirectoriesIterator::image_type & 
LeftRightDirectoriesIterator::right_image() const
{
	return _right_image;
}

    
// Iterable left right directories. 
// IStereoImagesIterator provider
class LeftRightDirectories
{
	
	 directory_iterator left_itr, right_itr;
	 boost::scoped_ptr<LeftRightDirectoriesIterator> 
	 	begin_iterator_p, end_iterator_p;
	public:
		LeftRightDirectories(const string &directory_name);
		~LeftRightDirectories();
	
		typedef LeftRightDirectoriesIterator  iterator;
		typedef LeftRightDirectoriesIterator const_iterator;
		// <<< how to define properly a const_iterator ? and non const_one ?
	
		const_iterator &begin();
		const_iterator &end();
	
};

LeftRightDirectories::LeftRightDirectories(const string &input_directory_name)
{

 	path input_images_path(input_directory_name);

      if( !exists(input_images_path))
	throw runtime_error("Indicated input images path does not exist");

      if( !is_directory(input_images_path) )
	throw runtime_error("Indicated input images path is not a directory");

      path left_images_path( input_images_path / "left" );
      path right_images_path( input_images_path / "right" );

      if( !exists(left_images_path) 
	  || !exists(right_images_path) 
	  || !is_directory(left_images_path) 
	  || !is_directory(right_images_path) )
	throw runtime_error("Did not found the 'left' and 'right' input subdirectories");
	
	directory_iterator left_itr( left_images_path ), right_itr(right_images_path);
	begin_iterator_p.reset(new LeftRightDirectoriesIterator(left_itr, right_itr));
		
	directory_iterator end_itr; // default construction yields past-the-end
	end_iterator_p.reset(new LeftRightDirectoriesIterator(end_itr, end_itr));
		 
	return;
}

LeftRightDirectories::~LeftRightDirectories()
{
	// nothing to do here
	return;
}

LeftRightDirectories::const_iterator &LeftRightDirectories::begin()
{
	return *begin_iterator_p;
};

LeftRightDirectories::const_iterator &LeftRightDirectories::end()
{
	return *end_iterator_p;
};

// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~

class SplitImagesDirectoryIterator: public IStereoImagesIterator< CImg<uint8_t> >
{
  
      
	public:
		typedef CImg<uint8_t> image_type;
	
		SplitImagesDirectoryIterator();
	
		SplitImagesDirectoryIterator(const directory_iterator &_directory_itr);
			
		SplitImagesDirectoryIterator(const SplitImagesDirectoryIterator& it);
			
		~SplitImagesDirectoryIterator();
		
		///@name IStereoImagesIterator interface implementation		
		///{
		bool operator==(const SplitImagesDirectoryIterator &it);
		bool operator!=(const SplitImagesDirectoryIterator &it);
		
		const IStereoImagesIterator<image_type> & operator++();
			
		const image_type& left_image()  const;
		const image_type& right_image() const;
		///}
		
	private:
		void read_first_images();
		void read_images();
		directory_iterator directory_itr, end_itr;
		image_type original_image, _left_image, _right_image;
};

void SplitImagesDirectoryIterator::read_first_images()
{
	
		bool valid_files = true;
		do
		{
			valid_files = true;
			if(directory_itr == end_itr) return; // reached the end, nothing to do
			
			if( is_directory( *directory_itr ) )
	  		{
				cout << "Omiting subdirectory " 
					 <<  directory_itr->string() << endl;
				valid_files = false;
	  		}
			else
				break;
						
	  		++directory_itr;
		}while(valid_files == false);
		
		// got valid files, open the images --
		read_images();
		
	return;
}

SplitImagesDirectoryIterator::SplitImagesDirectoryIterator()
{

	return;
}

SplitImagesDirectoryIterator::SplitImagesDirectoryIterator(
	const directory_iterator &_directory_itr)
	: directory_itr(_directory_itr)
{
	read_first_images();
	return;
}
			
SplitImagesDirectoryIterator::SplitImagesDirectoryIterator(
		const SplitImagesDirectoryIterator& it)
		: 	IStereoImagesIterator<image_type>()
{

	directory_itr = it.directory_itr;
	read_first_images();
	return;
}
		
		
SplitImagesDirectoryIterator::~SplitImagesDirectoryIterator()
{
	// nothing to do here
	return;
}

bool SplitImagesDirectoryIterator::operator==(const SplitImagesDirectoryIterator &it)
{
	return (directory_itr == it.directory_itr);
}


bool SplitImagesDirectoryIterator::operator!=(const SplitImagesDirectoryIterator &it)
{
	return (directory_itr != it.directory_itr);
}

void SplitImagesDirectoryIterator::read_images()
{
	if(true)
	{
		cout << "Reading "  << "\t" 
			 << directory_itr->string() << endl;
	}
	
	original_image.assign(directory_itr->string().c_str());
	
	int dimx = original_image.dimx();
	const int dimy = original_image.dimy();
	if((dimx % 2) == 1) dimx -= 1; // ensures dimx to be an even number
			  
	_left_image = original_image.get_crop(0,0, dimx/2,dimy); // left side
	_right_image = original_image.get_crop(dimx/2,0, dimx,dimy); // right side
	
	return;
}


const IStereoImagesIterator<SplitImagesDirectoryIterator::image_type> & 
SplitImagesDirectoryIterator::operator++()
{
	
	if(directory_itr == end_itr)
		throw std::runtime_error(
			"LeftRightDirectoriesIterator::operator++ called over "\
			"an iterator already at the end of one of the directories");
	else
	{
		bool valid_files = true;
		do
		{
			valid_files = true;
			++directory_itr;
			if(directory_itr == end_itr) return *this;
			
			if( is_directory( *directory_itr ) )
	  		{
				cout << "Omiting subdirectory " 
					 <<  directory_itr->string() << endl;
				valid_files = false;
	  		}			
		}while(valid_files == false);
		
		// got valid files, open the images --
		read_images();
	}
	return *this;
}

	
const SplitImagesDirectoryIterator::image_type & 
SplitImagesDirectoryIterator::left_image() const

{
	return _left_image;
}

const SplitImagesDirectoryIterator::image_type & 
SplitImagesDirectoryIterator::right_image() const
{
	return _right_image;
}

    
// Iterate on a directory where left right images are stored in a single image file
class SplitImagesDirectory
{
	 directory_iterator directory_it;
	 boost::scoped_ptr<SplitImagesDirectoryIterator> 
	 	begin_iterator_p, end_iterator_p;
	public:
		SplitImagesDirectory(const string &directory_name);
		~SplitImagesDirectory();
	
		typedef SplitImagesDirectoryIterator  iterator;
		typedef SplitImagesDirectoryIterator const_iterator;
		// <<< how to define properly a const_iterator ? and non const_one ?
	
		const_iterator &begin();
		const_iterator &end();
};

SplitImagesDirectory::SplitImagesDirectory(const string &input_directory_name)
{

 	path input_images_path(input_directory_name);

    if( !exists(input_images_path))
		throw runtime_error("Indicated input images path does not exist");

    if( !is_directory(input_images_path) )
		throw runtime_error("Indicated input images path is not a directory");

     
	directory_iterator directory_it( input_directory_name );
	begin_iterator_p.reset(new SplitImagesDirectoryIterator(directory_it));
		
	// default construction yields past-the-end
	end_iterator_p.reset(new SplitImagesDirectoryIterator());
		 
	return;
}

SplitImagesDirectory::~SplitImagesDirectory()
{
	// nothing to do here
	return;
}

SplitImagesDirectory::const_iterator &SplitImagesDirectory::begin()
{
	return *begin_iterator_p;
};

SplitImagesDirectory::const_iterator &SplitImagesDirectory::end()
{
	return *end_iterator_p;
};


// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
template<typename ImagesDirectory> 
void process_images(ImagesDirectory &image_directory, args::variables_map& options)
{ // process all the images in the directory
	
	
	double min_shift = 5.0, max_shift = 15.0;
      if(options.count("min_shift"))
	min_shift = options["min_shift"].as<double>();
      if(options.count("max_shift"))
	max_shift = options["max_shift"].as<double>();
	
	
	typename ImagesDirectory::const_iterator images_it;
	
	typedef uint8_t value_type;
	
	 // for each image in the input set ---
	for(images_it = image_directory.begin();
		images_it != image_directory.end();
		++images_it)
	{			
	  if(true)
		{
		  testStereo<value_type>(
		  				images_it->left_image(), 
		  				images_it->right_image(),
								 min_shift, max_shift);
		}	
	} // end of 'for each image in the input directory'
	
	
	
    if(false)
	{
	  CImg<value_type> left_image("tsukuba1color.png"), right_image("tsukuba2color.png");
	  testStereo(left_image, right_image, min_shift, max_shift);
	}
	
	return;
}


// ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
int test_stereo_main(int argc, char *argv[])
{

  try
    { // all the code is exception catch protected
      
#ifdef WIN32
      freopen( "CON", "w", stdout ); // redirect stdout to console output
      freopen( "CON", "w", stderr ); // redirect stderr to console output
#endif

      cout << Program_title << endl << endl;
   
      // define the options --
      args::options_description desc("Application options");

      desc.add_options()
        ("help", "produce help message")
        ("cimg_info", args::value< bool >()->zero_tokens(),
         "print some informations about the CImg lbirary and exit")
        
	("input_path,i", args::value< string >(),
         "path to the directory where input images are stored."
         "Expects to find <path>/left and <path>/right subdirectories with the respective images,"
         "or a list of images where left half and right half contains the stereo pair")

        ("output_path,o", args::value< string >(),
         "Path where the disparity images will be recorded")

        ("min_shift", args::value< double >()->default_value(-100.0),
         "")
	("max_shift", args::value< double >()->default_value(100.0),
         "")

	//         ("patch_size", args::value< int >()->default_value(25),
	//          "Size of sides of the patches used to compute the features vectors [pixels]")
	// 	  ("max_patches_per_file", args::value< int >()->default_value(100),
	//          "Will subsample the trainning examples if too many pat")
        ;


      // parse the options --
      args::variables_map options;
      args::store(args::parse_command_line(argc, argv, desc), options);
      args::notify(options);

      if ( options.count("help") )
        {
	  cout << desc << endl;
	  return 0;
        }

      if( options.count("cimg_info") )
        {
	  cimg::info(); // print some information about the environment configuration
	  return 0;
        }


      string input_images_directory;
      if( options.count("input_path") == 0)
	throw runtime_error("No input images directory set");
      else
	input_images_directory = options["input_path"].as<string>();

	// main processing call ---
	
	// <<< this try/catch stuff is pure crap ! how to do better ?!
	try
	{
		LeftRightDirectories input_images(input_images_directory);
		process_images<>(input_images, options);
	}
	catch(...)
	{
		cout << "Using LeftRightDirectories failed, will try SplitImagesDirectory" << endl;
		
		SplitImagesDirectory input_images(input_images_directory);
		process_images<>(input_images, options);
	}
	
      
      // end of the game --
      printf( "\n\nHave a nice day\n\n" );
       
    } // end of initial 'try'
  catch(exception &e)
    {
      cout << "An exception was raised: " << e.what() << endl;
    }

  return 0; // end of main

  // parse input arguments
   
    
  // for every pair of files in the directory 
  {
	
    // compute disparity
   
    // update image
   	
    // save into a TIFF file
  }
   

  return 0;
} // end of main
