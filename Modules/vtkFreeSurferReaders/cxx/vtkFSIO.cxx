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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSIO.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/30 21:51:20 $
  Version:   $Revision: 1.4 $

=========================================================================*/

#include "vtkFSIO.h"
#include "vtkByteSwap.h"

int vtkFSIO::ReadShort (FILE* iFile, short& oShort) {

  short s = 0;
  int result ;

  // Read an short. Swap if we need to. Return the value.
  result = fread (&s, sizeof(short), 1, iFile);
  vtkByteSwap::Swap2BE (&s);
  oShort = s;

  return result;
}

int vtkFSIO::ReadShortZ (gzFile iFile, short& oShort) {

  short s = 0;
  int result ;

  // Read an short. Swap if we need to. Return the value.
  result = gzread (iFile, &s, sizeof(short));
  vtkByteSwap::Swap2BE (&s);
  oShort = s;

  return result;
}

int vtkFSIO::ReadInt (FILE* iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read an int. Swap if we need to. Return the value.
  result = fread (&i, sizeof(int), 1, iFile);
  vtkByteSwap::Swap4BE (&i);
  oInt = i;

  return result;
}

int vtkFSIO::ReadIntZ (gzFile iFile, int& oInt) {
  int i = 0;
  int result;

  result = gzread(iFile, &i, sizeof(int));
  vtkByteSwap::Swap4BE(&i);
  oInt = i;
  return result;
}

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

int vtkFSIO::ReadInt3Z (gzFile iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read three bytes. Swap if we need to. Stuff into a full sized int
  // and return.
  result = gzread (iFile, &i, 3*sizeof(int));
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

int vtkFSIO::ReadInt2Z (gzFile iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read two bytes. Swap if we need to. Return the value
  result = gzread (iFile, &i, 2*sizeof(int));
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

int vtkFSIO::ReadFloatZ (gzFile iFile, float& oFloat) {

  float f = 0;
  int result ;

  // Read a float. Swap if we need to. Return the value
  result = gzread (iFile, &f, sizeof(float));
  vtkByteSwap::Swap4BE (&f);
  oFloat = f;
  
  return result;
}
