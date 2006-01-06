/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkFSIO.h,v $
  Date:      $Date: 2006/01/06 17:57:41 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSIO.h,v $
  Language:  C++
  Date:      $Date: 2006/01/06 17:57:41 $
  Version:   $Revision: 1.6 $

=========================================================================*/
// .NAME vtkFSIO - Some IO functions for irregular FreeSurface files.
// .SECTION Description
// Some simple functions for doing silly things like reading three
// byte ints, common in FreeSurfer file types.

#ifndef __vtkFSIO_h
#define __vtkFSIO_h

#include <stdio.h>
#include <zlib.h>


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

  static int ReadShortZ (gzFile iFile, short& oShort);
  static int ReadIntZ (gzFile iFile, int& oInt);
 static int ReadInt3Z (gzFile iFile, int& oInt);
 static int ReadInt2Z (gzFile iFile, int& oInt);
  static int ReadFloatZ (gzFile iFile, float& oFloat);
};

#endif
