/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGInterfileReader.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2001
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
// .NAME vtkCISGInterfileReader - read Interfile image file format
// .SECTION Description
// vtkCISGInterfileReader is an instantiated class of vtkCISGMultiReader.
// It allows reading of Interfile format, a common image file format in
// nuclear medicine. This file format is also supported by other software
// packages such as the Analyze package from the Biomedical Imaging 
// Resource, Mayo Clinic (http://www.mayo.edu/bir).
// 
// .SECTION Caveats
//
//
// .SECTION see also
// vtkCISGMultiReader vtkCISGInterfileWriter

#ifndef __vtkCISGInterfileReader_h
#define __vtkCISGInterfileReader_h

#include <vtkCISGFileConfigure.h>

#include <stdio.h>

#include "vtkCISGMultiReader.h"

class VTK_CISGFILE_EXPORT vtkCISGInterfileReader : public vtkCISGMultiReader
{
public:
  vtkTypeMacro(vtkCISGInterfileReader,vtkCISGMultiReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with NULL file prefix; file pattern "%s.%d"; image range 
  // set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
  // header size 0; and byte swapping turned off.
  static vtkCISGInterfileReader *New();

  // Checks whether data is in Interfile format. It checks for the magic
  // keyword "interfile" within the first 20 lines of the header file.
  static int CheckHeader(char *filename);

protected:

  // Description:
  // Default constructor.
  vtkCISGInterfileReader();

  // Description:
  // Destructor.
  ~vtkCISGInterfileReader();

  // Description:
  // Copy constructor (empty).
  vtkCISGInterfileReader(const vtkCISGInterfileReader&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGInterfileReader&) {};

  // Description:
  // This method reads the header and volume data using ReadHeader() and 
  // ReadVolume().
  void Execute();

  // Description:
  // Read header of data. 
  // This method sets the image dimensions, voxel size, data type etc.
  int ReadHeader(FILE *fp, char *name);

  // Description:
  // Read a volume of data. 
  // Data is read according to header information obtained by ReadHeader().
  vtkDataArray *ReadVolume();

  // Description:
  // Member to hold name for the image data file.
  char *ImageDataFile;

  // Description:
  // Member to set from header for bigendian or littlendian byte order.
  int bigendian;

};

#endif


