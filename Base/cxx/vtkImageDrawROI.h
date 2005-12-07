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
// .NAME vtkImageDrawROI -  Draw contour on an image
// .SECTION Description
// Drawing and user interaction (select, etc.) for drawing on images

#ifndef __vtkImageDrawROI_h
#define __vtkImageDrawROI_h

#include "vtkImageData.h"
#include "vtkImageInPlaceFilter.h"
#include "vtkPoints.h"
#include "point.h"
#include "vtkSlicer.h"
#include "vtkImageReformat.h"

#define ROI_SHAPE_POLYGON 1
#define ROI_SHAPE_LINES   2
#define ROI_SHAPE_POINTS  3
//>> AT 01/17/01 01/19/01
#define ROI_SHAPE_CROSSES 4
#define ROI_SHAPE_BOXES   5
//<< AT 01/17/01 01/19/01

class VTK_SLICER_BASE_EXPORT vtkImageDrawROI : public vtkImageInPlaceFilter
{
public:    
    static vtkImageDrawROI *New();
  vtkTypeMacro(vtkImageDrawROI,vtkImageInPlaceFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Display user interaction (mainly selection) in image
    void SelectPoint(int x, int y);
    void DeselectPoint(int x, int y);
    void TogglePoint(int x, int y);
    void StartSelectBox(int x, int y);
    void DragSelectBox(int x, int y);
    void EndSelectBox(int x, int y);
    void SelectAllPoints();
    void DeselectAllPoints();
    void AppendPoint(int x, int y);
    void InsertPoint(int x, int y);
    void InsertAfterSelectedPoint(int x, int y);
    void DeleteSelectedPoints();
    void DeleteAllPoints();
    void MoveSelectedPoints(int deltaX, int deltaY);
    void MoveAllPoints(int deltaX, int deltaY);
    int  IsNearSelected(int x, int y);

    void SetClosed(int closed)
    { this->Closed = closed; }

    // Description:
    // Attributes of the poly/line/points drawn in the slice
    vtkSetVector3Macro(PointColor, float);
    vtkGetVectorMacro(PointColor, float, 3);
    vtkSetVector3Macro(SelectedPointColor, float);
    vtkGetVectorMacro(SelectedPointColor, float, 3);
    vtkSetVector3Macro(LineColor, float);
    vtkGetVectorMacro(LineColor, float, 3);

    vtkGetMacro(NumSelectedPoints, int);
    vtkGetMacro(NumPoints, int);

    vtkGetMacro(PointRadius, int);
    vtkSetMacro(PointRadius, int);

    vtkGetMacro(HideROI, int);
    vtkSetMacro(HideROI, int);
    vtkBooleanMacro(HideROI, int);
    void SetImageReformat( vtkImageReformat* ir) {
      this->image_reformat = ir;
    }

    vtkGetMacro(HideSpline, int);
    vtkSetMacro(HideSpline, int);
    vtkBooleanMacro(HideSpline, int);

    // Description:
    // Shape functions affect the way the contour is drawn
    // based on the input points
    void SetShapeToPolygon() {
        this->Shape = ROI_SHAPE_POLYGON; this->Modified();};
    void SetShapeToLines() {
        this->Shape  = ROI_SHAPE_LINES; this->Modified();};
    void SetShapeToPoints() {
        this->Shape = ROI_SHAPE_POINTS; this->Modified();};
    int GetShape() {return this->Shape;};
    // >> AT 01/17/01 01/19/01
    void SetShapeToCrosses() 
      {
        this->Shape = ROI_SHAPE_CROSSES;
        this->Modified();
      }
    void SetShapeToBoxes() 
      {
        this->Shape = ROI_SHAPE_BOXES;
        this->Modified();
      }
    /*char *GetShapeString() {switch (this->Shape) {
        case ROI_SHAPE_POLYGON: return "Polygon";
        case ROI_SHAPE_LINES: return "Lines";
        case ROI_SHAPE_POINTS: return "Points";
        default: return "None";};};*/
    char *GetShapeString()
      {
        switch (this->Shape) {
        case ROI_SHAPE_POLYGON: return "Polygon";
        case ROI_SHAPE_LINES: return "Lines";
        case ROI_SHAPE_POINTS: return "Points";
        case ROI_SHAPE_CROSSES: return "Crosses";
        case ROI_SHAPE_BOXES: return "Boxes";
        default: return "None";}
      }
    // << AT 01/17/01 01/19/01

    // Description:
    // Get the points from the contour drawn on the slice.
    // Used for Apply (to actually mark the points in the volume)
    vtkPoints* GetPoints();
    vtkPoints* GetPointsInterpolated(int density);
    void LoadStackPolygon(vtkPoints* pts);

protected:
        vtkImageDrawROI();
    ~vtkImageDrawROI();
        vtkImageDrawROI(const vtkImageDrawROI&) {};
    void operator=(const vtkImageDrawROI&) {};

    vtkPoints *Points;
    vtkImageReformat* image_reformat;
    vtkPoints *Samples;
    
    Point *firstPoint;
    Point *lastPoint;

    int drawSelectBox;
    Point sbox;
    Point sbox1;
    Point sbox2;

    int NumPoints;
    int NumSelectedPoints;
    int PointRadius;
    int HideROI;
    int HideSpline;
    int Shape;
    int Closed;

    float PointColor[3];
    float SelectedPointColor[3];
    float LineColor[3];

    void DrawPoints(vtkImageData *outData, int extent[6]);
    void DrawLines(vtkImageData *outData, int extent[6]);
    void DrawSelectBox(vtkImageData *outData, int extent[6]);
    //>> AT 01/17/01 01/19/01
    void DrawCrosses(vtkImageData *outData, int extent[6]);
    void DrawBoxes(vtkImageData *outData, int extent[6]);
    //<< AT 01/17/01 01/19/01
    void DrawSpline(vtkImageData *outData, int outExt[6]);
    // Not threaded because its too simple of a filter
    void ExecuteData(vtkDataObject *);
};

#endif

