# MultiCam-Detector
This program makes a pixel wise replacement from a Slave camera to a
Master camera, for an XRay or Neutron imaging detector. The Master
camera configuration may generate images with saturated pixels due to
the lack of light coming from the scintillator of the detector, as
well as gamma streaks. These are very challenging to be denoised
without altering the raw images with spacial medians and/or
z-medians. Therefore, the information lost in these hot pixels is
taken from the Slave camera (a secondary camera) by doing the pixel
wise replacement. However, the slave camera may have a different setup
concerning the zoom, rotation, and perspective point of view. Thus,
the program is meant to first analyze the Master image looking for the
coordinates of hot pixels, transform the Slave image, get the
correspondent pixel value from the Slave image, and replace the
saturated information of the hot pixels.  

#Usage in mac/linux
./multicam with no arguments prompt help. Usage:
./multicam <path> <MasterCam_image> <SlaveCam_image> <configfile>"<< std::endl;
        <path> is the working path of the input and output files.
        <MasterCam_image> is the picture in .tif or .fit format in where the pixel value with coordinates from <hotpixels_file> will be replaced with the pixel values of the same coordinates from the <SlaveCam_image>.
        <SlaveCam_image> is the picture in .tif or .fit format to use to correct the values in the picture from the Master Cammera."
        <configfile> is the config file where registration points are
		stored."

#Registration of the images:
The config.cfg file has to be located in the same folder
 where master and slave images are located, i.e. the program looks for
 this file in the specified path in the arguments of the command line
 in Linux/Mac. As for Windows, it also has to be located in the same
 folder where the images are. This is to allow for different
 config.cfg files for different set of images. Use the
 config_example.cfg file provided here as a reference.

#Windows install:

This program compiles with Visual Studio 2019. 
Download for Windows: https://download.imagemagick.org/ImageMagick/download/binaries/
- Opencv library used as well for points transformations. Available downloading with Brew install opencv. Lib setting available in: https://medium.com/@jaskaranvirdi/setting-up-opencv-and-c-development-environment-in-xcode-b6027728003
Download for Windows: https://github.com/opencv/opencv/releases

Visual studio setup: follow instructions in
https://www.youtube.com/watch?v=eDGSkdeV8YI
however in the minute 2.40 instead of link to .dll it has to link to
the .lib, also add the path to the .lib in the
Linker->General->Additional Library Directores



#Mac install:

brew install imagemagick@6

brew install opencv@2
brew install pkg-config

Makefile config for opencv is taken from:
pkg-config --cflags --libs /usr/local/opt/opencv\@2/lib/pkgconfig/opencv.pc



#History and contact information:
- Windows version 10/21/2021 @ NIST, Gaithersburg, by Federico Suarez.
- Created by Federico Suarez 9/11/2021 @ NIST, Gaithersburg. Contact: fedesuarez79@gmail.com or WhatsApp/Telegram +5491151447588.
- Program based on imagemagick libraries and examples at https://imagemagick.org/MagickWand/
Library at https://imagemagick.org/script/magick-wand.php
