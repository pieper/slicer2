/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkRectangle.cxx,v $
  Date:      $Date: 2006/01/06 17:58:00 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/
/*===========================================================

3D Slicer Software Licence Agreement(c)

Copyright 2003-2004 Massachusetts Institute of Technology 
(MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and 
Women's Hospital, Inc. on behalf of the copyright holders 
and contributors. Permission is hereby granted, without 
payment, to copy, modify, display and distribute this 
software and its documentation, if any, for research 
purposes only, provided that (1) the above copyright notice
and the following four paragraphs appear on all copies of 
this software, and (2) that source code to any 
modifications to this software be made publicly available 
under terms no more restrictive than those in this License 
Agreement. Use of this software constitutes acceptance of 
these terms and conditions.

3D Slicer Software has not been reviewed or approved by 
the Food and Drug Administration, and is for non-clinical, 
IRB-approved Research Use Only. In no event shall data or 
images generated through the use of 3D Slicer Software be 
used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE 
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE 
COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY 
DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS." THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, 
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Authors: Michael McKenna, David Small, Steve Pieper.
    Small Design Firm, Inc.,
    in association with Isomics, Inc.,
    and Partners Health Care.  MA, USA.
    June, 2004

===========================================================*/


#include "vtkRectangle.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"


#include "vtkTextureText.h"

#include "simpleVectors.h"

#ifdef _WIN32 // WINDOWS
#include <vector>
#else // UNIX
#include <vector.h>
#endif



vtkCxxRevisionMacro(vtkRectangle, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkRectangle);


vtkRectangle::vtkRectangle()
{
    this->Normal[2] = 1.0;
    this->Normal[0] = this->Normal[1] = 0.0;

    this->Width = 1;
    this->Height = 1;
    this->ZOffset = 0;

    this->Left = 0;
    this->Bottom = 0;

    this->TextureTurn = 0;
}


void vtkRectangle::Execute()
{
    vtkIdType pts[4];
    int numPts;
    int numPolys;
    vtkPoints *newPoints; 
    vtkFloatArray *newNormals;
    vtkFloatArray *newTCoords;
    vtkCellArray *newPolys;
    vtkPolyData *output = this->GetOutput();

    // Set things up; allocate memory
    numPts = 4;
    numPolys = 1;

    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);
    newNormals = vtkFloatArray::New();
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3*numPts);
    newTCoords = vtkFloatArray::New();
    newTCoords->SetNumberOfComponents(2);
    newTCoords->Allocate(2*numPts);

    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(numPolys,4));

    // Generate points and point data
    int curPt = 0;

    // v44 - nudge in corner of map Gradient showed some sort of wrap-around on the outer edge
    //vtkFloatingPointType zed = 0.02;
    //vtkFloatingPointType one = 0.98;
    vtkFloatingPointType zed = 0.;
    vtkFloatingPointType one = 1.;

    std::vector< Vector2D<vtkFloatingPointType> > tex;
    Vector2D<vtkFloatingPointType> t;
    t.set(zed, zed);
    tex.push_back(t);
    t.set(one, zed);
    tex.push_back(t);
    t.set(one, one);
    tex.push_back(t);
    t.set(zed, one);
    tex.push_back(t);

    // The TextureTurn count is the # of 90 degree clockwise turns to make for the texture coords
    int turnI = this->TextureTurn;

    Vector3D<vtkFloatingPointType> p;
    //Vector2D<vtkFloatingPointType> t;
    p.set(Left, Bottom, ZOffset);
    //t.set(zed, zed);
    if (turnI >= 4) turnI = 0;
    t = tex[turnI++];
    newPoints->InsertPoint(curPt, p.values);
    newTCoords->InsertTuple(curPt, t.values);
    newNormals->InsertTuple(curPt++, this->Normal);

    p.set(Left + Width, Bottom, 0);
    //t.set(one, zed);
    if (turnI >= 4) turnI = 0;
    t = tex[turnI++];
    newPoints->InsertPoint(curPt, p.values);
    newTCoords->InsertTuple(curPt, t.values);
    newNormals->InsertTuple(curPt++, this->Normal);

    p.set(Left + Width, Bottom + Height, 0);
    //t.set(one, one);
    if (turnI >= 4) turnI = 0;
    t = tex[turnI++];
    newPoints->InsertPoint(curPt, p.values);
    newTCoords->InsertTuple(curPt, t.values);
    newNormals->InsertTuple(curPt++, this->Normal);

    p.set(Left, Bottom + Height, 0);
    //t.set(zed, one);
    if (turnI >= 4) turnI = 0;
    t = tex[turnI++];
    newPoints->InsertPoint(curPt, p.values);
    newTCoords->InsertTuple(curPt, t.values);
    newNormals->InsertTuple(curPt++, this->Normal);

    // Generate polygon connectivity        
    pts[0] = 0;
    pts[1] = 1;
    pts[2] = 2;
    pts[3] = 3;
    newPolys->InsertNextCell(4,pts);
    

    // Update ourselves and release memory
    output->SetPoints(newPoints);
    newPoints->Delete();

    output->GetPointData()->SetNormals(newNormals);
    newNormals->Delete();

    output->GetPointData()->SetTCoords(newTCoords);
    newTCoords->Delete();

    output->SetPolys(newPolys);
    newPolys->Delete();
}


/*
void vtkRectangle::SetScalePoints(vtkFloatingPointType x, vtkFloatingPointType y) {
    SetWidth(x);
    SetHeight(y);
    Modified();
}
*/


// like vtkTextuireText::SetPositionOffset - don't move in regular space - offset the points
void vtkRectangle::SetPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z) {
    SetLeft(x);
    SetBottom(y);
    SetZOffset(z);
    Modified();
}


void vtkRectangle::GetPositionOffset(vtkFloatingPointType pos[3]) {
    pos[0] = this->GetLeft();
    pos[1] = this->GetBottom();
    pos[2] = this->GetZOffset();
}


void vtkRectangle::AddPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z) {
    vtkFloatingPointType pos[3];
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    this->AddPositionOffset(pos);
}


// adjust rect vert locations, by adding a new bias into the PositionOffset
void vtkRectangle::AddPositionOffset(vtkFloatingPointType posNew[3]) {
    vtkFloatingPointType posCur[3];
    this->GetPositionOffset(posCur);
    for (int i = 0; i < 3; i++) posCur[i] += posNew[i];

    this->SetPositionOffset(posCur[0], posCur[1], posCur[2]);
}


void vtkRectangle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  /*  TODO - do print
  os << indent << "X Resolution: " << this->XResolution << "\n";
  os << indent << "Y Resolution: " << this->YResolution << "\n";

  os << indent << "Origin: (" << this->Origin[0] << ", "
                              << this->Origin[1] << ", "
                              << this->Origin[2] << ")\n";

  os << indent << "Point 1: (" << this->Point1[0] << ", "
                               << this->Point1[1] << ", "
                               << this->Point1[2] << ")\n";
...
*/

}
