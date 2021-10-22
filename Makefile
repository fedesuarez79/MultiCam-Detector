# Makefile to compile multi_cam.cc
CC = g++

CFLAGS = -I/usr/local/opt/imagemagick@6/include/ImageMagick-6 -I/usr/local/Cellar/opencv@2/2.4.13.7_12/include/opencv -I/usr/local/Cellar/opencv@2/2.4.13.7_12/include -L/usr/local/Cellar/opencv@2/2.4.13.7_12/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab


LINKER = -L/usr/local/opt/imagemagick@6/lib -lMagickWand-6.Q16 
SOURCE = multi_cam
TARGET = multi_cam
OBJECTS = $(SOURCE).o

all: $(TARGET)

$(OBJECTS): $(SOURCE).cpp
	$(CC) $(CFLAGS) -c $(SOURCE).cpp

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LINKER) $(OBJECTS) -o $(TARGET)

clean:
	rm -rf *.o $(TARGET)
