/* 
 
OCR Image WordProcessor

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
1. read lineProcess information file
2. calculate the column black pixel for each line
3. detect words for each line
4. write word information file
 
*/

#include <Magick++.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>

#define WORD_TXT_FILE "wordProcessFile.txt"
#define IMAGE_FILE_NAME "wordProcess.png"
#define IMAGE_FILE_TYPE "PNG"

using namespace std;
using namespace Magick;

int main(int argc, char* argv[]) {
  
  if(argc == 1) {
    cout << "Err: Image File name missing" << endl;
    return 1;
  }
  if(argc == 2) {
    cout << "Err: Line Information File missing" << endl;
    return 1;
  }
  
  string sourceFile = argv[1];
  string lineInformationFile = argv[2];
  Image image;
  Image outImage;
  Image cropImage;
  
  try {
    int lineCount=0;
    //create txt file
    ofstream wordProcessFileTXT(WORD_TXT_FILE);
    ifstream lineProcessFileTXT(lineInformationFile.c_str());
    string line;

    // read image object
    image.read(sourceFile);
    outImage = image;
    cout << "\nWidthxHeight: " << image.columns() \
	 << "x" << image.rows() << endl;
    //read the line information
    if (lineProcessFileTXT.is_open()) {
      while (!lineProcessFileTXT.eof() ) {
	char *lineChar;
	int lineStartPositionY;
	int lineEndPositionY;
	
	getline (lineProcessFileTXT,line);
	
	if(line.size()>0) {
	  cout << "line: " << line << endl;
	  lineCount++;
	  //allocate space for lineChar
	  lineChar = (char *)malloc( ( (line.size()) * sizeof(char) ) + 1);
	  strcpy(lineChar, line.c_str());
	  lineStartPositionY = (int)atoi(strtok(lineChar, ","));
	  lineEndPositionY = (int)atoi(strtok(NULL, ";"));

	  cout << "lineStartPositionY: " << lineStartPositionY \
	       << " lineEndPositionY: " << lineEndPositionY;

	  //free the allocated buffer
	  free(lineChar);

	  int h = lineEndPositionY;
	  int w = image.columns();
	  unsigned int countWord = 0;
	  int x = 0;
	  int y = lineStartPositionY;
	  Color blackColor("black");
	  Color whiteColor("white"); 
	  int *rowsums = new int [w];
	  
	  cout << "\nStarting word segmentation";
	  //store the black pixel count in column in array
	  for (x=0; x < w; x++) {
	    rowsums[x] = 0;
	    for (y=lineStartPositionY; y < h; y++) {
	      // black count
	      rowsums[x] += !(ColorMono (image.pixelColor(x,y)).mono());
	    }
	  }
	  
	  //get the minimum value
	  int minimumValue;
	  minimumValue=rowsums[0];
	  for (x = 0; x < w; x++) {
	    if(minimumValue > rowsums[x]) {
	      minimumValue = rowsums[x];
	    }
	  }
	  cout << "\nMin value " << minimumValue << endl;
  	  
	  int startWordPixel=0;
	  int endWordPixel=0;
	  int startWordPixelFlag=0;
	  int endWordPixelFlag=0;
	  
	  /* padding before starting and \
	     end of and word */
	  int startAndEndPixelFix = 2;
	  //detect start and end of word
	  for (x = 1; x < w-1; x++) { 
	    //start of a word
	    if(rowsums[x]<=minimumValue && \
	       rowsums[x+1] > minimumValue) {
	      //draw a border
	      for (y = lineStartPositionY; y < h; y++) {
		outImage.pixelColor(x,y, blackColor);
	      }
	      //padding
	      startWordPixelFlag = x-startAndEndPixelFix;
	    }
	    //end of a word
	    if(rowsums[x] > minimumValue && \
	       rowsums[x+1] <= minimumValue) {
	      //draw a border
	      for (y = lineStartPositionY; y < h; y++) {
		outImage.pixelColor(x,y, blackColor);
	      }
	      // check for valid startWordPixelFlag 
	      if(startWordPixelFlag != 0) {
		//padding
		endWordPixelFlag = x+startAndEndPixelFix;
	      }
	      else {
		endWordPixelFlag = 0;
	      }
	    }
	    /* check for valid endWordPixel and \
	       startWordPixel combination */
	    if(startWordPixelFlag < endWordPixelFlag) {
	      // duplicate data check with previous value
	      if(endWordPixel != endWordPixelFlag) {
		startWordPixel = startWordPixelFlag;
		endWordPixel = endWordPixelFlag;
		countWord++;
		
		wordProcessFileTXT << lineStartPositionY << "," \
				   << lineEndPositionY << ":" \
				   << startWordPixel << "," \
				   << endWordPixel << ";"<< endl;
		cout << "start:" << startWordPixel \
		     << " end: " << endWordPixel << endl;
		
		/*
		//crop the word
		cropImage = image;
		cropImage.crop( Geometry((endWordPixel-startWordPixel),\
					 (lineEndPositionY-lineStartPositionY),\
					 startWordPixel, lineStartPositionY ));
		cropImage.magick(IMAGE_FILE_TYPE);
		
		//convert int to string
		std::stringstream out;
		out << lineCount << "_" << countWord;
		string name = "imagesWordProcess/" + out.str() + ".png";
		cropImage.write( name );
		*/
	      }
	    }
	  }
	  
	  outImage.magick(IMAGE_FILE_TYPE);
	  outImage.write(IMAGE_FILE_NAME);
	  
	  cout << "end of a line" << endl;
	}
      }
      lineProcessFileTXT.close();
      wordProcessFileTXT.close();
    }
    else cout << "Unable to open file";
  }
  catch( Exception &error_ ) {
    cout << "Caught exception: " << error_.what() << endl;
    return 1;
  }
  
  return 0;
}
