/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGInterfileWriter.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:36 $
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
// .NAME vtkCISGInterfileWriter - write Interfile image file format
// .SECTION Description
// vtkCISGInterfileWriter is an instantiated class of vtkCISGMultiWriter.
// It allows writing of Interfile format, a common image file format in
// nuclear medicine. This file format is also supported by other software
// packages such as the Analyze package from the Biomedical Imaging 
// Resource, Mayo Clinic (http://www.mayo.edu/bir).
// 
// .SECTION Caveats
//
//
// .SECTION see also
// vtkCISGMultiWriter vtkCISGInterfileWriter

#ifndef __vtkCISGInterfileWriter_h
#define __vtkCISGInterfileWriter_h

#include <vtkCISGFileConfigure.h>

#include "vtkCISGMultiWriter.h"
#include "vtkStructuredPoints.h"

class VTK_CISGFILE_EXPORT vtkCISGInterfileWriter : public vtkCISGMultiWriter {
public:

  static vtkCISGInterfileWriter *New();
  vtkTypeMacro(vtkCISGInterfileWriter,vtkCISGMultiWriter);
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
  vtkCISGInterfileWriter() {};

  // Description:
  // Destructor.
  ~vtkCISGInterfileWriter() {};

  // Description:
  // Copy constructor (empty).
  vtkCISGInterfileWriter(const vtkCISGInterfileWriter&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGInterfileWriter&) {};

  // Description:
  // Writes the image data.
  void WriteData();
  int CopyHeaderFile(FILE *output, vtkCISGInterfileReader *Reader, 
                     char *ImageFileName);
  void CreateDefaultHeader(FILE *output, char *ImageFileName);

};

#endif


