/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#ifndef __vtkFileOps_h
#define __vtkFileOps_h

#include "vtkObject.h"
#include <vtkEMLocalSegmentConfigure.h>

class VTK_EMLOCALSEGMENT_EXPORT vtkFileOps { //; prevent man page generation
  public:
  static vtkFileOps *New() {return (new vtkFileOps);}

  // Note if varname = NULL it will just write the data in the file
  void WriteVectorMatlabFile (char *filename, char *name,unsigned char *vec, int xMax) const;
  void WriteVectorMatlabFile (char *filename, char *varname,float *vec, int xMax) const;
  void WriteVectorMatlabFile (char *filename, char *varname,double *vec, int xMax) const;
  void WriteMatrixMatlabFile (char *filename,char *varname, double **mat, int imgY, int imgX) const;

  // GE Format
  int  WriteMRIfile(char *fname, unsigned char *header, int headersize, short *data, int npixels);
  // Transforms a double vec (with values between min and max) into an unsigned short file where min is 0 and max is MaxOutput
  // int XSize, int YSize are only important if FlagUpsideDown is set 
  void WriteDoubleToUShortToGEFile(char* FileName, double* vec, int XSize, int YSize, int XYSize , double min, double max, unsigned short MaxOutput,  bool FlagUpsideDown) const; 
  // Does not work correclty
  int  WriteMRIfile(char *fname, double *data, int np);

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
static void WriteToFlippedGEFile(char *filename,T *vec, int XSize, int YSize, int XYSize) {
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
    cerr << "Could not open file " << filename << "\n";
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
