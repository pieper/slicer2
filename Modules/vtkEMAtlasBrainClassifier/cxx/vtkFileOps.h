/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
#ifndef __vtkFileOps_h
#define __vtkFileOps_h

#include "vtkObject.h"
#include <vtkEMAtlasBrainClassifierConfigure.h>
    
class VTK_EMATLASBRAINCLASSIFIER_EXPORT vtkFileOps { //; prevent man page generation
  public:
  static vtkFileOps *New() {return (new vtkFileOps);}

  // Note if varname = NULL it will just write the data in the file
  void WriteVectorMatlabFile (char *filename, char *name,unsigned char *vec, int xMax) const;
  void WriteVectorMatlabFile (char *filename, char *varname,float *vec, int xMax) const;
  void WriteVectorMatlabFile (char *filename, char *varname,double *vec, int xMax) const;
  void WriteMatrixMatlabFile (char *filename,char *varname, double **mat, int imgY, int imgX) const;

  // ----------------------------------------------
  // Kilian: Old Stuff - I think you can take all of this out  
  // GE Format
  int  WriteMRIfile(char *fname, unsigned char *header, int headersize, short *data, int npixels);
  // Transforms a double vec (with values between min and max) into an unsigned short file where min is 0 and max is MaxOutput
  // int XSize, int YSize are only important if FlagUpsideDown is set 
  void WriteDoubleToUShortToGEFile(char* FileName, double* vec, int XSize, int YSize, int XYSize , double min, double max, unsigned short MaxOutput,  bool FlagUpsideDown) const; 
  // Does not work correclty
  int  WriteMRIfile(char *fname, double *data, int np);
  //  End of old stuff
  // ----------------------------------------------

  // -------------------------------
  // Write MRI Functions from fileops.c
  // -------------------------------
  static int makeDirectoryIfNeeded(char *fname);
  static char* pathComponent(char *fname);

protected:
  void WriteVectorMatlabFile (FILE *f,char *name, double *vec, int xMax) const;
  void WriteMatrixMatlabFile (FILE *f,char *name,double **mat, int imgY, int imgX) const;

  void ensureGEByteOrderForShort(short *data, int np);
  int IsMSBFirstForShort(void);
  short convertShortFromGE(short ge);
  int fileIsCompressed(char *fname, char **newFileName);
  int uncompressedFileName(char *fname, char **newFileName);
};

//BTX

template <class T>  
static T MaxValue(T *vec, int size) {
  T result = *vec;
  for (int i = 1; i < size; i++) {
    if (result < *vec ) result = *vec;
    vec++;
  }
  return result;
}

template <class Tin, class Tout> 
static void TransferDataFormat(Tin *vec_in, Tout *vec_out, int Size, Tout max_out) {
  Tin max_in = MaxValue(vec_in, Size);
  Tin quote  = Tin(max_out) / (max_in != 0 ? max_in : 1.0);
  for (int i = 0; i < Size; i++) {
    vec_out[i] = Tout((vec_in[i])*quote);
  }
}
 

template <class T> 
void FlipXAxis(T *invec, T *outvec, int XSize, int YSize, int XYSize) {
  // Flip around the X-Axis
  invec += XYSize;
  for (int y = 0; y < YSize; y++) {
    invec -= XSize;
    memcpy(outvec,invec,sizeof(T)*XSize);
    outvec += XSize;
  }
}

// Kilian - Fixed function in August 03 - works now correctly 
// Flip around the Y Axis
// vec += XSize;
// for (y = 0; y < YSize; y++) {
//  if (y) vec += 2*XSize;
//  for (x = 0; x < XSize; x++) *res++ = *vec--;
// }
// res -= XYSize;

template <class T> 
static void vtkFileOps_WriteToFlippedGEFile(char *filename,T *vec, int XSize, int YSize, int XYSize) {
  T* res = new T[XYSize];
  FlipXAxis(vec,res,XSize,YSize,XYSize);
  WriteToGEFile(filename,res,XYSize);
  delete[] res;
}

// Opens up a new file and writes down result in the file
template <class T> 
static int WriteToGEFile(char *filename,T *vec, int size) {
  int appendFlag = 0;
  // If you enter - as name => prints it on the screen
  FILE *f = (strcmp(filename,"-")) ? fopen(filename,((appendFlag)?"ab":"wb")):stdout;
  if ( f == NULL ) {
      //cerr << "Could not open file " << filename << "\n";
      fprintf(stderr, "Could not open file %s\n", filename);
    return 0;
  }
  // Cannot individually do fwrite for each element - > makes a double out of it 
  fwrite(vec, sizeof(T), size, f);
  fflush(f);
  fclose(f);
  return 1;
}
//ETX
#endif
