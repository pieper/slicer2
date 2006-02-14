/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageFillROI.h,v $
  Date:      $Date: 2006/02/14 20:40:12 $
  Version:   $Revision: 1.17 $

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

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

#define SHAPE_POLYGON 1
#define SHAPE_LINES   2
#define SHAPE_POINTS  3

class VTK_SLICER_BASE_EXPORT vtkImageFillROI : public vtkImageInPlaceFilter
{
public:
    static vtkImageFillROI *New();
    vtkTypeMacro(vtkImageFillROI,vtkImageInPlaceFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(Value, vtkFloatingPointType);
    vtkGetMacro(Value, vtkFloatingPointType);

    void SetShapeToPolygon() {this->shape = SHAPE_POLYGON;};
    void SetShapeToLines() {this->shape = SHAPE_LINES;};
    void SetShapeToPoints() {this->shape = SHAPE_POINTS;};
    void SetShape(int s) {this->shape = s;};
    int GetShape() {return this->shape;};
    const char *GetShapeString() {switch (this->shape) {
        case SHAPE_POLYGON: return "Polygon";
        case SHAPE_LINES: return "Lines";
        case SHAPE_POINTS: return "Points";
        default: return "None";};};
    void SetShapeString(const char *str) {
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
  vtkImageFillROI(const vtkImageFillROI&);
    void operator=(const vtkImageFillROI&);

    vtkPoints *Points;
    vtkFloatingPointType Value;
    int Radius;
    int shape;

    // not threaded because too simple a filter
    void ExecuteData(vtkDataObject *);
};
#endif
