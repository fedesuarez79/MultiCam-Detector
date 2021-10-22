#include <iostream>
#include <opencv2/opencv.hpp>
#include <wand/MagickWand.h>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>

#ifdef _WIN32 
//Windows version
#include <Windows.h>
#include <tchar.h>

#else
//linux and mac code goes here
#include <unistd.h>

#endif

//Class to store point coordinates and gray value
class Coords {
public:
    int x, y, v; //x and y are coordinates and v is the gray value of the pixel in the corresponding coordinate
};

//Class to store config parameters
class ConfigParameters {
public:
    std::string parameter; //parameter name from config file
    double value; //value corresponding to the parameter
};


/************************
Open Config File and return all the values in a vector of ConfigParameters.
<filename> is the filename containing the registration points and config variables.
Returns a vector of ConfigParameters that has parameter names and values.
*************************/
std::vector<ConfigParameters> GetConfigFile(std::string filename)
{
    std::vector<ConfigParameters> config_parameters;
    ConfigParameters dummy;
    std::cout << "Read Config file: " << filename << "... ";

    std::ifstream cFile(filename.c_str());
    if (cFile.is_open())
    {
        std::string line;
        while (getline(cFile, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                line.end());
            if (line[0] == '#' || line.empty())
                continue;
            int delimiterPos = line.find("=");
            std::string name = line.substr(0, delimiterPos);
            double value = stof(line.substr(delimiterPos + 1));
            //std::cout << name << " " << value << '\n';
            dummy.parameter = name;
            dummy.value = value;
            config_parameters.push_back(dummy);
        }

    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
        exit(0);
    }
    std::cout << "OK!" << std::endl;
    return config_parameters;
}


/************************
Initialize program. First set working directory and then read config file. 
<path> is the working path to change working directory to. 
<config_file> is the filename in the working directory to read.
Returns a vector of ConfigParameters that has parameter names and values.
*************************/
std::vector<ConfigParameters> Init(std::string path, std::string config_file) {
    std::cout << std::endl << "INITIALIZING" << std::endl;

#ifdef _WIN32 
    //// Set working path   
    std::wstring stemp = std::wstring(path.begin(), path.end());
    LPCWSTR sw = stemp.c_str();
    if (!SetCurrentDirectory(sw))
    {
        printf("SetCurrentDirectory failed (%d): check correct path and or include double slashes in string definition.\n", GetLastError());
        exit(0);
    }
    TCHAR Buffer[MAX_PATH];
    DWORD dwRet;
    dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
    if (dwRet == 0)
    {
        printf("GetCurrentDirectory failed (%d)\n", GetLastError());
        exit(0);
    }
    _tprintf(TEXT("Current working path (%s)\n"), Buffer);
#else
    chdir(path.c_str());
    char s[300];
    std::cout<<"Current working path: "<<getcwd(s,300)<<std::endl;
#endif

    //// Read Config File
    std::vector<ConfigParameters> config_parameters = GetConfigFile(config_file);
    std::cout<<"Initialization finished OK!"<<std::endl<<std::endl;
    return config_parameters;
}


