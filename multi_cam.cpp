/*
- Repository created in https://github.com/fedesuarez79/MultiCam-Detector
- Windows version 10/21/2021 @ NIST, Gaithersburg, by Federico Suarez.
- Created by Federico Suarez 9/11/2021 @ NIST, Gaithersburg. Contact: fedesuarez79@gmail.com or WhatsApp/Telegram +5491151447588.
- Program based on imagemagick libraries and examples at https://imagemagick.org/MagickWand/
Library at https://imagemagick.org/script/magick-wand.php
Download for Windows: https://download.imagemagick.org/ImageMagick/download/binaries/
- Opencv library used as well for points transformations. Available downloading with Brew install opencv. Lib setting available in: https://medium.com/@jaskaranvirdi/setting-up-opencv-and-c-development-environment-in-xcode-b6027728003
Download for Windows: https://github.com/opencv/opencv/releases
- The logic of the program is to replace the information of hot pixels from the picture taken by a Master Cammera with the information of the same pixels from a picture taken by a Slave Cammera. The SlaveCam could be located off axis so its ratio might need to be modified. The list of bad pixels can be provided to the program by a text file containing x-y coordinates.

*/


#include "multi_cam.h"


/*************************
***** MAIN FUNCTION  *****
*************************/

#ifdef _WIN32 
//Windows version
int main(){
    std::string path, mastercam_file, slavecam_file, config_file;
    mastercam_file = "master_f1.4_3s_00001_000001.tif";
    slavecam_file = "slave_f1.4_3s_00001_000001.tif";
    config_file = "config.cfg";
    path = "C:\\Users\\fns14\\Downloads\\NewSetup";

#else
//linux and mac code goes here
int main(int argc, const char** argv) {
    std::string path, mastercam_file, slavecam_file, config_file;
    if (argc == 5) {
        path = argv[1];
        mastercam_file = argv[2];
        slavecam_file = argv[3];
        config_file = argv[4];
    }
    else {
        std::cerr << "Program usage: ./command <path> <MasterCam_image> <SlaveCam_image> <configfile>"<< std::endl;
        std::cerr << "<path> is the working path of the input and output files." << std::endl;
        std::cerr << "<MasterCam_image> is the picture in .tif or .fit format in where the pixel value with coordinates from <hotpixels_file> will be replaced with the pixel values of the same coordinates from the <SlaveCam_image>." << std::endl;
        std::cerr << "<SlaveCam_image> is the picture in .tif or .fit format to use to correct the values in the picture from the Master Cammera." << std::endl ;
        std::cerr << "<configfile> is the config file where registration points are stored." << std::endl << std::endl;
	exit(0);
    }
#endif


    /////////////////////////////
    ////  INITIALIZING       ////
    /////////////////////////////
    std::vector<ConfigParameters> config_parameters = Init(path, config_file);

    /////////////////////////////
    ////  PROGRAM START      ////
    /////////////////////////////

    //// Get list of hot pixels in Master raw image
    std::vector<cv::Point2f> hotpoints = ImageGetHotPoints(mastercam_file, GetParameterValueFromConfig(config_parameters, "MasterThresholdHotPixels"));
    
    //// Generate flat slave image where pixel information is going to be taken from (Rotate and perspective transform).
    ImageRotateAndPerspectiveTransformation("Slave", slavecam_file, config_parameters);
    ImageRotateAndPerspectiveTransformation("Master", mastercam_file, config_parameters);
 
    //// Transform hotpoints from list of coordinates to flathotpoints corresponding to flat images
    std::vector<cv::Point2i> flathotpoints = PointsRotateAndPerspectiveTransformation("Master", hotpoints, config_parameters);

    ////Adjust Brightness and Contrast
    ImageAdjustBrightnessContrast("SlaveFinal.tif", "SlaveFinalAdjusted.tif", GetParameterValueFromConfig(config_parameters, "SlaveBrightness"), GetParameterValueFromConfig(config_parameters, "SlaveContrast"));

    ///// Get values from Slave flat image corresponding to flathotpoints
    std::cout << "Get VALUES FOR HOTPIXELS FROM SLAVE IMAGE" << std::endl;
    std::vector<Coords> flatcoordvalues = PointsGetValues("SlaveFinalAdjusted.tif", flathotpoints); //stores the coordinates and value extracted from flat image

    ///// Set pixel values 
    std::cout << std::endl << "SET VALUES OF HOTPIXELS IN MASTER IMAGE" << std::endl;
    std::vector<Coords> coordvalues; //to store the coordinates and values for raw image
    Coords dummycoords;

    //Get vector of hotpoints with integer values
    std::vector<cv::Point2i> hotpoints_i;
    for (int i = 0; i < hotpoints.size(); i++) {
        hotpoints_i.push_back(hotpoints[i]);
    }

    std::cout << "# hotpoints: " << hotpoints_i.size() << std::endl;
    std::cout << "# coordflatim: " << flatcoordvalues.size() << std::endl;

    for (int i = 0; i < flatcoordvalues.size(); i++) { //dump values from flatcoordvalues to hotpoint coordinates, the order should be correct
        dummycoords.x = hotpoints_i[i].x;
        dummycoords.y = hotpoints_i[i].y;
        dummycoords.v = flatcoordvalues[i].v;
        coordvalues.push_back(dummycoords);
    }
    //for(int i=0;i<coordvalues.size();i++){
    //  std::cout<<coordvalues[i].x<<" "<<coordvalues[i].y<<" "<<coordvalues[i].v<<" "<<std::endl;
    //}
    SetValues(mastercam_file, "MasterCorregida.tif", coordvalues);

    ////////////////////////////////////////////
    ////  OPTIONAL IMAGES WITH CIRCLES      ////
    ////////////////////////////////////////////

    //// Draw circles around flathotpoints in Master and Slave flat images
    ImageDrawCirclesAroundPoints("SlaveFinalAdjusted.tif", flathotpoints);
    ImageDrawCirclesAroundPoints("MasterFinal.tif", flathotpoints);
    ImageDrawCirclesAroundPoints("MasterCorregida.tif", hotpoints_i);
    ImageDrawCirclesAroundPoints(mastercam_file, hotpoints_i);

    
    return 0;
}
