/* 

OCR Image AlphabetProcessor

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
1. read the word information file
2. calculate the line pixel information
3. detect the main line for each word
4. temporarely make the top side of the longest line (matra) white, including matra, ekar, oikar etc.
5. detect ukar, drugukar etc.
6. temporarely remove lower part from before ukar, grugukar etc.
7. separate individual alphabet
8. return the longest line (matra)
9. return the top part ekar, oikar etc.
10. return the lower part  ukar, drugukar etc.

*/


#include <Magick++.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>

#define IMAGE_FILE_NAME "alphabetProcess.png"
#define IMAGE_FILE_TYPE "PNG"

using namespace std;
using namespace Magick;

int main(int argc, char* argv[]) {
  
  if(argc == 1) {
    cout << "Err: File name missing" << endl;
    return 1;
  }
  if(argc == 2) {
    cout << "Err: Word Information File missing" << endl;
    return 1;
  }

  InitializeMagick(*argv);

  string sourceFile = argv[1];
  string wordInformationFile = argv[2];
  //source image
  Image image;
  //work image
  Image work;
  Image cropImage;
  
  try {
    int lineCount=0;
    string line;
    ifstream myfile (wordInformationFile.c_str());
    // read image object
    image.read(sourceFile);
    work = image;
    cout << "\nWidthxHeight: " << image.columns() \
	 << "x" << image.rows();

    if (myfile.is_open()) {
      while (!myfile.eof() ) {
	char *lineChar;					
	int lineStartPositionY;
	int lineEndPositionY;
	int wordStartPositionX;
	int wordEndPositionX;					
	
	getline (myfile,line);

	if(line.size() > 0 ) {
	  cout << line << endl;
	  lineCount++;
	  //allocate space for lineChar
	  lineChar = (char *)malloc( ( (line.size()) * sizeof(char) ) + 1);
	  strcpy(lineChar, line.c_str());
	  
	  lineStartPositionY = (int)atoi(strtok(lineChar, ","));
	  lineEndPositionY = (int)atoi(strtok(NULL, ":"));
	  wordStartPositionX = (int)atoi(strtok(NULL, ","));
	  wordEndPositionX = (int)atoi(strtok(NULL, ";"));
	  
	  cout << "lineStartPositionY " << lineStartPositionY \
	       << " lineEndPositionY " << lineEndPositionY \
	       << " wordStartPositionX " << wordStartPositionX \
	       << " wordEndPositionX " << wordEndPositionX << endl;
	  
	  //free the allocated buffer
	  free(lineChar);

	  int h = lineEndPositionY-lineStartPositionY;
	  int w = wordEndPositionX-wordStartPositionX;
	  int x = 0;
	  int y = 0;
	  //generate black color
	  Color blackColor("black");
	  Color whiteColor("white"); 
	  
	  /* to return the top line (matra) 
	     of word after calculation */
	  int *lineH = new int [h];
	  int *lineW = new int [w];
	  //for storing space between alphabet
	  int *lineD = new int [w];
 
	  //set initial value
	  for (y = 0; y < h; y++) {
	    lineH[y] = 0;
	  }
	  for (x = 0; x < w; x++) { 
	    lineW[x] = 0;
	  }
	  for (x = 0; x < w; x++) { 
	    lineD[x] = 0;
	  }
	  
	  int *rowsumsH = new int [h];
	  int *rowsumsW = new int [w];
	  
	  cout << "Starting segmentation" << endl;

	  /* store each row black pixels count in 
	     rowsumsH array */
	  for (y=0; y < h; y++) {
	    rowsumsH[y] = 0;
	    for (x = 0; x < w; x++) {
	      // black count
	      rowsumsH[y] += !(ColorMono (work.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY))).mono());
	    }
	  }
	  
	  int maxValue;
	  maxValue=rowsumsH[0];
	  /* assume the the line will be
	     the max value */
	  for (y = 0; y < h; y++) { 
	    if(maxValue <= rowsumsH[y]) {
	      maxValue = rowsumsH[y];
	    }
	  }
	  cout << "Maximum Value: " << maxValue << endl;
	  
	  /* hold the lowest pixel
	     containg longest line */
	  int lineLastPosition=0;
	  /* hold the top pixel
	     containg longest line */
	  int lineFirstPosition = -1;
	  	  
	  //detect start and end of line
	  for (y = 0; y < h; y++) {
	    /* make the longest line (matra)
	       temporary white color */
	    if(rowsumsH[y]>=maxValue-5) {
	      for (x = 0; x < w; x++) {
		/* backup the longest line (matra)
		   information in lineH and lineW
		   variable */
		if(!(ColorMono (work.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY))).mono()) ) {
		  lineH[y] = 1;
		  lineW[x] = 1;
		}
		//set white color
		work.pixelColor((x+wordStartPositionX), \
				(y+lineStartPositionY), \
				whiteColor);
	      }
	      cout << "Location: " << y << endl;
	      
	      //we need only first pixel
	      if(lineFirstPosition == -1){ 
		lineFirstPosition = y;
	      }
	      /* the last pixel of longest
		 line (matra) */
	      lineLastPosition = y;						
	    }
	  }
	  
	  /* temporearely removing ekar, 
	     oikar upper symbom above the
	     longest line (matra).
	     we need to make white from top (y=0) */
	  for (y = 0; y < lineFirstPosition; y++) {
	    for (x =0; x < w; x++) {
	      work.pixelColor( (x+wordStartPositionX), \
			       (y+lineStartPositionY), \
			       whiteColor);
	    }
	  }
	  
	  /* caluculating first black pixel
	     from left and right side of
	     the word to detect the ukar,drghu-kar */
	  int *rowsumsStart = new int [h];
	  int *rowsumsEnd = new int [h];
	  int *rowsumsDistanceStartEnd = new int [h];
	  
	  //set initial value
	  for (y=0; y < h; y++) {
	    rowsumsStart[y] = 0;
	    rowsumsEnd[y] = 0;
	  }

	  /* calculate the first black
	     pixel from left */
	  for (y =0; y < h; y++) {
	    for (x =0; x < w; x++) {
	      if ((!(ColorMono (work.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY) )).mono())) == 1) {
		rowsumsStart[y] = x;
		break; //any other good option
	      }
	    }

	    /* calculate the first black
	       pixel from right */
	    for (x = w; x > 0; x--) {
	      if ((!(ColorMono (work.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY) )).mono())) == 1) {
		rowsumsEnd[y] = x;
		break; //any other good option
	      }
	    }

	    rowsumsDistanceStartEnd[y] =  rowsumsEnd[y] - rowsumsStart[y];
	    cout << "Distance: " << rowsumsEnd[y] - rowsumsStart[y] \
		 << " location: " << y << endl;
	  }
	  
	  /*to detect the ukar,
	    drughu kar position */
	  int minDistance;
	  /* pixel over y-axis before
	     starting ukar to make 
	     temporarely wite */
	  int endOfLine=0;
	  
	  /* checking from below longest line (matra) 
	     add 1 with lineLastPosition value to 
	     avoid longest line (matra) */
	  minDistance=rowsumsDistanceStartEnd[lineLastPosition+1]; 
	  /* to avoid ukar, drugukar lowest 
	     part in the mixDistance 
	     (h-2) */
	  for (y = (lineLastPosition+1); y < h-2; y++) {
	    if(minDistance >= rowsumsDistanceStartEnd[y]) {
	      minDistance = rowsumsDistanceStartEnd[y];
	      endOfLine = y;
	    }
	  }
	  //the location of ukar, drughukar
	  cout << "Min Distance: " << minDistance << endl;
	  	  
	  /* temporearely removing uker,grugukar etc. 
	     lower symbom below the line */
	  for (y =endOfLine; y < h; y++) {
	    for (x =0; x < w; x++) {
	      work.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY), whiteColor);
	    }
	  }
	  	  
	  /* scanning over x-axis
	     create some gap between longest 
	     line where it seems different word */
	  for (x = 0; x < w; x++) {
	    rowsumsW[x] = 0;
	    for (y =0; y < h; y++) {
	      // black count
	      rowsumsW[x] += !(ColorMono (work.pixelColor( (x+wordStartPositionX) , (y+lineStartPositionY) )).mono());
	    }
	  }
	  
	  int minValue;
	  //fix the min val by
	  int minValueFix = 2;
	  minValue=rowsumsW[0];
	  /* let assume minimum value is 
	     the free space between the lines */
	  for (x = 0; x < w; x++) {
	    if(minValue > rowsumsW[x]) {
	      minValue = rowsumsW[x];
	    }
	  }
	  //fix calculated min val
	  minValue = minValue + minValueFix;
	  
	  int startAlphabetPixel=0;
	  int endAlphabetPixel=-1;
	  int startAlphabetPixelFlag=0;
	  int endAlphabetPixelFlag=-1;
	  unsigned int countAlphabet = 0;

	  //start and end of alphabet
	  for (x = 0; x < w-1; x++) {
	    //start of alphabet
	    if(rowsumsW[x]<=(minValue) &&	\
	       rowsumsW[x+1]>(minValue)) { 
	      /* //draw a line
	      for (y = 0; y < h; y++) {
		work.pixelColor( (x+wordStartPositionX) ,		\
				 (y+lineStartPositionY), blackColor);
	      }
	      cout << "Location vertical: " << x << endl; */
	      startAlphabetPixelFlag = x;
	    }

	    //end of alphabet
	    if(rowsumsW[x]>(minValue) && \
	       rowsumsW[x+1]<=(minValue)) {
	      /* //draw a line
	      for (y = 0; y < h; y++) {
		work.pixelColor((x+wordStartPositionX) , \
				(y+lineStartPositionY), blackColor);
	      } */    

	      //check for valid endAlphabetPixelFlag
	      if(startAlphabetPixelFlag != 0) {
		endAlphabetPixelFlag = x;
	      }
	    }
	    else {
	      //nothing
	    }
	    
	    /* check for valid endAlphabetPixel
	       and startAlphabetPixel combination */
	    if(startAlphabetPixelFlag < endAlphabetPixelFlag) { 
	      /* return top longest line (matra )
		 for alphabet */
	      for(int p=startAlphabetPixelFlag; \
		  p<endAlphabetPixelFlag; p++) { 
		lineD[p] = 1;
	      }
	      //check for dupliacate with previous value
	      if(startAlphabetPixel != startAlphabetPixelFlag && \
		 endAlphabetPixel != endAlphabetPixelFlag) {
		
		/* create gap between two alphabet
		   by makeing the longest line (matra)
		   white color
		   (present one start - previous one end pixel)*/
		if(endAlphabetPixel > 0) {
		  for(int i=endAlphabetPixel; \
		      i<startAlphabetPixelFlag; i++) {
		    for(int j=0; j<h; j++) {
		      cout << "I " << i  << " J " << j << endl;
		      work.pixelColor( (i+wordStartPositionX), \
				       (j+lineStartPositionY), \
				       whiteColor);
		    }
		  }
		}
		
		startAlphabetPixel = startAlphabetPixelFlag;
		endAlphabetPixel = endAlphabetPixelFlag;
		countAlphabet++;
		
		//crop the alphabet
		cropImage = image;
				
		cropImage.crop(Geometry((endAlphabetPixel-startAlphabetPixel),\
					(lineEndPositionY-lineStartPositionY), \
					(wordStartPositionX+startAlphabetPixel),\
					lineStartPositionY));
		
		int cropImageWidth = endAlphabetPixel-startAlphabetPixel;
		int a,b;

		/* set white color around
		   the crop image border */
		for (b = lineStartPositionY; \
		     b < (lineEndPositionY-lineStartPositionY); \
		     b++) {
		  cropImage.pixelColor(0, b, whiteColor);
		  cropImage.pixelColor(cropImageWidth-1, b, whiteColor);
		}
		for (a=wordStartPositionX; a<cropImageWidth; a++) {
		  cropImage.pixelColor(a, 0, whiteColor);
		  cropImage.pixelColor(a, h-1, whiteColor);
		}
		
		/*
		cropImage.magick(IMAGE_FILE_TYPE);
		//convert int to string
		std::stringstream out;
		out << lineCount << "_" << countAlphabet;
		string name = "imagesAlphabetProcess/" + out.str() + ".png";
		cropImage.write( name );
		*/
	      }
	    }
	  }
	  
	  /* return the top longest line (matra)
	     avoiding the gap between alphabet (lineD) */
	  for (y = 0; y < h; y++) {
	    for (x = 0; x < w; x++) { 
	      if(lineW[x] == 1 && lineH[y] == 1 && lineD[x] == 1) {
		work.pixelColor( (x+wordStartPositionX), \
				 (y+lineStartPositionY), \
				 blackColor);
	      }
	    }
	  }
	  
	  //return the top symbom ekar, oikar
	  for (y = 0; y < lineFirstPosition-2; y++) {
	    for (x = 0; x < w; x++) { 
	      if((!(ColorMono (image.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY) )).mono())) == 1) {
		work.pixelColor( (x+wordStartPositionX), \
				 (y+lineStartPositionY), \
				 blackColor);
	      }
	    }
	  }
	  
	  //return the bottom symbol ukar, drugukar
	  for (y = endOfLine; y <h ; y++) {
	    for (x = 0; x < w; x++) { 
	      if((!(ColorMono (image.pixelColor( (x+wordStartPositionX), (y+lineStartPositionY) )).mono())) == 1) {
		work.pixelColor( (x+wordStartPositionX), \
				 (y+lineStartPositionY), \
				 blackColor);
	      }
	    }
	  }
	  	  
	  /* std::stringstream out;  //convert int to string
	    out << lineCount;
	    string name = "alphabet_" + out.str() + ".png"; */
	  
	  work.magick(IMAGE_FILE_TYPE);
	  cout << work.rows() << " " << work.columns() << endl;
	  work.write(IMAGE_FILE_NAME);
	  	  
	  free(lineH);
	  free(lineW);
	  free(lineD);
	  free(rowsumsStart);
	  free(rowsumsEnd);
	  free(rowsumsDistanceStartEnd);
	}
      }
      myfile.close();
      
    }
    else cout << "Unable to open file"; 
  }
  catch( Exception &error_ ) {
    cout << "Caught exception: " << error_.what() << endl;
    return 1;
  }
  
  return 0;
}
