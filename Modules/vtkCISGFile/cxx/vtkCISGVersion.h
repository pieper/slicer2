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
  Module    : $RCSfile: vtkCISGVersion.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:26 $
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
// .NAME vtkCISGVersion - Versioning class for vtkCISG
// .SECTION Description
// Holds methods for defining/determining the current vtk version
// (major, minor, build).

// .SECTION Caveats
// This file will change frequently to update the VTKSourceVersion which
// timestamps a particular source release.


#ifndef __vtkCISGVersion_h
#define __vtkCISGVersion_h

#include <vtkCISGFileConfigure.h>


#include "vtkVersion.h"

#define VTK_CISG_MAJOR_VERSION 2
#define VTK_CISG_MINOR_VERSION 0
#define VTK_CISG_BUILD_VERSION 0
#define VTK_CISG_VERSION "2.0.0"
#define VTK_CISG_SOURCE_VERSION "vtkCISG version " VTK_CISG_VERSION ", $Date: 2005/04/19 14:48:26 $ (GMT)"

#define VTK_CISG_DISCLAIMER "\n\
****************** VTK CISG Registration Toolkit *******************-\n\
            written by T. Hartkens <thomas@hartkens.de>\n\
           based on code by D. Rueckert and J. Schnabel\n\
                    www.image-registration.com         \n\
\n\
                Prof. D.J. Hawkes   Dr. D.L.G. Hill\n\
     Computational Imaging Science Group, King's College London\n\
                  http://www-ipg.umds.ac.uk/cisg\n\
*********************************************************************\n\
version " VTK_CISG_VERSION ", Copyright (C) 2001-2002 CISG, King's College London\n\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License.\n\
The software has no regulatory approval eg, no CE mark or FDA\n\
approval, and is not to be used for clinical purposes, or in any way\n\
to effect patient management. It comes with ABSOLUTELY NO WARRANTY.\n\
"


class VTK_CISGFILE_EXPORT vtkCISGVersion : public vtkObject {
public:
  static vtkCISGVersion *New();
  vtkTypeMacro(vtkCISGVersion,vtkObject);
  
  // Description: 
  // Return the version of vtk this object is a part of.
  // A variety of methods are included. GetVTKSourceVersion returns a string
  // with an identifier which timestamps a particular source tree. 
  static const char *GetVersion() { return VTK_CISG_VERSION; }
  static int GetMajorVersion() { return VTK_CISG_MAJOR_VERSION; }
  static int GetMinorVersion() { return VTK_CISG_MINOR_VERSION; }
  static int GetBuildVersion() { return VTK_BUILD_VERSION; }
  static const char *GetSourceVersion() { return VTK_CISG_SOURCE_VERSION; }
  static const char *GetDisclaimer() { return  VTK_CISG_DISCLAIMER; };
  
protected:
  vtkCISGVersion() {}; //insure constructor/destructor protected
  ~vtkCISGVersion() {};
private:
  vtkCISGVersion(const vtkCISGVersion&);  // Not implemented.
  void operator=(const vtkCISGVersion&);  // Not implemented.
};

#endif 
