// .NAME vtkIbrowserIO - Some IO functions 
// .SECTION Description
// Some simple functions for like reading three
// byte ints, common in FreeSurfer file types.

#ifndef __vtkIbrowserIO_h
#define __vtkIbrowserIO_h

#include <stdio.h>

class  vtkIbrowserIO {
 public:

  static vtkIbrowserIO *New () { return NULL; }

  static int ReadShort (FILE* iFile, short& oShort);
  static int ReadInt (FILE* iFile, int& oInt);
  static int ReadInt3 (FILE* iFile, int& oInt);
  static int ReadInt2 (FILE* iFile, int& oInt);
  static int ReadFloat (FILE* iFile, float& oFloat);

 protected:
  vtkIbrowserIO ( ) { }
  ~vtkIbrowserIO ( ) { }

};

#endif