/************************
Get list of hotpoints from file. 
<filename> is a text file containing a list of x y coordinates of hotpoints. Each raw is a point.
Returns vector of points containing the coordinates of the values read from file.
*************************/
std::vector<cv::Point2f> GetPointsFromFile(std::string filename)
{

    std::cout << "GET LIST OF HOTPOINTS" << std::endl;

    std::vector<cv::Point2f> points;
    std::cout << "Read file: " << filename << "... ";

    std::string x, y;
    std::ifstream cFile(filename.c_str());
    if (cFile.is_open())
    {
        while (!cFile.eof()) {
            cFile >> x;
            if (x.at(0) == '#') {
                cFile.ignore(1, '\n');
                continue;
            }
            if (cFile.eof())break;
            cFile >> y;
            if (cFile.eof())break;
            points.push_back(cv::Point2f(stof(x), stof(y)));
        }

    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    std::cout << "OK!" << std::endl << std::endl;
    return points;
}

/************************
Read table of points from config parameters and return the string of floats needed for the Distort Perspective method. 
<config_parameters> contains the table of pixel coordinates as text file x y per row (points are top-left x-y, top-right x-y, bottom-left x-y, bottom-right x-y). It is generated with GetConfigFile(). 
<points> is where the set of 8 values will be stored. 
<image> must be "Slave" or "Master"
returns <points>
*************************/
double* GetPerspectivePointsFromConfig(std::vector<ConfigParameters> config_parameters, double points[], std::string image) {
    for (int i = 0; i < config_parameters.size(); i++) {
        if (config_parameters[i].parameter == image + "SourceTopLeftX") {
            points[0] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceTopLeftY") {
            points[1] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestTopLeftX") {
            points[2] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestTopLeftY") {
            points[3] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceTopRightX") {
            points[4] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceTopRightY") {
            points[5] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestTopRightX") {
            points[6] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestTopRightY") {
            points[7] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceBottomLeftX") {
            points[8] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceBottomLeftY") {
            points[9] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestBottomLeftX") {
            points[10] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestBottomLeftY") {
            points[11] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceBottomRightX") {
            points[12] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "SourceBottomRightY") {
            points[13] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestBottomRightX") {
            points[14] = config_parameters[i].value;
        }
        else if (config_parameters[i].parameter == image + "DestBottomRightY") {
            points[15] = config_parameters[i].value;
        }
    }
    return points;
}

/************************
Read table of points from config parameters and return the string of floats needed for the Distort Perspective method. 
<config_parameters> contains the table of pixel coordinates as text file x y per row (points are top-left x-y, top-right x-y, bottom-left x-y, bottom-right x-y). It is generated with GetConfigFile(). 
<name> is the parameter name to look for.
Returns the parameter value specified.
*************************/
double GetParameterValueFromConfig(std::vector<ConfigParameters> config_parameters, std::string name) {
    for (int i = 0; i < config_parameters.size(); i++) {
        if (config_parameters[i].parameter == name) {
            return config_parameters[i].value;
        }
    }
    return 0;
}

/************************
Open image and look for pixel coordinates with values higher or equal to threshold. 
<image_name is a grayscale image file. 
<threshold> is the value from 0-65535 (black to white).
Returnds vector of points containing coordinates of points with value above or equal to threshold.
*************************/
std::vector<cv::Point2f> ImageGetHotPoints(std::string image_name, long threshold) {

    std::cout << "FIND HOT PIXELS IN " + image_name << std::endl;

    ////Return vector definition
    std::vector<cv::Point2f> hotpoints;
    cv::Point2f dummy;

    /////Input and Output Image matrices;
    cv::Mat input, output, output2;

    //// Create a wand 
    MagickWand* mw = NULL;
    MagickWandGenesis();
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);

    //// Open image
    std::cout << "Opening file: " << image_name << " to get bad pixel values... ";
    if (!MagickReadImage(mw, image_name.c_str())) {
        std::cerr << std::endl << "Could not open " << image_name << " image... Aborting." << std::endl;
        exit(0);
    }
    else {
        std::cout << "OK!" << std::endl;
    }

    std::cout << "Looking for hot pixels with value >="<<threshold<<"... ";

    //// Pixel region iterator and pixel wand to get px input info
    PixelIterator* imw;
    PixelWand** pmw;
    long y;
    long x;
    long v;
    std::vector<Coords> coordinates;
    Coords dummycoords;
    size_t width, height;

    //// Scan the whole image
    width = MagickGetImageWidth(mw);
    height = MagickGetImageHeight(mw);
    imw = NewPixelIterator(mw); //pixel iterator
    for (int y = 0; y < height; y++) {
        ///// Get the next row from each image
        pmw = PixelGetNextIteratorRow(imw, &width);
        for (x = 0; x < (long)width; x++) {
            v = PixelGetRedQuantum(pmw[x]);
            if (v >= threshold) {
                dummycoords.v = v;
                dummycoords.x = x;
                dummycoords.y = y;
                //std::cout<<x<<" "<<y<<" "<<v<<std::endl;
                coordinates.push_back(dummycoords);
                dummy = cv::Point(x, y);
                hotpoints.push_back(dummy);
            }
        }
    }
    std::cout << hotpoints.size() << " found!" << std::endl;

    ///// Clean up the iterators and magick wands
    imw = DestroyPixelIterator(imw);
    mw = DestroyMagickWand(mw);
    MagickWandTerminus();
    std::cout << std::endl;
    return hotpoints;
}


