/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSIO.h,v $
  Language:  C++
  Date:      $Date: 2002/10/07 21:08:44 $
  Version:   $Revision: 1.1 $

=========================================================================*/
// .NAME vtkFSIO - Some IO functions for irregular FreeSurface files.
// .SECTION Description
// Some simple functions for doing silly things like reading three
// byte ints, common in FreeSurfer file types.

#ifndef __vtkFSIO_h
#define __vtkFSIO_h

#include <stdio.h>

class  vtkFSIO {
 public:

  static vtkFSIO *New () { return NULL; }

  // These use FILE types instead of file streams for no good reason,
  // simply because the old code from which this is adapted is
  // C-based.
  static int ReadInt3 (FILE* iFile, int& oInt);
  static int ReadInt2 (FILE* iFile, int& oInt);
  static int ReadFloat (FILE* iFile, float& oInt);

};

#endif
