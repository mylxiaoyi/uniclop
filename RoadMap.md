# Road map #

  1. [Visual odometry](RoadMap#Visual_odometry.md) (work in progress)
  1. [Self calibration](RoadMap#Self_calibration.md) (desired)
  1. [Dense visual SLAM](RoadMap#Dense_visual_SLAM.md) (maybe)
<wiki:gadget url="http://hosting.gmodules.com/ig/gadgets/file/101831953162459328471/MyPicasa.xml" up\_feed="http://picasaweb.google.com/data/feed/base/user/rodrigo.benenson/albumid/5346596060422944273?alt=rss&kind=photo&hl=en\_US" up\_random="1" up\_interval="5" up\_bgColor="ffffff" title="Work in progress images" height="300" border="0" />

[album](http://picasaweb.google.com/rodrigo.benenson/Uniclop?feat=directlink)

## Visual odometry ##

  1. ~~Setup Mercurial source tree and build process~~ DONE (using scons and monodevelop)
  1. ~~Create the C++ guideline and cpplint.py variant~~ DONE
  1. ~~Create a video input application (from webcam, video file and directory with images) (using Cheese as a basis)~~ DONE
  1. Create features computation namespace and implement/warp a few. WIP
    * Must have : Star, Daisy
    * Nice to have (in order of priority): Harris, SURF, KLT, SIFT, dense\_sift; ~~FAST~~ done
  1. Create features matcher using F matrix constraints and RANSAC. WIP
    * Must have : 5-points algorithm, RANSAC
    * Nice to have (in order of priority): PROSAC, 8-points algorithm, ARRSAC, [GroupSAC](http://www.cc.gatech.edu/~nikai//assets/Ni09iccv.pdf)
  1. Create displacement estimation application using 5-points algorithms
    * How to have a consistent scale ?
  1. Create a displacement estimation application using previous steps and 3d triangulation
    * Should have nice visualisation using VTK
  1. Create a displacement estimation application using previous steps and bundle adjustment
    * This step should provide a descent online monocular visual odometry
  1. ???
  1. Create .deb installer
  1. Create .msi installer

## Self calibration ##
  1. Start from the visual odometry application
  1. ???
  1. Create offline camera calibration application
  1. Create online camera calibration application
  1. ???
  1. Create .deb installer
  1. Create .msi installer

## Dense visual SLAM ##
  1. Start from the visual odometry application
  1. Compute dense stereo matching between successive keyframes using DAISY
  1. ???
    * How to integrate this sequence of localized depth maps in a consistent local map ?
  1. ???
  1. Create .deb installer
  1. Create .msi installer



---


Developments steps:

multiple applications

  1. Video input
  1. Features detection
  1. Features tracking
  1. Displacement estimation using 5-points algorithm only (and manual calibration)
  1. 3D Visualization of feature points
  1. Dense stereo matching using Daisy and known baseline (manual baseline measurement)
  1. Dense stereo matching using Daisy and baseline from displacement estimate
> -> Target: visualisation of a dense cloud of 3D points + vehicule trajectory