/************************
Transform image to rotate and compensate perspective distortion. 
<image_type> is either "Slave" or "Master". 
<image_name> image file name to work with. 
<config_parameters> contains the table of pixel coordinates as text file x y per row. It is generated with GetConfigFile().
Returns true if execution was correct.
*************************/
bool ImageRotateAndPerspectiveTransformation(std::string image_type, std::string image_name, std::vector<ConfigParameters> config_parameters) {

    std::cout << "TRANSFORMATION OF " + image_type + " IMAGE" << std::endl;

    /////Input and Output Image matrices;
    cv::Mat input, output, output2;

    /////Load the image
    std::cout << "Open file: " + image_name + "... ";
    input = cv::imread(image_name.c_str(), cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);
    if (input.empty()) {
        std::cerr << std::endl << "Could not open " << image_name << " image... Aborting." << std::endl;
        exit(0);
    }
    std::cout << "OK!" << std::endl;

    /////Generate points for matrices generation
    cv::Point2f src[4], dst[4];
    /////Perspective source quad-points and destination quad-points, clockwise
    src[0] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceTopLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceTopLeftY"));
    src[1] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceTopRightX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceTopRightY"));
    src[2] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomRightX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomRightY"));
    src[3] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomLeftY"));
    dst[0] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestTopLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "DestTopLeftY"));
    dst[1] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestTopRightX"), GetParameterValueFromConfig(config_parameters, image_type + "DestTopRightY"));
    dst[2] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestBottomRightX"), GetParameterValueFromConfig(config_parameters, image_type + "DestBottomRightY"));
    dst[3] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestBottomLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "DestBottomLeftY"));

    //////ROTATION//////
    cv::Point origin = cv::Point(0, 0); //origin point set to 0,0 because rotations between images are calculated from there
    /////Generation of rotation matrices for master image
    double rotangle = GetParameterValueFromConfig(config_parameters, image_type + "Rotation");
    std::cout << "Rotating " + image_type + " image with " << rotangle << " degrees... ";
    cv::Mat rotmat = getRotationMatrix2D(origin, rotangle, 1);
    //std::cout<<rotmat<<std::endl;
    /////Rotate image
    warpAffine(input, output, rotmat, output.size());
    /////draw circle in points of rotated image to be used in perspective transformation
    cv::circle(output, src[0], 15, cv::Scalar(0, 0, 0), 5, 1);
    cv::circle(output, src[1], 15, cv::Scalar(0, 0, 0), 5, 1);
    cv::circle(output, src[2], 15, cv::Scalar(0, 0, 0), 5, 1);
    cv::circle(output, src[3], 15, cv::Scalar(0, 0, 0), 5, 1);
    /////save rotated image
    cv::imwrite(image_type + "Rotated.tif", output);
    std::cout << "OK!" << std::endl;

    //////PERSPECTIVE TRANSFORMATION///////
    std::cout << "Applying Perspective Transformation for " + image_type + " image... ";
    cv::Mat perspmat = getPerspectiveTransform(src, dst);
    //std::cout<<perspmat<<std::endl;  
    /////apply perspective transformation
    warpPerspective(output, output2, perspmat, output2.size());
    //Display input and output
     //imshow("Input",input);
     //here it should be the pause
     //int k = cv::waitKey(0);
     //if(k == 27){         //wait for ESC key to exit
     //    cv::destroyAllWindows();
     //}

     /////Save final image
    cv::imwrite(image_type + "Final.tif", output2);
    std::cout << "OK!" << std::endl;

    std::cout << std::endl;
    return true;
}

