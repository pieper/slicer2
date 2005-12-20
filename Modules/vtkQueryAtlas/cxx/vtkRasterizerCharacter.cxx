/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkRasterizerCharacter.cxx,v $
  Date:      $Date: 2005/12/20 22:56:15 $
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


#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "vtkRasterizerCharacter.h"


vtkCxxRevisionMacro(vtkRasterizerCharacter, "$Revision: 1.1.8.1 $");
vtkStandardNewMacro(vtkRasterizerCharacter);


vtkRasterizerCharacter::vtkRasterizerCharacter()
{
    this->mAsciiValue = 0;
    this->mHeight = 0;
    this->mWidth = 0;
    this->mAdvanceWidth = 0;
    this->mTopExtent = 0;
    this->mLeftExtent = 0;
    this->mBitmap = NULL;
}


vtkRasterizerCharacter::~vtkRasterizerCharacter()
{
    if (this->mBitmap != NULL) free(this->mBitmap);
}


int vtkRasterizerCharacter::GetBottomExtent() {
    return (this->mTopExtent - this->mHeight);
}


int vtkRasterizerCharacter::GetRightExtent() {
    return (this->mLeftExtent + this->mWidth);
}


void vtkRasterizerCharacter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    /*  TODO - do print
    os << indent << "X Resolution: " << this->XResolution << "\n";
    os << indent << "Y Resolution: " << this->YResolution << "\n";
    ...
    */
}
