/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSIO.cxx,v $
  Language:  C++
  Date:      $Date: 2002/10/07 21:08:44 $
  Version:   $Revision: 1.1 $

=========================================================================*/

#include "vtkFSIO.h"
#include "vtkByteSwap.h"

int vtkFSIO::ReadInt3 (FILE* iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read three bytes. Swap if we need to. Stuff into a full sized int
  // and return.
  result = fread (&i, 3, 1, iFile);
  vtkByteSwap::Swap4BE (&i);
  oInt = ((i>>8) & 0xffffff);

  return result;
}

int vtkFSIO::ReadInt2 (FILE* iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read two bytes. Swap if we need to. Return the value
  result = fread (&i, 2, 1, iFile);
  vtkByteSwap::Swap4BE (&i);
  oInt = i;
  
  return result;
}

int vtkFSIO::ReadFloat (FILE* iFile, float& oFloat) {

  float f = 0;
  int result ;

  // Read a float. Swap if we need to. Return the value
  result = fread (&f, 4, 1, iFile);
  vtkByteSwap::Swap4BE (&f);
  oFloat = f;
  
  return result;
}



