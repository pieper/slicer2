/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTextureFont.h,v $
  Date:      $Date: 2005/12/20 22:56:16 $
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


#ifndef __vtkTextureFont_h
#define __vtkTextureFont_h


#include "vtkFreetypeRasterizer.h"
#include "vtkFontParameters.h"

#include "vtkImageData.h"
#include <vtkQueryAtlasConfigure.h>

#include "simpleVectors.h"


class VTK_QUERYATLAS_EXPORT vtkTextureFont : public vtkImageData
{

public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkTextureFont,vtkImageData);

    static vtkTextureFont *New();

    int CreateFont();
    int CreateRasterizer(char *fontpath);

//BTX
    Vector2D<vtkFloatingPointType> GetChararacterPosition(int c);
    Vector2D<vtkFloatingPointType> GetChararacterSize(int c);
//ETX

    vtkGetMacro(Width, int);
    vtkGetMacro(Height, int);

    vtkGetObjectMacro(FreetypeRasterizer,vtkFreetypeRasterizer);
    vtkGetObjectMacro(FontParameters,vtkFontParameters);
    vtkGetMacro(Initialized,int);
    vtkGetMacro(Error,int);


protected:
    vtkTextureFont();
    ~vtkTextureFont();

//BTX
    Vector2D<vtkFloatingPointType> *mCharPositions;
    Vector2D<vtkFloatingPointType> *mCharSizes;
//ETX
    vtkFontParameters *FontParameters;
    vtkFreetypeRasterizer *FreetypeRasterizer;

//    char *FontFileName;
//    char *FontDirectory;

    int    Width;
    int Height;

    int Initialized;
    int Error;

    vtkTextureFont *loadFont( char *fontname, int blur = 0);   
    vtkTextureFont *filter_image(unsigned char *src, unsigned char *dst, int wd, int ht, int filter); 
    void copy_array(unsigned char *dst, unsigned char *src, int size);
    void zero_array(unsigned char *ptr, int size);
    void MakeImageData(unsigned char *Alpha, unsigned char *intensity);

//BTX
    void vtkTextureFont::copyArrayOffset(unsigned char *dst, 
                                     unsigned char *src,
                                     int w, int h, int xOff, int yOff);

    void vtkTextureFont::mattArrayOffset(unsigned char *dstAlpha, unsigned char *dstI, 
                                     unsigned char *srcAlpha, unsigned char srcLevel, 
                                     int w, int h, int xOff, int yOff);
//ETX
};

#endif
