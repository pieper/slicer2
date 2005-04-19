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
  Module    : $RCSfile: vtkInternalFlip.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:27 $
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
// .NAME  vtkInternalFlip - Class to flip images
// .SECTION Description
// Small helper class to flip images after reading or before writing.

// .SECTION see also
// vtkCISGMultiReader vtkCISGMultiWriter

#ifndef __InternalFlip_h
#define __InternalFlip_h

#include <vtkCISGFileConfigure.h>


template <class T> void InternalFlip( int axis, 
                              T *data,
                              int Dim[3]) {
  T buffer;
  int pos1,pos2=0;
  long z,y,x;
  switch (axis) {
  case 0: 
    for (z=0; z< Dim[2]; z++) {
      for (y=0; y< Dim[1]; y++) {
        for (x=0; x< Dim[0]/2; x++) {
          pos1=z*Dim[1]*Dim[0]+ y*Dim[0] + x;
          pos2= z*Dim[1]*Dim[0]+ y*Dim[0] + Dim[0]-x-1;
          buffer=data[pos1];
          data[pos1]=data[pos2];
          data[pos2]=buffer;
        }
      }
    }
    break;
  case 1: 
    for (z=0; z< Dim[2]; z++) {
      for (y=0; y< Dim[1]/2; y++) {
        for (x=0; x< Dim[0]; x++) {
          pos1=z*Dim[1]*Dim[0]+ y*Dim[0] + x;
          pos2= z*Dim[1]*Dim[0]+ (Dim[1]-y-1)*Dim[0] + x;
          buffer=data[pos1];
          data[pos1]=data[pos2];
          data[pos2]=buffer;
        }
      }
    }
    break;
  case 2: 
    for (z=0; z< Dim[2]/2; z++) {
      for (y=0; y< Dim[1]; y++) {
        for (x=0; x< Dim[0]; x++) {
          pos1=z*Dim[1]*Dim[0]+ y*Dim[0] + x;
          pos2= (Dim[2]-z-1)*Dim[1]*Dim[0]+ y*Dim[0] + x;
          buffer=data[pos1];
          data[pos1]=data[pos2];
          data[pos2]=buffer;
        }
      }
    }
    break;
  }
  
}



#endif
