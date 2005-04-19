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
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGCifstream.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2001
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:25 $
  Version   : $Revision: 1.2 $

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
=========================================================================*/
// .NAME vtkCISGCifstream - class for reading compressed file streams.
//
// .SECTION Description
// This class defines and implements functions for reading compressed file
// streams. The file streams can be either uncompressed or must be compressed
// with the UNIX compress(1) command. The original code has been written by
// Daniel Rueckert. vtkCISGCifstream is derived from ifstream.
//
// .SECTION see also
// vtkCISGCofstream.h

#ifndef _CIFSTREAM_H

#define _CIFSTREAM_H

#include <vtkCISGFileConfigure.h>

#include <vtkObject.h>
#ifdef WIN32
#include <fstream>     
#else
#include <fstream.h>     
#endif
#include <stdlib.h>

#define True 1
#define False 0

class  VTK_CISGFILE_EXPORT vtkCISGCifstream : public ifstream {

  // Description:
  // Read one code from the standard input. If EOF, return -1.
  long int getcode(int);

protected:

  // Description:
  // Flag whether file is compressed
  int _compressed;

  // Description:
  // Flag whether file is swapped
  int _swapped;

  // Description:
  // Swap 16 bit data
  void swap16(char *a, char *b, int n);

  // Description:
  // Swap 32 bit data
  void swap32(char *a, char *b, int n);

  // Description:
  // Swap 64 bit data
  void swap64(char *a, char *b, int n);

  // Description:
  // Checks whether swapping is required.
  int swapping();
  
public:

  // Description:
  // Read n data as array (possibly compressed) from offset
  int  CRead(char *data, int length, int offset);

  // Description:
  // Read n data as array of char (possibly compressed) from offset
  int CReadAsChar  (char *data, int length, int offset);

  // Description:
  // Read n data as array of unsigned char (possibly compressed) from offset
  int CReadAsUChar (unsigned char  *data, int length, int offset);

  // Description:
  // Read n data as array of short (possibly compressed) from offset
  int CReadAsShort (short *data, int length, int offset);

  // Description:
  // Read n data as array of unsigned short (possibly compressed) from offset
  int CReadAsUShort(unsigned short *data, int length, int offset);

  // Description:
  // Read n data as array of short (possibly compressed) from offset
  int CReadAsInt   (int *data, int length, int offset);

  // Description:
  // Read n data as array of unsigned short (possibly compressed) from offset
  int CReadAsUInt  (unsigned int *data, int length, int offset);

  // Description:
  // Read n data as array of short (possibly compressed) from offset
  int CReadAsFloat (float *data, int length, int offset);

  // Description:
  // Read n data as array of unsigned short (possibly compressed) from offset
  int CReadAsDouble(double *data, int length, int offset);

  // Description:
  // Read n data as string (possibly compressed) from offset
  int  CReadAsString(char *data, int length, int offset);

  // Description:
  // Open file (inline)
  void Open(char *);

  // Description:
  // Close file (inline)
  void Close();

  // Description:
  // Returns whether file is compressed (inline)
  int  IsCompressed();

  // Description:
  // Returns whether file is swapped (inline)
  int  IsSwapped();

  // Description:
  // Flips swapping member (inline)
  void FlipSwapping();
};


inline void vtkCISGCifstream::Close()
{
  this->close();
}

inline int vtkCISGCifstream::IsCompressed()
{
  return _compressed;
}

inline int vtkCISGCifstream::IsSwapped()
{
  return _swapped;
}

inline void vtkCISGCifstream::FlipSwapping() {
  if (_swapped) {
    _swapped = 0;
  } else {
    _swapped = 1;
  }
}

#endif
