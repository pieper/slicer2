/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTextureTextPolys.cxx,v $
  Date:      $Date: 2005/12/20 22:56:17 $
  Version:   $Revision: 1.1.8.1 $

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


#include "vtkTextureTextPolys.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"


#include "vtkTextureText.h"


vtkCxxRevisionMacro(vtkTextureTextPolys, "$Revision: 1.1.8.1 $");
vtkStandardNewMacro(vtkTextureTextPolys);


vtkTextureTextPolys::vtkTextureTextPolys()
{
    this->Normal[2] = 1.0;
    this->Normal[0] = this->Normal[1] = 0.0;
}


void vtkTextureTextPolys::SetTextureText(vtkTextureText *iT) {
    this->mTextureText = iT;
}


void vtkTextureTextPolys::Execute()
{
    vtkIdType pts[4];
    int i;
    int numPts;
    int numPolys;
    vtkPoints *newPoints;
    vtkFloatArray *newNormals;
    vtkFloatArray *newTCoords;
    vtkCellArray *newPolys;
    vtkPolyData *output = this->GetOutput();

    if (this->mTextureText == NULL) return;

    // v48 - don't make a bad poly set - can stop all other rendering too.
    if (this->mTextureText->GetError()) return;

    // Set things up; allocate memory
    numPts = this->mTextureText->mCharCount * 4;
    numPolys = this->mTextureText->mCharCount;

    if (numPts == 0) return;

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

    int p = this->mTextureText->mCharCount;

    for (int ichar = 0; ichar < p; ichar++) {
        for (i=0; i<4; i++) {
            newPoints->InsertPoint(curPt, this->mTextureText->mCharGsetCoords[ichar * 4 + i].values);
            newTCoords->InsertTuple(curPt, this->mTextureText->mTexCoords[ichar * 4 + i].values);
            newNormals->InsertTuple(curPt++, this->Normal);
        }

        // Generate polygon connectivity
        // v20 - Why was this done 4x?!
        //for (int cc = 0; cc < 4; cc++) {
            pts[0] = 0 + ichar * 4;
            pts[1] = 1 + ichar * 4;
            pts[2] = 2 + ichar * 4;
            pts[3] = 3 + ichar * 4;

            newPolys->InsertNextCell(4,pts);
        //}
    }

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


void vtkTextureTextPolys::PrintSelf(ostream& os, vtkIndent indent)
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
