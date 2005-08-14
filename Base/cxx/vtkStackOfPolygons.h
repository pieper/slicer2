/*=========================================================================

   vtkStackOfPolygons
   Created by Chand T. John for Slicer/NMBL Pipeline

=========================================================================*/
// .NAME vtkStackOfPolygons - represents a sequence of control polygons
// .SECTION Description
// vtkStackOfPolygons is a concrete implementation of the vtkObject class.
// vtkStackOfPolygons represents an ordered sequence of slices, each possibly
// containing multiple control polygons, meant to keep track of all control
// points plotted by the user in Slicer/NMBL Pipeline's Editor->Draw module
// in each slice of data.

#ifndef __vtkStackOfPolygons_h
#define __vtkStackOfPolygons_h

#include "vtkObject.h"
#include "vtkPolygonList.h"
#include "point.h"
#include "vtkSlicer.h"

#define NUM_STACK_SLICES 200

class VTK_SLICER_BASE_EXPORT vtkStackOfPolygons : public vtkObject {
public:
    static vtkStackOfPolygons *New();
    vtkTypeMacro(vtkStackOfPolygons,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Adds polygon to slice s, position p
    void SetPolygon(vtkPoints *polygon, int s, int p, int d, int closed, int preshape, int label);

    // Adds polygon to slice s, first empty position
    void SetPolygon(vtkPoints *polygon, int s, int d);

    // Returns pointer to polygon p of slice s
    vtkPoints* GetPoints(int s, int p);

    // Returns pointer to first nonempty polygon of slice s
    vtkPoints* GetPoints(int s);

    // Returns pointer to sampled polygon of polygon p in slice s.
    vtkPoints* GetSampledPolygon(int s, int p);

    // Returns density of polygon p in slice s.
    int GetDensity(int s, int p);

    // Returns closedness of polygon p in slice s.
    int GetClosed(int s, int p);

    // Returns preshape of polygon p in slice s.
    int GetPreshape(int s, int p);

    // Returns label of polygon p in slice s.
    int GetLabel(int s, int p);

    // Resets polygon p in slice s
    void RemovePolygon(int s, int p);

    // Returns number of points in first nonempty polygon of slice s.
    int GetNumberOfPoints(int s);

    // Returns lowest index in which there is an empty polygon.
    int ListGetInsertPosition(int s);

    // Returns lowest index after p in which there is an empty polygon.
    int ListGetNextInsertPosition(int s, int p);

    // Returns lowest index in which there is a nonempty polygon.
    int ListGetRetrievePosition(int s);

    // Returns lowest index after p in which there is a nonempty polygon.
    int ListGetNextRetrievePosition(int s, int p);

    // Returns number of polygons to apply, next time Apply is clicked.
    int GetNumApplyable(int s);

    // Returns index of qth polygon to apply.
    int GetApplyable(int s, int q);

    // Removes all polygons in the stack
    void Clear();

    // Returns true if any polygon has ever been applied on slice s
    int Nonempty(int s);

    // Returns number of points in polygon p of slice s.
    int GetNumberOfPoints(int s, int p);

protected:
    vtkStackOfPolygons();
    ~vtkStackOfPolygons();
    vtkStackOfPolygons(const vtkStackOfPolygons&) {};
    void operator=(const vtkStackOfPolygons&) {};

    // Store polygon data for up to 200 slices
    vtkPolygonList *PointStack[NUM_STACK_SLICES];

    // IsNonEmpty[s] == false iff no polygon has ever been applied on it
    int IsNonEmpty[NUM_STACK_SLICES];
};

#endif


