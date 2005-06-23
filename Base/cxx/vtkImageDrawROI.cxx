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
#include "vtkImageDrawROI.h"
#include "vtkObjectFactory.h"

#define SET_PIXEL(x, y, color) { ptr = &outPtr[(y)*nxnc + (x)*nc]; \
    ptr[0] = color[0]; ptr[1] = color[1]; ptr[2] = color[2];}

//------------------------------------------------------------------------------
vtkImageDrawROI* vtkImageDrawROI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageDrawROI");
  if(ret)
    {
    return (vtkImageDrawROI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageDrawROI;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageDrawROI::vtkImageDrawROI()
{
    this->HideROIOff();
    
    this->firstPoint = NULL;
    this->lastPoint = NULL;

    this->NumPoints = 0;
    this->NumSelectedPoints = 0;
    this->PointRadius = 1;

    this->PointColor[0] = 1;
    this->PointColor[1] = 0;
    this->PointColor[2] = 0;

    this->SelectedPointColor[0] = 1;
    this->SelectedPointColor[1] = 1;
    this->SelectedPointColor[2] = 0;

    this->LineColor[0] = 1;
    this->LineColor[1] = 0;
    this->LineColor[2] = 0;

    this->drawSelectBox = 0;
    this->sbox.x = 0;
    this->sbox.y = 0;
    this->sbox1.x = 0;
    this->sbox1.y = 0;
    this->sbox2.x = 0;
    this->sbox2.y = 0;

    this->Points = vtkPoints::New();

    this->Shape = ROI_SHAPE_POLYGON;
}

vtkImageDrawROI::~vtkImageDrawROI()
{
    this->Points->Delete();
}

vtkPoints* vtkImageDrawROI::GetPoints()
{
    Point *p = this->firstPoint;

    // count points
    int n=0;
    this->Points->Reset();
    while (p) {
        n++;
        this->Points->InsertNextPoint(p->x, p->y, 0);
        p = p->GetNext();
    }
    return this->Points;
}

void vtkImageDrawROI::AppendPoint(int x, int y)
{
    Point *p = new Point(x, y);

    if (this->firstPoint == NULL) {
        this->firstPoint = p;
    }
    else {
        this->lastPoint->next = p;
    }
    this->lastPoint = p;
    this->NumPoints++;

    this->Modified();
}

static int ClickPoint(Point *p, int x, int y, int r)
{
    if (x >= p->x - r && x <= p->x + r &&
        y >= p->y - r && y <= p->y + r)
        return 1;
    return 0;
}

void vtkImageDrawROI::TogglePoint(int x, int y)
{
    Point *p = this->firstPoint;
    while (p != NULL) {
        if (ClickPoint(p, x, y, this->PointRadius)) {
            if (p->IsSelected()) {
                p->Deselect();
                this->NumSelectedPoints--;
            }
            else {
                p->Select();
                this->NumSelectedPoints++;
            }
            this->Modified();
            return;
        }
        p = p->GetNext();
    }
}

void vtkImageDrawROI::SelectPoint(int x, int y)
{
    Point *p = this->firstPoint;
    while (p != NULL) {
        if (ClickPoint(p, x, y, this->PointRadius)) {
            p->Select();
            this->NumSelectedPoints++;
            this->Modified();
            return;
        }
        p = p->GetNext();
    }
}

void vtkImageDrawROI::DeselectPoint(int x, int y)
{
    Point *p = this->firstPoint;
    while (p != NULL) {
        if (ClickPoint(p, x, y, this->PointRadius)) {
            p->Deselect();
            this->NumSelectedPoints--;
            this->Modified();
            return;
        }
        p = p->GetNext();
    }
}

void vtkImageDrawROI::StartSelectBox(int x, int y)
{
    this->sbox.x = x;
    this->sbox.y = y;
}

void vtkImageDrawROI::DragSelectBox(int x, int y)
{
    this->drawSelectBox = 1;

    // Set sbox (Select Box) points so #1 is lower left
    if (x < this->sbox.x) {
        sbox1.x = x;
        sbox2.x = sbox.x;
    } else {
        sbox1.x = sbox.x;
        sbox2.x = x;
    }
    if (y < this->sbox.y) {
        sbox1.y = y;
        sbox2.y = sbox.y;
    } else {
        sbox1.y = sbox.y;
        sbox2.y = y;
    }
    // Force redraw
    this->Modified();
}

void vtkImageDrawROI::EndSelectBox(int x, int y)
{
    this->drawSelectBox = 0;

    // Set sbox (Select Box) points so #1 is lower left
    if (x < this->sbox.x) {
        sbox1.x = x;
        sbox2.x = sbox.x;
    } else {
        sbox1.x = sbox.x;
        sbox2.x = x;
    }
    if (y < this->sbox.y) {
        sbox1.y = y;
        sbox2.y = sbox.y;
    } else {
        sbox1.y = sbox.y;
        sbox2.y = y;
    }

    // Select all points in box
    int r = this->PointRadius;
    Point *p = this->firstPoint;
    while (p != NULL) {
        if (p->x+r >= sbox1.x && p->x-r <= sbox2.x &&
            p->y+r >= sbox1.y && p->y-r <= sbox2.y)
        {
            if (!p->IsSelected())
            {  
                p->Select();
                this->NumSelectedPoints++;
                this->Modified();
            }
        }
        p = p->GetNext();
    }

    // Force redraw
    this->Modified();
}

void vtkImageDrawROI::SelectAllPoints()
{
    Point *p = this->firstPoint;
    while (p != NULL) {
        p->Select();
        p = p->GetNext();
    }
    this->NumSelectedPoints = this->NumPoints;
    this->Modified();
}

void vtkImageDrawROI::DeselectAllPoints()
{
    Point *p = this->firstPoint;
    while (p != NULL) {
        p->Deselect();
        p = p->GetNext();
    }
    this->NumSelectedPoints = 0;
    this->Modified();
}

// Insert a new point at (x,y) after the last selected point.
// If no points exist, create a new one and select it.
// If points exist, but none are selected, select the last.
void vtkImageDrawROI::InsertAfterSelectedPoint(int x, int y)
{
    Point *p1 = this->firstPoint;
    Point *p2 = NULL;
    Point *sel = NULL;

    // special case of empty list
    if (p1 == NULL) {
        // Add and select new point
        Point *p = new Point(x, y);
        this->firstPoint = p;
        this->lastPoint = p;
        this->NumPoints++;
        p->Select();
        this->NumSelectedPoints++;
        this->Modified();
        return;
    }

    // p1 traverses list
    // p2 points to last point.
    p2 = p1;
    while (p1) {
        if (p1->IsSelected()) {
            sel = p1;
        }
        p2 = p1;
        p1 = p1->GetNext();
    }

    if (!sel) {
        p2->Select();
        sel = p2;
    }

    // Add new point
    p1 = sel;
    p2 = p1->GetNext(); 
    Point *p = new Point(x, y);
    p1->next = p;
    p->next = p2;
    // p2 = NULL if p1 is tail, that's ok
    if (!p2)
        this->lastPoint = p;
    this->NumPoints++;

    // Select the new point instead of any others
    this->DeselectAllPoints();
    p->Select();
    this->NumSelectedPoints++;

    this->Modified();
}

void vtkImageDrawROI::DeleteSelectedPoints()
{
    Point *p1 = this->firstPoint;
    Point *p2;

    if (!p1) return;
    
    // Special case: firstPoint
    while (p1 && p1->IsSelected())
    {
        p2 = p1->GetNext();
        delete p1;
        this->NumPoints--;
        this->NumSelectedPoints--;
        p1 = p2;
        this->firstPoint = p1;
        this->Modified();
    }
    // None left?
    if (p1 == NULL) {
        this->lastPoint = NULL;
        return;
    }

    // Delete points after first point
    p2 = p1->GetNext();
    while (p2) 
    {
        if (p2->IsSelected()) 
        {
            // Are we deleting the last point?
            if (p2->GetNext() == NULL)
            {
                this->lastPoint = p1;

                // If deleting last point, select last point.
                this->lastPoint->Select();

                p1->next = NULL;
                delete p2;
                this->NumPoints--;
                this->NumSelectedPoints--;
                this->Modified();
                return;
            }
            p1->next = p2->GetNext();
            delete p2;
            this->NumPoints--;
            this->NumSelectedPoints--;
            this->Modified();
            p2 = p1->GetNext();
        }
        else {
            p1 = p2;
            if (p2)
                p2 = p2->GetNext();
        }
    }
}

void vtkImageDrawROI::DeleteAllPoints()
{
    Point *p1 = this->firstPoint;
    Point *p2;

    while (p1) {
        p2 = p1->GetNext();
        delete p1;
        p1 = p2;
    }
    this->firstPoint = this->lastPoint = NULL;
    this->NumPoints = this->NumSelectedPoints = 0;

    this->Modified();
}

void vtkImageDrawROI::MoveSelectedPoints(int deltaX, int deltaY)
{
    Point *p = this->firstPoint;
    while (p) {
        if (p->IsSelected()) {
            p->x += deltaX;
            p->y += deltaY;
        }
        p = p->GetNext();
    }
    this->Modified();
}

void vtkImageDrawROI::MoveAllPoints(int deltaX, int deltaY)
{
    Point *p = this->firstPoint;
    while (p) {
        p->x += deltaX;
        p->y += deltaY;
        p = p->GetNext();
    }
    this->Modified();
}

static void ConvertColor(float *f, unsigned char *c)
{
    c[0] = (int)(f[0] * 255.0);
    c[1] = (int)(f[1] * 255.0);
    c[2] = (int)(f[2] * 255.0);
}

// Draw line including first, but not second end point
static void DrawLine(int xx1, int yy1, int xx2, int yy2, unsigned char color[3],
                     unsigned char *outPtr, int pNxnc, int pNc)
{
    unsigned char *ptr;
    int dx, dy, dy2, dx2, r, dydx2;
    int x, y, xInc;
    int nxnc = pNxnc, nc=pNc;
    int x1, y1, x2, y2;

    // Sort points so x1,y1 is below x2,y2
    if (yy1 <= yy2) {
        x1 = xx1;
        y1 = yy1;
        x2 = xx2;
        y2 = yy2;
    } else {
        x1 = xx2;
        y1 = yy2;
        x2 = xx1;
        y2 = yy1;
    }
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    dx2 = dx << 1;
    dy2 = dy << 1;
    if (x1 < x2)
        xInc = 1;
    else
        xInc = -1;
    x = x1;
    y = y1;

    // Horizontal and vertical lines don't need to be special cases,
    // but it will make them faster.

    // Horizontal
    if (dy == 0)
    {
        if (x1 < x2) {
            for (x=x1; x < x2; x++)
                SET_PIXEL(x, y1, color);
        } else {
            for (x=x2; x < x1; x++)
                SET_PIXEL(x, y1, color);
        }
    }
    // Vertical
    else if (dx == 0)
    {
        for (y=y1; y < y2; y++)
            SET_PIXEL(x1, y, color);
    }

    // < 45 degree slope
    else if (dy <= dx)
    {
        dydx2 = (dy-dx) << 1;
        r = dy2 - dx;

        // Draw first point
        SET_PIXEL(x, y, color);

        // Draw up to (not including) end point
        if (x1 < x2)
        {
            while (x < x2)
            {
                x += xInc;
                if (r <= 0)
                    r += dy2;
                else {
                    y++;
                    r += dydx2;
                }
                SET_PIXEL(x, y, color);
            }
        }
        else
        {
            while (x > x2)
            {
                x += xInc;
                if (r <= 0)
                    r += dy2;
                else {
                    y++;
                    r += dydx2;
                }
                SET_PIXEL(x, y, color);
            }
        }
    }

    // > 45 degree slope
    else
    {
        dydx2 = (dx-dy) << 1;
        r = dx2 - dy;

        // Draw first point
        SET_PIXEL(x, y, color);

        // Draw up to (not including) end point
        while (y < y2)
        {
            y++;
            if (r <= 0)
                r += dx2;
            else {
                x += xInc;
                r += dydx2;
            }
            SET_PIXEL(x, y, color);
        }
    }
}

// Draw line including first, but not second end point
static void DrawThickLine(int xx1, int yy1, int xx2, int yy2, 
                          unsigned char color[3],
                          unsigned char *outPtr, int pNxnc, int pNc, int radius)
{
    unsigned char *ptr;
    int r, dx, dy, dy2, dx2, dydx2;
    int x, y, xInc;
    int nxnc = pNxnc, nc=pNc;
    int x1, y1, x2, y2;
    int rad=radius, rx1, rx2, ry1, ry2, rx, ry;

    // Sort points so x1,y1 is below x2,y2
    if (yy1 <= yy2) {
        x1 = xx1;
        y1 = yy1;
        x2 = xx2;
        y2 = yy2;
    } else {
        x1 = xx2;
        y1 = yy2;
        x2 = xx1;
        y2 = yy1;
    }
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    dx2 = dx << 1;
    dy2 = dy << 1;
    if (x1 < x2)
        xInc = 1;
    else
        xInc = -1;
    x = x1;
    y = y1;

    // Draw first point
    rx1 = x - rad; ry1 = y - rad;
    rx2 = x + rad; ry2 = y + rad;
    for (ry=ry1; ry <= ry2; ry++)
        for (rx=rx1; rx <= rx2; rx++)
            SET_PIXEL(rx, ry, color);

    // < 45 degree slope
    if (dy <= dx)
    {
        dydx2 = (dy-dx) << 1;
        r = dy2 - dx;

        // Draw up to (not including) end point
        if (x1 < x2)
        {
            while (x < x2)
            {
                x += xInc;
                if (r <= 0)
                    r += dy2;
                else {
                    // Draw here for a thick line
                    rx1 = x - rad; ry1 = y - rad;
                    rx2 = x + rad; ry2 = y + rad;
                    for (ry=ry1; ry <= ry2; ry++)
                        for (rx=rx1; rx <= rx2; rx++)
                            SET_PIXEL(rx, ry, color);
                    y++;
                    r += dydx2;
                }
                rx1 = x - rad; ry1 = y - rad;
                rx2 = x + rad; ry2 = y + rad;
                for (ry=ry1; ry <= ry2; ry++)
                    for (rx=rx1; rx <= rx2; rx++)
                        SET_PIXEL(rx, ry, color);
            }
        }
        else
        {
            while (x > x2)
            {
                x += xInc;
                if (r <= 0)
                    r += dy2;
                else {
                    // Draw here for a thick line
                    rx1 = x - rad; ry1 = y - rad;
                    rx2 = x + rad; ry2 = y + rad;
                    for (ry=ry1; ry <= ry2; ry++)
                        for (rx=rx1; rx <= rx2; rx++)
                            SET_PIXEL(rx, ry, color);
                    y++;
                    r += dydx2;
                }
                rx1 = x - rad; ry1 = y - rad;
                rx2 = x + rad; ry2 = y + rad;
                for (ry=ry1; ry <= ry2; ry++)
                    for (rx=rx1; rx <= rx2; rx++)
                        SET_PIXEL(rx, ry, color);
            }
        }
    }

    // > 45 degree slope
    else
    {
        dydx2 = (dx-dy) << 1;
        r = dx2 - dy;

        // Draw up to (not including) end point
        while (y < y2)
        {
            y++;
            if (r <= 0)
                r += dx2;
            else {
                // Draw here for a thick line
                rx1 = x - rad; ry1 = y - rad;
                rx2 = x + rad; ry2 = y + rad;
                for (ry=ry1; ry <= ry2; ry++)
                    for (rx=rx1; rx <= rx2; rx++)
                        SET_PIXEL(rx, ry, color);
                x += xInc;
                r += dydx2;
            }
            rx1 = x - rad; ry1 = y - rad;
            rx2 = x + rad; ry2 = y + rad;
            for (ry=ry1; ry <= ry2; ry++)
                for (rx=rx1; rx <= rx2; rx++)
                    SET_PIXEL(rx, ry, color);
        }
    }
}

void vtkImageDrawROI::DrawSelectBox(vtkImageData *outData, int outExt[6])
{
    unsigned char color[3];
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    long nxnc, nc, nx;
    Point p1, p2, p3, p4;
    long xMin, xMax, yMin, yMax;
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;

    ConvertColor(this->SelectedPointColor, color);

    //    s2
    // s1
    //
    // p4 p3
    // p1 p2
    //
    p1.x = sbox1.x;
    p1.y = sbox1.y;
    p2.x = sbox2.x;
    p2.y = sbox1.y;
    p3.x = sbox2.x;
    p3.y = sbox2.y;
    p4.x = sbox1.x;
    p4.y = sbox2.y;

    if (p1.x >= xMin && p1.x <= xMax &&
        p1.y >= yMin && p1.y <= yMax &&
        p2.x >= xMin && p2.x <= xMax &&
        p2.y >= yMin && p2.y <= yMax &&
        p3.x >= xMin && p3.x <= xMax &&
        p3.y >= yMin && p3.y <= yMax &&
        p4.x >= xMin && p4.x <= xMax &&
        p4.y >= yMin && p4.y <= yMax)
    {
        DrawLine(p1.x, p1.y, p2.x, p2.y, color, outPtr, nxnc, nc);
        DrawLine(p2.x, p2.y, p3.x, p3.y, color, outPtr, nxnc, nc);
        DrawLine(p3.x, p3.y, p4.x, p4.y, color, outPtr, nxnc, nc);
        DrawLine(p4.x, p4.y, p1.x, p1.y, color, outPtr, nxnc, nc);
    }
}

void vtkImageDrawROI::DrawLines(vtkImageData *outData, int outExt[6])
{
    unsigned char color[3];
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    int r = this->PointRadius;
    int nx, nc, nxnc;
    Point *p1, *p2;
    long xMin, xMax, yMin, yMax;
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;

    ConvertColor(this->LineColor, color);

    p1 = this->firstPoint;
    if (!p1) return;
    p2 = p1->GetNext();    
    while (p2 != NULL)
    {
        if (this->GetShape() == ROI_SHAPE_POLYGON)
        {
            if (p1->x >= xMin && p1->x <= xMax &&
                p1->y >= yMin && p1->y <= yMax &&
                p2->x >= xMin && p2->x <= xMax &&
                p2->y >= yMin && p2->y <= yMax)
            {
                DrawLine(p1->x, p1->y, p2->x, p2->y, color, outPtr, nxnc, nc);
            }
        }
        else
        {
            if (p1->x-r >= xMin && p1->x+r <= xMax &&
                p1->y-r >= yMin && p1->y+r <= yMax &&
                p2->x-r >= xMin && p2->x+r <= xMax &&
                p2->y-r >= yMin && p2->y+r <= yMax)
            {
                DrawThickLine(p1->x, p1->y, p2->x, p2->y, color, outPtr, 
                    nxnc, nc, r);
            }
        }
        p1 = p2;
        p2 = p1->GetNext();
    }
}

void vtkImageDrawROI::DrawPoints(vtkImageData *outData, int outExt[6])
{
    int x, y, x1, x2, y1, y2;
    unsigned char color[3], *ptr;
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    int r = this->PointRadius;
    Point *p;
    int nx, nc, nxnc;
    int xMin, xMax, yMin, yMax;
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;

    p = this->firstPoint;
    while (p)
    {
        if (p->IsSelected())
            ConvertColor(this->SelectedPointColor, color);
        else
            ConvertColor(this->PointColor, color);

        x1 = p->x - r;
        x2 = p->x + r;
        y1 = p->y - r;
        y2 = p->y + r;
        if (x1 >= xMin && x1 <= xMax && y1 >= yMin && y1 <= yMax &&
            x2 >= xMin && x2 <= xMax && y2 >= yMin && y2 <= yMax)
        {
            for (y=y1; y <= y2; y++)
                for (x=x1; x <= x2; x++)
                    SET_PIXEL(x, y, color);
        }
        p = p->GetNext();
    }
}

//>> AT 01/17/01
void vtkImageDrawROI::DrawCrosses(vtkImageData *outData, int outExt[6])
{
    int x, y, x1, x2, y1, y2;
    unsigned char color[3], *ptr;
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    int r = this->PointRadius;
    Point *p;
    int nx, nc, nxnc;
    int xMin, xMax, yMin, yMax;
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;

    p = this->firstPoint;
    while (p)
    {
        if (p->IsSelected())
            ConvertColor(this->SelectedPointColor, color);
        else
            ConvertColor(this->PointColor, color);

        x1 = p->x - r;
        x2 = p->x + r;
        y1 = p->y - r;
        y2 = p->y + r;
        if (x1 >= xMin && x1 <= xMax && y1 >= yMin && y1 <= yMax &&
            x2 >= xMin && x2 <= xMax && y2 >= yMin && y2 <= yMax)
          {
            for(y = y1; y <= y2; y++)
              SET_PIXEL(p->x, y, color);
            
            for(x = x1; x <= x2; x++)
              SET_PIXEL(x, p->y, color);
          }
        p = p->GetNext();
    }
}
//<< AT 01/17/01

//>> AT 01/19/01
void vtkImageDrawROI::DrawBoxes(vtkImageData *outData, int outExt[6])
{
    int x, y, x1, x2, y1, y2;
    unsigned char color[3], *ptr;
    unsigned char *outPtr = (unsigned char *) \
        outData->GetScalarPointerForExtent(outExt);
    int r = this->PointRadius;
    Point *p;
    int nx, nc, nxnc;
    int xMin, xMax, yMin, yMax;
    xMin = outExt[0];
    xMax = outExt[1];
    yMin = outExt[2];
    yMax = outExt[3];
    nx = outExt[1] - outExt[0] + 1;
    nc = outData->GetNumberOfScalarComponents();
    nxnc = nx*nc;

    p = this->firstPoint;
    while (p)
    {
        if (p->IsSelected())
            ConvertColor(this->SelectedPointColor, color);
        else
            ConvertColor(this->PointColor, color);

        x1 = p->x - r;
        x2 = p->x + r;
        y1 = p->y - r;
        y2 = p->y + r;
        if (x1 >= xMin && x1 <= xMax && y1 >= yMin && y1 <= yMax &&
            x2 >= xMin && x2 <= xMax && y2 >= yMin && y2 <= yMax)
          {
            for(y = y1; y <= y2; y++)
              {
            SET_PIXEL(x1, y, color);
            SET_PIXEL(x2, y, color);
              }
            
            for(x = x1; x <= x2; x++)
              {
            SET_PIXEL(x, y1, color);
            SET_PIXEL(x, y2, color);
              }
          }
        p = p->GetNext();
    }
}
//<< AT 01/19/01

//----------------------------------------------------------------------------
// Description:
// this is cool
void vtkImageDrawROI::ExecuteData(vtkDataObject *out)
{

  // let superclass allocate data
  this->vtkImageInPlaceFilter::ExecuteData(out);

  if ( this->GetInput()->GetDataObjectType() != VTK_IMAGE_DATA )
  { vtkWarningMacro ("was sent non-image data data object");
    return;
  }

  vtkImageData *inData = (vtkImageData *) this->GetInput();
  vtkImageData *outData = this->GetOutput();
  int *outExt = outData->GetWholeExtent();

  int x1, *inExt;
  
    // ensure 3 component data
    x1 = inData->GetNumberOfScalarComponents();
    if (!(x1 == 3 || x1 == 4))
    {
        vtkErrorMacro("Input has "<<x1<<" components instead of 3 or 4.");
        return;
    }

    // Ensure input is unsigned char
    x1 = inData->GetScalarType();
    if (x1 != VTK_UNSIGNED_CHAR)
    {
        vtkErrorMacro("Input is type "<<x1<<" instead of unsigned char.");
        return;
    }

    // Ensure intput is 2D
    inExt = this->GetInput()->GetWholeExtent();
    if (inExt[5] != inExt[4]) {
        vtkErrorMacro("Input must be 2D.");
        return;
    }
  
    // Draw and connect points
    if (!(this->HideROI))
    {
        switch (this->GetShape())
        {
        case ROI_SHAPE_POLYGON:
            DrawLines(outData, outExt);
            DrawPoints(outData, outExt);
            break;
        case ROI_SHAPE_LINES:
            DrawLines(outData, outExt);
            DrawPoints(outData, outExt);
            break;
        case ROI_SHAPE_POINTS:
            DrawPoints(outData, outExt);
            break;
        //>> AT 01/17/01 01/19/01
        case ROI_SHAPE_CROSSES:
          DrawCrosses(outData, outExt);
          break;
        case ROI_SHAPE_BOXES:
          DrawBoxes(outData, outExt);
          break;
        //<< AT 01/17/01 01/19/01
        }
    }

    // Draw Select Box
    if (this->drawSelectBox)
    {
        DrawSelectBox(outData, outExt);
    }
}

void vtkImageDrawROI::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkImageInPlaceFilter::PrintSelf(os,indent);
    this->Points->PrintSelf(os, indent);
        os << indent << "DrawSelectBox: " << this->drawSelectBox;
        os << indent << "sbox x = " << this->sbox.x;
        os << indent << "sbox y = " << this->sbox.y;
        os << indent << "sbox1 x = " << this->sbox1.x;
        os << indent << "sbox1 y = " << this->sbox1.y;
        os << indent << "sbox2 x = " << this->sbox2.x;
        os << indent << "sbox2 y = " << this->sbox2.y;
        os << indent << "NumPoints: " << this->NumPoints;
        os << indent << "NumSelectedPoints: " << this->NumSelectedPoints;
        os << indent << "pointRadius: " << this->PointRadius;
    os << indent << "HideROI: " << this->HideROI;
        os << indent << "Shape: " << this->Shape;
        os << indent << "PointColor[0]: " << this->PointColor[0];
        os << indent << "PointColor[1]: " << this->PointColor[1];
        os << indent << "pointColor[2]: " << this->PointColor[2];
        os << indent << "SelectedPointColor[0]: " << this->SelectedPointColor[0];
        os << indent << "SelectedPointColor[1]: " << this->SelectedPointColor[1];
        os << indent << "SelectedPointColor[2]: " << this->SelectedPointColor[2];
        os << indent << "LineColor[0]: " << this->LineColor[0];
        os << indent << "LineColor[1]: " << this->LineColor[1];
        os << indent << "LineColor[2]: " << this->LineColor[2];
}