/************************
Transform points of coordinates of pixels to rotate and compensate perspective distortion. 
<image_type> is either "Slave" or "Master". 
<hotpoints> is the input vector of point2f points to be transformed. It is generated with GetConfigFile().
Returns vector of integer points transformed.
*************************/
std::vector<cv::Point2i> PointsRotateAndPerspectiveTransformation(std::string image_type, std::vector<cv::Point2f> hotpoints, std::vector<ConfigParameters> config_parameters) {

    std::cout << "TRANSFORMATION OF HOTPOINTS/" << std::endl;

    /////Generate points for matrices generation
    cv::Point2f src[4], dst[4];
    /////Perspective source quad-points and destination quad-points, clockwise
    src[0] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceTopLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceTopLeftY"));
    src[1] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceTopRightX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceTopRightY"));
    src[2] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomRightX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomRightY"));
    src[3] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "SourceBottomLeftY"));
    dst[0] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestTopLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "DestTopLeftY"));
    dst[1] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestTopRightX"), GetParameterValueFromConfig(config_parameters, image_type + "DestTopRightY"));
    dst[2] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestBottomRightX"), GetParameterValueFromConfig(config_parameters, image_type + "DestBottomRightY"));
    dst[3] = cv::Point2f(GetParameterValueFromConfig(config_parameters, image_type + "DestBottomLeftX"), GetParameterValueFromConfig(config_parameters, image_type + "DestBottomLeftY"));

    //////ROTATION//////
    cv::Point origin = cv::Point(0, 0); //origin point set to 0,0 because rotations between images are calculated from there
    //Generation of rotation matrices for master image
    double rotangle = GetParameterValueFromConfig(config_parameters, image_type + "Rotation");
    std::cout << "Rotating " + image_type + " raw image hotpoints " << rotangle << " degrees... ";
    cv::Mat rotmat = getRotationMatrix2D(origin, rotangle, 1);
    //std::cout<<rotmat<<std::endl;
    /////Rotate points
    std::vector<cv::Point2f> rotatedpoints;
    transform(hotpoints, rotatedpoints, rotmat);
    std::cout << "OK!" << std::endl;

    //////PERSPECTIVE TRANSFORMATION///////
    std::cout << "Applying Perspective Transformation for " + image_type + " raw hotpoints... ";
    cv::Mat perspmat = getPerspectiveTransform(src, dst);
    //std::cout<<perspmat<<std::endl;

    /////Apply perspective transformation
    std::vector<cv::Point2f> finalpoints;
    perspectiveTransform(rotatedpoints, finalpoints, perspmat);
    std::cout << "OK!" << std::endl;
    //std::cout<<"List of Flat hotpoints coordinates:"<<std::endl<<flathotpoints<<std::endl;

  /////Return vector of integer points rounded.
    std::vector<cv::Point2i> dummy;
    for (int i = 0; i < finalpoints.size(); i++) {
        dummy.push_back(finalpoints[i]);
    }

    std::cout << std::endl;
    return dummy;
}

/************************
Adjusts Brightness and Constrast of an image. 
<image_name> contains the image path. 
<brightness> is a value in percent -100 to 100. 
<contrast> is a value in percent -100 to 100.
Returns true if the execution was correct.
*************************/
bool ImageAdjustBrightnessContrast(std::string image_name, std::string output_name, double brightness, double contrast) {
    std::cout << "ADJUST CONTRAST IN " + image_name << std::endl;
    //// Create a wand 
    MagickWand* mw = NULL;
    MagickWandGenesis();
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);

    //// Open image
    std::cout << "Opening file: " << image_name << " to adjust brightness and contrast... ";
    if (!MagickReadImage(mw, image_name.c_str())) {
        std::cerr << std::endl << "Could not open " << image_name << " image... Aborting." << std::endl;
        exit(0);
    }
    else {
        std::cout << "OK!" << std::endl;
    }

    std::cout << "Adjusting Brightness " << brightness << "% and contrast " << contrast << "%... ";
    MagickBrightnessContrastImage(mw, brightness, contrast);
    MagickWriteImage(mw, output_name.c_str());
    std::cout << "OK!" << std::endl;

    mw = DestroyMagickWand(mw);
    MagickWandTerminus();
    std::cout << std::endl;
    return true;
}

