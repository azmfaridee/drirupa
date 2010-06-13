/* 

OCR Image PreProcessor
 
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
1. convert image to two color (black and white).
2. remove possible noise.
 
*/

#include <Magick++.h>
#include <iostream>

#define IMAGE_FILE_NAME "preProcess.png"
#define IMAGE_FILE_TYPE "PNG"
#define THRESHOLD 170
#define SCAN_ERR_FIX 4.5

using namespace std;
using namespace Magick;

int main(int argc, char* argv[]) {
  
  if(argc == 1) {
    cout << "Err: Image File name missing" << endl;
    return 1;
  }
  
  // image file name
  string sourceFile = argv[1];
  Image work;
  
  try {
    // read image object
    work.read(sourceFile);
    work.normalize();
    // seems not working
    work.monochrome();
    /* need to tune this value for different size 
       (create black and white) */
    work.threshold(THRESHOLD);
    
    cout << "WidthxHeight: " << work.columns() \
	 << "x" << work.rows() << endl;
    
    int height = work.rows();
    int width = work.columns();
    Color blackColor("black");
    Color whiteColor("white"); 
    int count = 0;
    
    work.quantizeColorSpace( GRAYColorspace );
    work.quantizeColors( 2 ); 
    work.quantize();
    work.colorSpace(GRAYColorspace);
    
    /* noise reduction algorithm copied 
       from http://bocra.sourceforge.net/
       pre-process the image */
    for (int y=1; y<height-1; y++) {
      for(int x=1; x<width-1; x++) {
	//return 1 for black and 0 for white
	count = !(ColorMono (work.pixelColor(x-1, y-1)).mono()) +
	  //calculate the region around a pixel
	  !(ColorMono (work.pixelColor(x  , y-1)).mono() )+ 
	  !(ColorMono (work.pixelColor(x+1, y-1)).mono() ) +
	  !(ColorMono (work.pixelColor(x  , y)).mono() ) + 
	  !(ColorMono (work.pixelColor(x-1, y)).mono() ) +
	  !(ColorMono (work.pixelColor(x+1, y)).mono() ) +
	  !(ColorMono (work.pixelColor(x-1, y+1)).mono() ) +
	  !(ColorMono (work.pixelColor(x  , y+1)).mono() )+
	  !(ColorMono (work.pixelColor(x+1, y+1)).mono() );
	
	// fix the minor scanning error
	if (count > SCAN_ERR_FIX) {
	  // check if that specific point is white
	  if ( (ColorMono (work.pixelColor(x,y)).mono() ) == 1) {
	    // set black
	    work.pixelColor(x, y, blackColor);
	  }
	}
	else {
	  if ( (ColorMono (work.pixelColor(x,y)).mono() ) == 0) {
	    work.pixelColor(x, y, whiteColor); 
	  }
	}
      }
    }
    work.magick(IMAGE_FILE_TYPE);
    work.write(IMAGE_FILE_NAME);
  }
  
  catch( Exception &error_ ) {
    cout << "Caught exception: " << error_.what() << endl;
    return 1;
  }
  
  return 0;
}
