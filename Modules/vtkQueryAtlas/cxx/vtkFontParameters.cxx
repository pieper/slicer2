/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkFontParameters.cxx,v $
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

#include <math.h>

#include "vtkFontParameters.h"
#include "vtkTextureFontManager.h"
#include "vtkTextureText.h"


vtkCxxRevisionMacro(vtkFontParameters, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkFontParameters);


vtkFontParameters::vtkFontParameters()
{
    this->FontFileName = NULL;
    this->FontDirectory = NULL;

    // TODO - make a local default resolution?
    this->Resolution = vtkFreetypeRasterizer::GetDefaultRasterizerResolution();

    // TODO - make a SetDefaultBlur?  0 was the previous constructor default (2 might be better)
    this->Blur = 0;

    // TODO - make a SetDefaultStyle?  VTK_TEXTURE_TEXT_STYLE_PLAIN was the previous constructor default
    this->Style = VTK_TEXTURE_TEXT_STYLE_PLAIN;

    // TODO - make a SetDefaultStyleBorder?  2 was the previous constructor default
    this->StyleBorderSize = 2;

    this->TextureText = NULL;
    this->TextureFont  = NULL;

//    mError = 0;
//    this->Initialized = 0;
}


vtkFontParameters::~vtkFontParameters()
{
    // TODO - do whatever cleanups
}


void vtkFontParameters::Modified()
{
    if (this->TextureText != NULL) {
        // don't keep getting new fonts until after we have done the initial Text setup
        if (!this->TextureText->GetInitialized()) return;

        this->RequestTextureFont();

        return;
    }

    if (this->TextureFont != NULL) {
        if (this->TextureFont->GetInitialized())
            vtkErrorMacro("vtkFontParameters::Modified() - error: TextureFont cannot be modified.\n");
        return;
    }
}


// TextureText uses this the first time it gets/creates its font
vtkTextureFont *vtkFontParameters::RequestTextureFont() {
    vtkTextureFontManager *m = vtkTextureFontManager::GetDefaultManager();
    vtkTextureFont *f = m->GetTextureFont(this);

    if (f == NULL) {
        vtkErrorMacro("vtkFontParameters::RequestTextureFont() - a NULL Font was returned from vtkTextureFontManager.\n");
        return NULL;
    }

    if (this->TextureText != NULL) {
         // if the specified font is the same as the one already being used, don't update
        if (f == this->TextureText->GetTextureFont()) return f;
        this->TextureText->SetTextureFont(f);
        this->TextureText->Modified();
    }

    return f;
}


void vtkFontParameters::SetFontDirectory(char *dirname) {
    // allocate an extra char, in case we need to add an extra '/'
    this->FontDirectory = (char *)malloc(strlen(dirname) + 2);
    strcpy(this->FontDirectory, dirname);
    int l = strlen(this->FontDirectory);

    if (this->FontDirectory[l-1] != '/'  &&  this->FontDirectory[l-1] != '\\')
        this->FontDirectory = strcat(this->FontDirectory, "/");

    Modified();
}


void vtkFontParameters::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    /*  TODO - do print
    os << indent << "X Resolution: " << this->XResolution << "\n";
    os << indent << "Y Resolution: " << this->YResolution << "\n";
    */
}
