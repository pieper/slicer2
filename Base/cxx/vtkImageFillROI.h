/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkImageFillROI - Paints on top of an image.
// .SECTION Description
// vtkImageFillROI will draw a polygon, line, or points in an
// image.  As opposed to vtkImageDrawROI, which draws only outlines
// around these shapes, this filter will also fill the inside of 
// polygons.
// This filter is used when applying image editing effects, 
// while vtkImageDrawROI is used while the user is actually
// drawing (for temporary interactive display).  So it is 
// important that the output from this filter (vtkImageFIllROI)
// correspond to that of the vtkImageDrawROI filter!
//

#ifndef __vtkImageFillROI_h
#define __vtkImageFillROI_h

#include "vtkImageData.h"
#include "vtkImageInPlaceFilter.h"
#include "vtkPoints.h"
#include "point.h"
#include <string.h>
#include "vtkSlicer.h"

#define SHAPE_POLYGON 1
#define SHAPE_LINES   2
#define SHAPE_POINTS  3

class VTK_SLICER_BASE_EXPORT vtkImageFillROI : public vtkImageInPlaceFilter
{
public:
    static vtkImageFillROI *New();
  vtkTypeMacro(vtkImageFillROI,vtkImageInPlaceFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(Value, float);
    vtkGetMacro(Value, float);

    void SetShapeToPolygon() {this->shape = SHAPE_POLYGON;};
    void SetShapeToLines() {this->shape = SHAPE_LINES;};
    void SetShapeToPoints() {this->shape = SHAPE_POINTS;};
    void SetShape(int s) {this->shape = s;};
    int GetShape() {return this->shape;};
    char *GetShapeString() {switch (this->shape) {
        case SHAPE_POLYGON: return "Polygon";
        case SHAPE_LINES: return "Lines";
        case SHAPE_POINTS: return "Points";
        default: return "None";};};
    void SetShapeString(char *str) {
        if (strcmp(str,"Polygon") == 0) this->SetShapeToPolygon();
        else if (strcmp(str,"Lines") == 0) this->SetShapeToLines();
        else this->SetShapeToPoints();};

    vtkSetMacro(Radius, int);
    vtkGetMacro(Radius, int);

    vtkSetObjectMacro(Points, vtkPoints);
    vtkGetObjectMacro(Points, vtkPoints);

protected:
    vtkImageFillROI();
  ~vtkImageFillROI();
  vtkImageFillROI(const vtkImageFillROI&) {};
    void operator=(const vtkImageFillROI&) {};

    vtkPoints *Points;
    float Value;
    int Radius;
    int shape;

    // not threaded because too simple a filter
    void ExecuteData(vtkDataObject *);
};
#endif
