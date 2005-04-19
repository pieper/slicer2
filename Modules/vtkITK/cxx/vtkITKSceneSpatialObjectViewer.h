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
  Module:    $RCSfile: vtkITKSceneSpatialObjectViewer.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 18:47:07 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkITKSceneSpatialObjectViewer
// .SECTION Description
// vtkITKSceneSpatialObjectViewer


#ifndef __vtkITKSceneSpatialObjectViewer_h
#define __vtkITKSceneSpatialObjectViewer_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4284 )
#pragma warning ( disable : 4018 )
#endif

#include "vtkProcessObject.h"
#include "sovVTKRenderer3D.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"

class VTK_EXPORT vtkITKSceneSpatialObjectViewer : public vtkProcessObject
{
 
public:
  static vtkITKSceneSpatialObjectViewer *New();
  vtkTypeRevisionMacro(vtkITKSceneSpatialObjectViewer, vtkProcessObject);

  void SetRenderer(vtkRenderer* renderer)
    {
    m_Renderer = renderer;
    }

  void SetFileName(const char* filename)
    {
    m_FileName = filename;
    }
  void AddActors();

protected:
  //BTX
  vtkITKSceneSpatialObjectViewer() // : Superclass ( vtkObject::New() )
    {
    };

  ~vtkITKSceneSpatialObjectViewer() {};
  //ETX
  
private:
  vtkITKSceneSpatialObjectViewer(const vtkITKSceneSpatialObjectViewer&);  // Not implemented.
  void operator=(const vtkITKSceneSpatialObjectViewer&);  // Not implemented.

  vtkRenderer*                 m_Renderer;
  const char*                  m_FileName;
};

vtkCxxRevisionMacro(vtkITKSceneSpatialObjectViewer, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKSceneSpatialObjectViewer);

#endif




