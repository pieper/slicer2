/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkAnalyze.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:33 $
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
// .NAME vtkAnalyze - 
// .SECTION Description
//

// .SECTION see also
//

#ifndef _VTKANALYZE_H

#define _VTKANALYZE_H

#include <vtkCISGFileConfigure.h>

#define ANALYZE_NONE                   0 
#define ANALYZE_UNKNOWN                0      
#define ANALYZE_BINARY                 1  
#define ANALYZE_U_CHAR                 2      
#define ANALYZE_SHORT                  4     
#define ANALYZE_INT                    8  
#define ANALYZE_FLOAT                  16
#define ANALYZE_COMPLEX                32   
#define ANALYZE_DOUBLE                 64 
#define ANALYZE_RGB                    128
#define ANALYZE_ALL                    255

#define ANALYZE_HEADERSIZE             348



#endif

