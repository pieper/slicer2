/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSIO.h,v $
  Language:  C++
  Date:      $Date: 2003/02/19 23:55:11 $
  Version:   $Revision: 1.2 $

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
  static int ReadShort (FILE* iFile, short& oShort);
  static int ReadInt (FILE* iFile, int& oInt);
  static int ReadInt3 (FILE* iFile, int& oInt);
  static int ReadInt2 (FILE* iFile, int& oInt);
  static int ReadFloat (FILE* iFile, float& oFloat);

};

#endif
