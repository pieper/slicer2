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

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: vtkITKSceneSpatialObjectViewer.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/19 18:47:07 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkITKSceneSpatialObjectViewer_txx
#define _vtkITKSceneSpatialObjectViewer_txx
#include "vtkITKSceneSpatialObjectViewer.h"

#include "itkSpatialObjectReader.h"
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAssembly.h>

void vtkITKSceneSpatialObjectViewer::AddActors()
  {
  typedef itk::SpatialObjectReader<3> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(m_FileName);

  try
    {
    reader->Update();
    }
 catch( itk::ExceptionObject &e )
    {
    std::cout << "Exeception " << e << std::endl;
    }
 
  typedef itk::GroupSpatialObject<3> GroupType;
  GroupType::Pointer group = reader->GetGroup();

  typedef itk::SceneSpatialObject<3> SceneType;
  SceneType::Pointer m_Scene = SceneType::New();
  m_Scene->AddSpatialObject(group);

  group->ComputeBoundingBox();
  itk::Vector<double,3> offset;
  offset[0] = -2*(group->GetBoundingBox()->GetCenter()[0]);
  offset[1] = -2*(group->GetBoundingBox()->GetCenter()[1]);
  offset[2] = -2*(group->GetBoundingBox()->GetCenter()[2]);

  sov::VTKRenderer3D::Pointer m_SovVTKRenderer = sov::VTKRenderer3D::New();
  m_SovVTKRenderer->SetScene(m_Scene);
  m_SovVTKRenderer->Update();

  typedef itk::SpatialObject<3> SpatialObjectType;
  SpatialObjectType::ChildrenListType* children = group->GetChildren(999999);
  SpatialObjectType::ChildrenListType::iterator it = children->begin();

  while(it != children->end())
    {
    m_SovVTKRenderer->AssociateWithRenderMethod(*it,"DTITubeTensorVTKRenderMethod3D"); // Associate the object with a particular render method    
    it++;
    }

  delete children;

  vtkActorCollection* actors = m_SovVTKRenderer->GetVTKRenderer()->GetActors();
  actors->InitTraversal();

  vtkActor* actor = actors->GetNextActor();
  actor = actors->GetNextActor();
  vtkAssembly* assembly = vtkAssembly::New();
  while(actor)
    {
    assembly->AddPart(actor);
    actor = actors->GetNextActor();
    if(actor)
      {
      actor = actors->GetNextActor();
      }
    }
  
  // Center the object
  offset[0] = -(assembly->GetBounds()[0]+assembly->GetBounds()[1])/2.0;
  offset[1] = -(assembly->GetBounds()[2]+assembly->GetBounds()[3])/2.0;
  offset[2] = -(assembly->GetBounds()[4]+assembly->GetBounds()[5])/2.0;
  assembly->SetPosition(offset[0],offset[1],offset[2]);

  m_Renderer->AddActor(assembly);

 }


#endif
