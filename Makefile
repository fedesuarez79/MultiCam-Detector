# Makefile to compile multi_cam.cc
CXX = clang++

CXXFLAGS = -I/usr/local/opt/imagemagick@6/include/ImageMagick-6 -I/usr/local/Cellar/opencv@2/2.4.13.7_12/include/opencv -I/usr/local/Cellar/opencv@2/2.4.13.7_12/include
# -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect-lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui 

LDFLAGS =  -L/usr/local/Cellar/opencv@2/2.4.13.7_12/lib -lopencv_imgproc -lopencv_highgui -lopencv_core -L/usr/local/opt/imagemagick@6/lib -lMagickWand-6.Q16 -lMagickCore-6.Q16
ADDS = -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16
SOURCE = multi_cam
TARGET = multi_cam
OBJECTS = $(SOURCE).o

all: $(TARGET)

$(OBJECTS): $(SOURCE).cpp
	$(CXX) $(CXXFLAGS) $(ADDS) -c $(SOURCE).cpp

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -rf *.o $(TARGET)
