/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGCofstream.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2001
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:35 $
  Version   : $Revision: 1.1 $

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
// .NAME vtkCISGCofstream - class for writing file streams.
//
// .SECTION Description
// This class defines and implements functions for writing file
// streams. The original code has been written by Daniel
// Rueckert. vtkCISGCofstream is derived from ofstream.
//
// .SECTION Caveats
// At the moment, only the writing of uncompressed file streams is supported.
//
// .SECTION see also
// vtkCISGCifstream.h

#ifndef _COFSTREAM_H
#define _COFSTREAM_H

#include <vtkCISGFileConfigure.h>

// system includes
#include <vtkObject.h>
#ifdef WIN32
#include <fstream>     
#else
#include <fstream.h>     
#endif
#include <stdlib.h>

#define True  1
#define False 0

class  VTK_CISGFILE_EXPORT vtkCISGCofstream : public ofstream 
{ 
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
  // Write n data as array from offset
  int Write(char *data, int offset, int length);

  // Description:
  // Write data as char (possibly compressed) from offset
  int WriteAsChar  (char data, int offset);

  // Description:
  // Write data as char (possibly compressed) from offset
  int WriteAsChar  (char *data, int length, int offset);

  // Description:
  // Write data as unsigned char (possibly compressed) from offset
  int WriteAsUChar (unsigned char data, int offset);

  // Description:
  // Write data as unsigned char (possibly compressed) from offset
  int WriteAsUChar (unsigned char *data, int length, int offset);

  // Description:
  // Write data as short (possibly compressed) from offset
  int WriteAsShort (short data, int offset);

  // Description:
  // Write data as short (possibly compressed) from offset
  int WriteAsShort (short *data, int length, int offset);

  // Description:
  // Write data as unsigned short (possibly compressed) from offset
  int WriteAsUShort(unsigned short data, int offset);

  // Description:
  // Write data as unsigned short (possibly compressed) from offset
  int WriteAsUShort(unsigned short *data, int length, int offset);

  // Description:
  // Write data as int (possibly compressed) from offset
  int WriteAsInt   (int data, int offset);

  // Description:
  // Write data as int (possibly compressed) from offset
  int WriteAsInt   (int *data, int length, int offset);

  // Description:
  // Write data as unsigned int (possibly compressed) from offset
  int WriteAsUInt  (unsigned int data, int offset);

  // Description:
  // Write data as unsigned int (possibly compressed) from offset
  int WriteAsUInt  (unsigned int *data, int length, int offset);

  // Description:
  // Write data as float (possibly compressed) from offset
  int WriteAsFloat (float data, int offset);

  // Description:
  // Write data as float (possibly compressed) from offset
  int WriteAsFloat (float *data, int length, int offset);

  // Description:
  // Write data as double (possibly compressed) from offset
  int WriteAsDouble(double data, int offset);

  // Description:
  // Write data as double (possibly compressed) from offset
  int WriteAsDouble(double *data, int length, int offset);

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

};


inline void vtkCISGCofstream::Close()
{
  this->close();
}

inline int vtkCISGCofstream::IsCompressed()
{
  return _compressed;
}

inline int vtkCISGCofstream::IsSwapped()
{
  return _swapped;
}

#endif
