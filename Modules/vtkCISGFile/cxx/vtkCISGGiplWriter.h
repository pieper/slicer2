/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGGiplWriter.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
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
// .NAME vtkCISGGiplWriter - write GIPL image file format
// .SECTION Description
// vtkCISGGIPLWriter is an instantiated class of vtkCISGMultiWriter.
// It allows writing of GIPL image file format, which is a format originating
// from the Computational Imaging Science Group, Div. of Radiological
// Sciences, Guy's Hospital, King's College London. GIPL stands for Guys
// Image Processing Library, which was based on the local VoxObj software
// package at Guy's developed by Colin Studholme, Derek Hill, Jason Zhao,
// Cliff Ruff, and Abhir H. Bhalerao. As time went by, (nearly) all that was
// left was this really useful image file format, which was adopted by
// quite a number of other groups. It contains both header and image volume
// data within one file, and supports VTK_CHAR, VTK_UNSIGNED_CHAR,
// VTK_SHORT, VTK_UNSIGNED_SHORT, VTK_INT, VTK_UNSIGNED_INT, VTK_FLOAT.
//
// .SECTION Caveats
//
//
// .SECTION see also
// vtkCISGMultiWriter vtkCISGGIPLWriter

#ifndef __vtkCISGGiplWriter_h
#define __vtkCISGGiplWriter_h

#include <vtkCISGFileConfigure.h>

#include "vtkCISGMultiWriter.h"
#include "vtkStructuredPoints.h"

class VTK_CISGFILE_EXPORT vtkCISGGiplWriter : public vtkCISGMultiWriter {
public:
  static vtkCISGGiplWriter *New();
  vtkTypeMacro(vtkCISGGiplWriter,vtkCISGMultiWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Checks for output file format.
  static int Check(char *filename);

  // Description:
  // Updates grandparent vtkDataWriter.
  void Update();
                               
protected:

  // Description:
  // Default constructor.
  vtkCISGGiplWriter() {};

  // Description:
  // Destructor.
  ~vtkCISGGiplWriter() {};

  // Description:
  // Copy constructor (empty).
  vtkCISGGiplWriter(const vtkCISGGiplWriter&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGGiplWriter&) {};

  // Description:
  // Writes the binary image data.
  void WriteData();

};

#endif


