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


#ifndef __vtkFontParameters_h
#define __vtkFontParameters_h

#include <vtkObject.h>
#include <vtkObjectFactory.h>

//#include "vtkTextureText.h"
//#include "vtkTextureFont.h"

class vtkTextureText;
class vtkTextureFont;

#include <vtkQueryAtlasConfigure.h>


enum {
    VTK_TEXTURE_TEXT_STYLE_PLAIN = 0,
    VTK_TEXTURE_TEXT_STYLE_OUTLINE = 1,
    VTK_TEXTURE_TEXT_STYLE_SHADOW = 2
};


class VTK_QUERYATLAS_EXPORT vtkFontParameters : public vtkObject
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkFontParameters,vtkObject);

    static vtkFontParameters *New();

    void Modified();
    vtkTextureFont *RequestTextureFont();

    vtkGetMacro(Resolution, int);
    vtkSetMacro(Resolution, int);

    vtkGetMacro(Blur, int);
    vtkSetMacro(Blur, int);

    vtkGetMacro(Style, int);
    vtkSetMacro(Style, int);

    vtkGetMacro(StyleBorderSize, int);
    vtkSetMacro(StyleBorderSize, int);

    vtkGetStringMacro(FontFileName);
    vtkSetStringMacro(FontFileName);
    vtkGetStringMacro(FontDirectory);
    
    // use a method, that adds a trailing '/' if necessary
    void SetFontDirectory(char *dirname);


    vtkGetObjectMacro(TextureText, vtkTextureText);
    // NOTE - for some reason, I can't use the Set macro w/ these classes - seemingly because the header files are cross-referenced.
    //vtkSetObjectMacro(TextureText, vtkTextureText);

    vtkGetObjectMacro(TextureFont, vtkTextureFont);
    //vtkSetObjectMacro(TextureFont, vtkTextureFont);


    // TODO - make private, and provide the Set methods
    vtkTextureText *TextureText;
    vtkTextureFont *TextureFont;


protected:
    vtkFontParameters();
    ~vtkFontParameters();

    int Blur;
    int Style;
    int StyleBorderSize;
    int Resolution;
    char *FontFileName;
    char *FontDirectory;
};

#endif