/************************
Draw circles around points for easy visualization of the pixels to be transformed. 
<image_name> contains the image path. 
<points> is a std::vector<cv::Point2i> of points to draw.
Returns true if the execution was correct.
*************************/
bool ImageDrawCirclesAroundPoints(std::string image_name, const std::vector<cv::Point2i>& points) {
    std::cout << "DRAWING CIRCLES IN IMAGE " + image_name << std::endl;

    /////Input and Output Image matrices;
    cv::Mat input;

    /////Load the image
    std::cout << "Open file: " + image_name + "... ";
    input = cv::imread(image_name.c_str(), cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);
    if (input.empty()) {
        std::cerr << std::endl << "Could not open " << image_name << " image... Aborting." << std::endl;
        exit(0);
    }
    std::cout << "OK!" << std::endl;

    /////Draw circle in points
    std::string label;
    cv::Point2i shifted;
    std::cout << "Drawing circles... ";
    for (int i = 0; i < points.size(); i++) {
        //std::cout<<points[i]<<std::endl;
        cv::circle(input, points[i], 15, 0, 5, 1);
        label = std::to_string(i);
        shifted.x = points[i].x + 15;
        shifted.y = points[i].y - 15;
        cv::putText(input,
            label.c_str(),
            shifted, // Coordinates (Bottom-left corner of the text string in the image)
            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
            1.5, // Scale. 2.0 = 2x bigger
            0, // BGR Color
            2, // Line Thickness (Optional)
            1); // Anti-alias (Optional, see version note)
    }

    /////Save file
    std::string dummy = image_name.erase(image_name.length() - 4);
    cv::imwrite(dummy + "WithCircles.tif", input);
    std::cout << "OK!" << std::endl;

    std::cout << std::endl;
    return true;
}

/************************
Transform image to rotate and compensate perspective distortion. 
<image_type> is either "Slave" or "Master". 
<image_name> image file name to work with. 
<config_parameters> contains the table of pixel coordinates as text file x y per row (points are top-left x-y, top-right x-y, bottom-left x-y, bottom-right x-y). It is generated with GetConfigFile().
Returns true if the execution was correct.
*************************/
bool RotateAndPerspectiveCorrectionMW(std::string image_type, std::string image_name, std::vector<ConfigParameters> config_parameters) {
    double persp_points[16];
    GetPerspectivePointsFromConfig(config_parameters, persp_points, image_type);

    MagickWand* mw = NULL;
    MagickWandGenesis();
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);
    if (!MagickReadImage(mw, image_name.c_str())) {
        std::cerr << std::endl << "Could not open " << image_name << " image... Aborting." << std::endl;
        exit(0);
    }

    //Rotate image
    double rotation = GetParameterValueFromConfig(config_parameters, image_type + "Rotation");
    PixelWand* fill = NULL;
    /* Pixel and drawing wands */
    fill = NewPixelWand();
    // Set the fill to "black" for background pixels
    PixelSetColor(fill, "black");
    std::cout << "Rotate " << image_type << " image " << rotation << " degrees... " << std::endl;
    MagickRotateImage(mw, fill, rotation);
    //Crop image after rotation
    MagickCropImage(mw, persp_points[14] + 1, persp_points[15] + 1, 0, 0);
    std::cout << "OK!" << std::endl;

    //Distort image to correct for perspective
    std::cout << "Perspective Correction for " << image_type << " image... " << std::endl;
    MagickDistortImage(mw, PerspectiveDistortion, 16, (const double*)&persp_points, MagickFalse);
    std::string dummy = image_type + "Out.tif";
    MagickWriteImage(mw, dummy.c_str());
    std::cout << "OK!" << std::endl;
    //cleaning
    if (mw)mw = DestroyMagickWand(mw);
    MagickWandTerminus();

    return true;
}

