/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include "vtkImageCrossHair2D.h"
#include "vtkObjectFactory.h"

#define SET_PIXEL(x, y, color) { ptr = &outPtr[(y)*nxnc + (x)*nc]; \
    ptr[0] = color[0]; ptr[1] = color[1]; ptr[2] = color[2];}


//------------------------------------------------------------------------------

vtkImageCrossHair2D* vtkImageCrossHair2D::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageCrossHair2D");
  if(ret)
    {
    return (vtkImageCrossHair2D*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageCrossHair2D;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageCrossHair2D::vtkImageCrossHair2D()
{
    this->ShowCursorOn();
    this->IntersectCrossOff();
    this->BullsEyeOff();

    this->Cursor[0] = 0;
    this->Cursor[1] = 0;
    
    this->CursorColor[0] = 1;
    this->CursorColor[1] = 1;
    this->CursorColor[2] = 0;

    this->NumHashes = 5;
    this->Magnification = 1.0;
    this->HashLength = 5.0;
    this->HashGap = 5.0;
    this->BullsEyeWidth = 10;
}

//----------------------------------------------------------------------------

static void ConvertColor(float *f, unsigned char *c)
{
    c[0] = (int)(f[0] * 255.0);
    c[1] = (int)(f[1] * 255.0);
    c[2] = (int)(f[2] * 255.0);
}

//----------------------------------------------------------------------------

// Draw line including first, AND second end point
static void DrawLine(int x1, int y1, int x2, int y2, unsigned char color[3],
                     unsigned char *outPtr, int nxnc, int nc)
{
    unsigned char *ptr;
    long dx = abs(x2 - x1);
    long dy = abs(y2 - y1);
    long dy2, dx2, p, dydx2;
    long x, y, xEnd, yInc, yEnd, xInc;

    // Horizontal and vertical lines don't need to be special cases,
    // but it will make them faster.

    // Horizontal
    if (dy == 0)
    {
        y = y1;
        if (x1 < x2) {
            xEnd = x2;
            for (x=x1; x <= xEnd; x++)
                SET_PIXEL(x, y, color);
        } else {
            xEnd = x1;
            for (x=x2; x <= xEnd; x++)
                SET_PIXEL(x, y, color);
        }
    }
    // Vertical
    else if (dx == 0)
    {
        x = x1;
        if (y1 < y2) {
            yEnd = y2;
            for (y=y1; y <= yEnd; y++)
                SET_PIXEL(x, y, color);
        } else {
            yEnd = y1;
            for (y=y2; y <= yEnd; y++)
                SET_PIXEL(x, y, color);
        }
    }
    // < 45 degree slope
    else if (dy <= dx)
    {
        dydx2 = (dy-dx) << 1;
        dy2 = dy << 1;
        p = dy2 - dx;

        if (x1 < x2) {
            x = x1;
            y = y1;
            xEnd = x2;
            if (y1 < y2)
                yInc = 1;
            else
                yInc = -1;
        } else {
            x = x2;
            y = y2;
            xEnd = x1;
            if (y2 < y1)
                yInc = 1;
            else
                yInc = -1;
        }

        // Draw first point
        SET_PIXEL(x, y, color);

        // Draw up to end point
        while (x < xEnd)
        {
            x++;
            if (p <= 0)
                p += dy2;
            else {
                y += yInc;
                p += dydx2;
            }
            SET_PIXEL(x, y, color);
        }
        // Draw end point
        SET_PIXEL(x, y, color);
    }
    // > 45 degree slope
    else
    {
        dx2 = dx << 1;
        p = dx2 - dy;
        dydx2 = (dx-dy) << 1;

        if (y1 < y2) {
            x = x1;
            y = y1;
            yEnd = y2;
            if (x1 < x2)
                xInc = 1;
            else
                xInc = -1;
        } else {
            x = x2;
            y = y2;
            yEnd = y1;
            if (x2 < x1)
                xInc = 1;
            else
                xInc = -1;
        }

        // Draw first point
        SET_PIXEL(x, y, color);

        // Draw up to end point
        while (y < yEnd)
        {
            y++;
            if (p <= 0)
                p += dx2;
            else {
                x += xInc;
                p += dydx2;
            }
            SET_PIXEL(x, y, color);
        }
        // Draw end point
        SET_PIXEL(x, y, color);
    }
}

//----------------------------------------------------------------------------

void vtkImageCrossHair2D::DrawCursor(vtkImageData *outData, int outExt[6])
{
    unsigned char color[3];
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    int x, y, nx, ny, nx1, ny1, nxnc, nc, numHash, h, d, eye, e;
    int xMin, xMax, yMin, yMax;
  int gapx, gapy,lenx,leny,lenx2,leny2;
    float mmLen,  mmGap, mag;
  float mmSpacing[3];
    
  // Get Extent and Number of compenents(nc)
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    ny = outExt[3] - outExt[2] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;
    nx1 = nx-1;
    ny1 = ny-1;

  // Hash Mark number and length.
    numHash = this->NumHashes;
    mag = this->Magnification;
    mmLen = this->HashLength;
    mmGap = this->HashGap;

  // Covert Color from rgb in the rang 0..1 to 0..255
    ConvertColor(this->CursorColor, color);

  // Cursor Position.
    x = this->Cursor[0];
    y = this->Cursor[1];
    
    outData->GetSpacing(mmSpacing);
    // Conversions between mm and pixels:
    //   mm = pix * mmSpacing / mag
    //   pix = mm / mmSpacing * mag

  // Spacing and length of hash marks in pixels. 
    gapx = (int)(mmGap / mmSpacing[0] * mag + 0.49);
    lenx = (int)(mmLen / mmSpacing[0] * mag + 0.49);
    lenx2 = lenx / 2;

    gapy = (int)(mmGap / mmSpacing[1] * mag + 0.49);
    leny = (int)(mmLen / mmSpacing[1] * mag + 0.49);
    leny2 = leny / 2;


    // Check bounds
    if (x < xMin || x > xMax || y < yMin || y > yMax)
    {
        return;
    }

    // Draw horizontal cross
    if (this->IntersectCross)
    {
        DrawLine(0, y, nx1, y, color, outPtr, nxnc, nc);
    }
    else 
    {
        if (x-gapx >= xMin)
        {
            DrawLine(0, y, x-gapx, y, color, outPtr, nxnc, nc);
        }
        if (x+gapx <= xMax)
        {
            DrawLine(x+gapx, y, nx1, y, color, outPtr, nxnc, nc);
        }
    }

    // Draw vertical cross
    if (this->IntersectCross)
    {
        DrawLine(x, 0, x, ny1, color, outPtr, nxnc, nc);
    }
    else 
    {
        if (y-gapy >= yMin)
        {
            DrawLine(x, 0, x, y-gapy, color, outPtr, nxnc, nc);
        }
        if (y+gapy <= yMax)
        {
            DrawLine(x, y+gapy, x, ny1, color, outPtr, nxnc, nc);
        }
    }

    // Hashes along horizontal cross
    d = x;
    for (h=0; h < numHash; h++)
    {
        d += gapx;
        if (d >= xMin && d <= xMax && y-leny2 >= yMin && y+leny2 <= yMax)
    {
            DrawLine(d, y-leny2, d, y+leny2, color, outPtr, nxnc, nc);
    }
    }
    d = x;
    for (h=0; h < numHash; h++)
    {
        d -= gapx;
        if (d >= xMin && d <= xMax && y-leny2 >= yMin && y+leny2 <= yMax)
    {
            DrawLine(d, y-leny2, d, y+leny2, color, outPtr, nxnc, nc);
    }
    }

    // Hashes along vertical cross
    d = y;
    for (h=0; h < numHash; h++)
    {
        d += gapy;
        if (d >= yMin && d <= yMax && x-lenx2 >= xMin && x+lenx2 <= xMax)
    {
            DrawLine(x-lenx2, d, x+lenx2, d, color, outPtr, nxnc, nc);
    }
    }
    d = y;
    for (h=0; h < numHash; h++)
    {
        d -= gapy;
        if (d >= yMin && d <= yMax && x-lenx2 >= xMin && x+lenx2 <= xMax)
    {
            DrawLine(x-lenx2, d, x+lenx2, d, color, outPtr, nxnc, nc);
    }
    }

    // Bullseye around center pixel
    if (this->BullsEye)
    {
        eye = this->BullsEyeWidth;
        e = eye / 2;
        if (x-e >= xMin && x+e <= xMax && y-e >= yMin && y+e <= yMax) 
        {
            DrawLine(x-e, y-e, x+e, y-e, color, outPtr, nxnc, nc); //bottom
            DrawLine(x+e, y-e, x+e, y+e, color, outPtr, nxnc, nc); //right
            DrawLine(x+e, y+e, x-e, y+e, color, outPtr, nxnc, nc); //top
            DrawLine(x-e, y+e, x-e, y-e, color, outPtr, nxnc, nc); //left
        }
    }
}

//---------------------------------------------------------------------------- 
 
void vtkImageCrossHair2D::ExecuteData(vtkDataObject *out)
{
  // let superclass allocate data
  this->vtkImageInPlaceFilter::ExecuteData(out);

  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();

    int *outExt = outData->GetExtent();
        int *inExt;

    // ensure 3 component data
    int x1 = outData->GetNumberOfScalarComponents();
    if (!(x1 == 3 || x1 == 4)) 
  {
      vtkErrorMacro(<<"Input has "<<x1<<" instead of 3 or 4 components.");
        return;
    }

    // Ensure input is unsigned char
    x1 = outData->GetScalarType();
    if (x1 != VTK_UNSIGNED_CHAR)
    {
        vtkErrorMacro("Input is type "<<x1<<" instead of unsigned char.");
        return;
    }


    // Ensure outtput is 2D
    inExt = outData->GetWholeExtent();
    if (inExt[5] != inExt[4]) 
  {
        vtkErrorMacro("Input must be 2D.");
        return;
    }


    // Draw cursor
    if (this->ShowCursor)
  {
        DrawCursor(outData, outExt);
  }
}

void vtkImageCrossHair2D::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkImageInPlaceFilter::PrintSelf(os,indent);

    os << indent << "ShowCursor:               " << this->ShowCursor;
    os << indent << "NumHashes:                " << this->NumHashes;
    os << indent << "BullsEyeWidth in mm:      " << this->BullsEyeWidth;
    os << indent << "BullsEye On/Off:          " << this->BullsEye;
    os << indent << "Length of a Hash Mark:    " << this->HashLength;
  os << indent << "Cursor Position:          " << this->Cursor[0] << "," << this->Cursor[1];
    os << indent << "Does the Cross Intersect? " << this->IntersectCross;
    os << indent << "Magnification:            " << this->Magnification;
    os << indent << "HashGap:                  " << this->HashGap;
    os << indent << "HashLength:               " << this->HashLength;
  os << indent << "Cursor Color:             " 
     << this->CursorColor[0] << "," << this->CursorColor[2] << "," << this->CursorColor[3];
}
