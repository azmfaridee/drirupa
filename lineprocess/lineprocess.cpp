/*
 
OCR Image LineProcessor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 

Copyright:
salahuddin pasha (salahuddin66) <salahuddin66@gmail.com>

--- Algorithm ---
1. calculate the row black pixel
2. detect start and end of the line
3. write the line information file

*/

#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define LINE_TXT_FILE "lineProcessFile.txt"
#define IMAGE_FILE_NAME "lineProcess.png"
#define IMAGE_FILE_TYPE "PNG"

using namespace std;
using namespace Magick;

int main(int argc, char* argv[]) {
  
  if(argc == 1) {
    cout << "Err: Image File name missing" << endl;
    return 1;
  }

  InitializeMagick(*argv);

  //image file name
  string sourceFile = argv[1];
  Image image;
  Image cropImage;
  ofstream lineProcessFileTXT(LINE_TXT_FILE);
 
  try {
    //read image object
    image.read(sourceFile);

    int h = image.rows();
    int w = image.columns();
    unsigned int countLine = 0;
    int x = 0;
    int y = 0;
    //generate black color
    Color blackColor("black");
    Color whiteColor("white");
    int *rowsums = new int [h];
    
    cout << "WidthxHeight: " << image.columns() \
	 << "x" << image.rows() << endl;    
    cout << "Starting segmentation" << endl;
    
    /* store black pixel count for
       each row in rowsums array */
    for (y = 0; y < h; y++) { 
      rowsums[y] = 0;
      for (x = 0; x < w; x++) {
	// black count
	rowsums[y] += !(ColorMono (image.pixelColor(x,y)).mono());
      }
    }
    
    /* get the minimum value
       assume minimum value is the 
       free space between the lines */
    int minimumValue;
    minimumValue=rowsums[0];
    for (y = 0; y < h; y++) { 
      if(minimumValue > rowsums[y]) {
	minimumValue = rowsums[y];
      }
    }
    cout << "Minimum Value: " << minimumValue << endl;
  
    int startLinePixel=0;
    int endLinePixel=0;
    int startLinePixelFlag=0;
    int endLinePixelFlag=0;
    
    /* detect start and end of line
     skipping 1 pixel for image border */
    for (y = 1; y < h-1; y++) {
      //start of a line
      if(rowsums[y]<=minimumValue && \
	 rowsums[y+1] > minimumValue) {
	//draw a border
	for (x = 0; x < w; x++) {
	  image.pixelColor(x,y, blackColor);
	}
	startLinePixelFlag = y;
      }
      //end of a line
      if(rowsums[y] > minimumValue && \
	 rowsums[y+1] <= minimumValue) {
	//draw a border
	for (x = 0; x < w; x++) {
	  image.pixelColor(x,y, blackColor);
	}
	//check for valid startLinePixelFlag
	if(startLinePixelFlag != 0) {  
	  endLinePixelFlag = y;
	}
	else {
	  endLinePixelFlag = 0;
	}
      }
      /* check for valid endLinePixel
	 and startLinePixel combination */
      if(startLinePixelFlag < endLinePixelFlag) {
	// duplicate data check with previous value
	if(startLinePixel != startLinePixelFlag && \
	   endLinePixel != endLinePixelFlag) {
	  startLinePixel = startLinePixelFlag;
	  endLinePixel = endLinePixelFlag;
	  countLine++;

	  lineProcessFileTXT <<startLinePixel << "," \
			     << endLinePixel << ";" << endl;
	  cout << "start: " << startLinePixel << " end: " \
	       << endLinePixel << endl;
	  
	  /*
	  //crop individual line
	  cropImage = image;
	  cropImage.crop( Geometry(w, (endLinePixel-startLinePixel), \
				   0, startLinePixel));
	  cropImage.magick(IMAGE_FILE_TYPE);
	  //convert int to string
	  std::stringstream out;
	  out << countLine;
	  string name = "imagesLineProcess/" + out.str() + ".png";
	  cropImage.write( name );
	  */
	}
      }
    }
    cout << "count line: " << countLine << endl;
    lineProcessFileTXT.close();
 
    image.magick(IMAGE_FILE_TYPE);
    image.write(IMAGE_FILE_NAME);
  }
  catch( Exception &error_ ) {
    cout << "Caught exception: " << error_.what() << endl;
    return 1;
  }
  
  return 0;
}