/************************
Open image and resize. 
<filename> is the input file. 
<width> is the size in percentage for the ouptut image width. 
<height> is the size in percentage for the output image height.
Returns true if execution was correct.
*************************/
bool ResizeImage(std::string filename, std::string outfile, int perc_width, int perc_height) {
    std::cout << "Resizing image: " << filename << " to " << perc_width << "% width and " << perc_height << "% height... ";
    MagickWand* mw = NULL;
    int w, h;
    MagickWandGenesis();
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);
    if (!MagickReadImage(mw, filename.c_str())) {
        std::cerr << std::endl << "Couldn't open " << filename << " original image from SlaveCam... Aborting." << std::endl;
        exit(0);
    }

    // Get the image's width and height
    w = MagickGetImageWidth(mw);
    h = MagickGetImageHeight(mw);
    // Scale image
    w = w * perc_width / 100;
    h = h * perc_height / 100;

    // Resize the image using the Lanczos filter
    // The blur factor is a "double", where > 1 is blurry, < 1 is sharp
    // I haven't figured out how you would change the blur parameter of MagickResizeImage on the command line so I have set it to its default of one.
    MagickResizeImage(mw, w, h, LanczosFilter, 1);

    // Write the new image
    if (MagickWriteImage(mw, outfile.c_str())) std::cout << "Saved scaled imaged as " << outfile << "... DONE." << std::endl;

    //clean up
    if (mw)mw = DestroyMagickWand(mw);
    MagickWandTerminus();
    return true;
}

/************************
Read values of pixels from image and return the reference of the std::vector of class Coords where the coordinates will be stored with the pixel gray value and returned. 
<filename> corresponding to the image to read values from.
<points> is the vector of points to read values from.
Returns vector of coordinates Coords containing x, y and value for each point.
*************************/
std::vector<Coords> PointsGetValues(std::string filename, std::vector<cv::Point2i> points) {
    MagickWand* mw = NULL;
    MagickWandGenesis();
    std::vector<Coords> coordinates;
    Coords dummy_coords;

    /* Create a wand */
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);
    /* Read the input image */
    std::cout << "Opening file: " << filename << " to get bad pixel values... ";
    if (!MagickReadImage(mw, filename.c_str())) {
        std::cerr << std::endl << "Could not open " << filename << " image... Aborting." << std::endl;
        exit(0);
    }
    /* Pixel region iterator and pixel wand to get px input info */
    PixelIterator* iterator = NULL;
    PixelWand** pixel = NULL;
    size_t x;
    int a, b;

    for (int i = 0; i < points.size(); i++) {
        a = points[i].x; //Point x
        b = points[i].y; //Point y
        dummy_coords.x = a;
        dummy_coords.y = b;

        iterator = NewPixelRegionIterator(mw, a, b, 1, 1);
        pixel = PixelGetNextIteratorRow(iterator, &x);
        // save value in coordinates
        dummy_coords.v = PixelGetRedQuantum(pixel[0]);
        //std::cout<<i<<" "<<dummy_coords.x<<" "<<dummy_coords.y<<" "<<dummy_coords.v<<std::endl;
        coordinates.push_back(dummy_coords);
    }
    std::cout << "OK!" << std::endl;
    /* Tidy up */
    if (mw) mw = DestroyMagickWand(mw);
    if (iterator) iterator = DestroyPixelIterator(iterator);
    MagickWandTerminus();
    return coordinates;
}

