/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkGipl.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:38 $
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
// .NAME vtkGipl - 
// .SECTION Description
//

// .SECTION see also
//

#ifndef _ITKGIPL_H

#include <vtkCISGFileConfigure.h>

// GIPL magic number
#define GIPL_MAGIC        719555000

// GIPL header size
#define GIPL_HEADERSIZE   256

// GIPL filter types
#define GIPL_BINARY       1
#define GIPL_CHAR         7
#define GIPL_U_CHAR       8
#define GIPL_SHORT        15
#define GIPL_U_SHORT      16
#define GIPL_U_INT        31
#define GIPL_INT          32
#define GIPL_FLOAT        64
#define GIPL_DOUBLE       65
#define GIPL_C_SHORT      144
#define GIPL_C_INT        160
#define GIPL_C_FLOAT      192
#define GIPL_C_DOUBLE     193

#endif