/************************
Remove points that have either negative x or y coordinate
<points> is the input vector of points (it may contain negative points).
Returns: vector of integer points without negative points.
*************************/
std::vector<cv::Point2i> RemoveNegativePoints(std::vector<cv::Point2i> points) {

    std::vector<cv::Point2i> cleanvector;
    for (int i = 0; i < points.size(); i++) {
        if (points[i].x < 0 || points[i].y < 0) {
            continue;
        }
        else {
            cleanvector.push_back(points[i]);
        }
    }


    return cleanvector;
}

/************************
Set values of pixels in image.
<infilename> corresponding to input image.
<outfilename> corresponds to output image.
<coordinates> has x, y and gray value to set and save in the output image.
Returns true if execution was correct.
*************************/
/* Set values of pixels of image .*/
bool SetValues(std::string infilename, std::string outfilename, std::vector<Coords>& coordinates) {
    std::cout << "Setting pixel gray values in file: " << infilename << "... ";
    MagickWand* mw = NULL;
    DrawingWand* dw = NULL;
    PixelWand* fill = NULL;
    MagickWandGenesis();
    /* Create a wand */
    mw = NewMagickWand();
    MagickSetType(mw, GrayscaleType);
    /* Read the input image */
    if (!MagickReadImage(mw, infilename.c_str())) {
        std::cerr << std::endl << "Could not open " << infilename << " image... Aborting." << std::endl;
        exit(0);
    }

    /* Pixel and drawing wands */
    fill = NewPixelWand();
    dw = NewDrawingWand();
    // Set the fill to "red" or you can do the same thing with this:
    // PixelSetColor(fill,"rgb(255,0,0)") or PixelSetColor(fill,"red");
    //PixelSetColor(fill,"rgb(100,101,100)");

    /* Pixel region iterator and pixel wand to get px input info */
    PixelIterator* iterator = NULL;
    PixelWand** pixel = NULL;
    size_t x;

    /*//Add this to list pixels and values in image corresponding to coordinates
    for(int i=0;i<coordinates.size();i++){
      iterator = NewPixelRegionIterator(mw,coordinates[i].x,coordinates[i].y,1,1);
      pixel=PixelGetNextIteratorRow(iterator,&x);
      std::cout<<i<<" "<<coordinates[i].x<<" "<<coordinates[i].y<<" "<<PixelGetRedQuantum(pixel[0])<<" "<<PixelGetGreenQuantum(pixel[0])<<" "<<PixelGetBlueQuantum(pixel[0])<<std::endl;
    }
    */

    Quantum q; //value to set in pixel
    int a, b;
    for (int i = 0; i < coordinates.size(); i++) {
        q = coordinates[i].v;
        PixelSetRedQuantum(fill, q);
        PixelSetGreenQuantum(fill, q);
        PixelSetBlueQuantum(fill, q);
        DrawSetFillColor(dw, fill);
        // Uses the current Fill as the colour of the point in coordinates
        DrawPoint(dw, coordinates[i].x, coordinates[i].y);
    }
    MagickDrawImage(mw, dw);
    /* write it */
    if (MagickWriteImage(mw, outfilename.c_str())) {
        std::cout << "wrote final file in " << outfilename << "... DONE." << std::endl;
    }
    else {
        std::cerr << "Couldn't write output file " << outfilename << "... ABORTING." << std::endl;
        exit(0);
    }

    /*//Add this to list pixels and values in image corresponding to coordinates
    for(int i=0;i<coordinates.size();i++){
      iterator = NewPixelRegionIterator(mw,coordinates[i].x,coordinates[i].y,1,1);
      pixel=PixelGetNextIteratorRow(iterator,&x);
      std::cout<<i<<" "<<coordinates[i].x<<" "<<coordinates[i].y<<" "<<PixelGetRedQuantum(pixel[0])<<" "<<PixelGetGreenQuantum(pixel[0])<<" "<<PixelGetBlueQuantum(pixel[0])<<std::endl;
    }
    */


    /* Tidy up */
    if (dw)dw = DestroyDrawingWand(dw);
    if (fill)fill = DestroyPixelWand(fill);
    if (mw) mw = DestroyMagickWand(mw);
    MagickWandTerminus();
    return true;
}
