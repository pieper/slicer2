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
// .NAME vtkImageGraph - Abstract Filter used in slicer to plot graphs
#ifndef __vtkImageGraph_h
#define __vtkImageGraph_h
#include "vtkImageSource.h"
#include "vtkImageData.h"
#include "vtkIndirectLookupTable.h"
#include "vtkSlicer.h"

// From vtkImagePlot
#define SET_PIXEL(x,y,color){ ptr  =&outPtr[y*NumXScalar+x*3]; memcpy(ptr,color,3);}


//BTX
class VTK_SLICER_BASE_EXPORT GraphList {
public:
   float* GetColor() {return this->Color;}
   void SetColor (float value[3]) {memcpy(this->Color,value,sizeof(float)*3);}

   void SetType(int val) {this->Type = val; }
   int GetType() {return this->Type; }

   int GetID() {return this->ID;}

  ~GraphList() {this->ID = -1;} 
   GraphList();

   float Color[3];
   int ID;
   int Type;
}; 


// Description:
// Type of curve:
//  0 = curve representing contious data (e.g. /) 
//  1 = curve representing discrete data (e.g. _|) 
class VTK_SLICER_BASE_EXPORT GraphEntryList : public GraphList{
public:
   vtkImageData* GetGraphEntry() {return this->GraphEntry;} 
   void SetGraphEntry(vtkImageData* value) {this->GraphEntry = value;} 

   GraphEntryList* GetNext() {return this->Next;} 

   void SetIgnoreGraphMinGraphMax(bool flag) {this->IgnoreGraphMinGraphMax = flag;} 
   bool GetIgnoreGraphMinGraphMax() {return this->IgnoreGraphMinGraphMax;}

   int AddEntry(vtkImageData* plot, float col[3],int type, bool ignore);  
   int DeleteEntry(int delID);

   GraphEntryList* MatchGraphEntry(vtkImageData* value);

   int GetNumFollowingEntries();

   GraphEntryList();
   ~GraphEntryList();

protected:

   vtkImageData* GraphEntry;
   bool IgnoreGraphMinGraphMax;
   GraphEntryList* Next; 
}; 

//ETX
class VTK_SLICER_BASE_EXPORT vtkImageGraph : public vtkImageSource  {
public:
  static vtkImageGraph *New();
  vtkTypeMacro(vtkImageGraph,vtkImageSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Dimension, int);
  vtkGetMacro(Dimension, int);

  // Description:
  // Length of canvas/graph in X direction
  vtkSetMacro(Xlength, int);
  vtkGetMacro(Xlength, int);

  // Description:
  // Length of canvas/graph in Y direction
  vtkSetMacro(Ylength, int);
  vtkGetMacro(Ylength, int);

  // Description:
  // Global extrema over all the curves/regions 
  vtkGetMacro(GraphMax, float);
  vtkGetMacro(GraphMin, float);

  // Description:
  // Define the value range of the background (i.e. LookupTable)
  vtkSetVector2Macro(DataBackRange, int);
  vtkGetVector2Macro(DataBackRange, int);

  // Defines Background of Graph
  vtkSetObjectMacro(LookupTable,vtkScalarsToColors);
  vtkGetObjectMacro(LookupTable,vtkScalarsToColors);
   
  // Description:
  // Thickness for the curve 
  vtkSetMacro(CurveThickness, int);
  vtkGetMacro(CurveThickness, int);

  // Description: 
  // Ignore the GraphMin - GraphMax setting of the graph
  // This only makes sense in 1D when we plot two curves with independent output range
  // e.g. histogram and gaussian curve.
  void SetIgnoreGraphMinGraphMax(vtkImageData *plot, int flag);
  int GetIgnoreGraphMinGraphMax(vtkImageData *plot);

  // Description: 
  // Set the color of a curve 
  void SetColor(vtkImageData *plot, float color0, float color1,float color2);
  float* GetColor(vtkImageData *plot);

  GraphEntryList* GetGraphList() { return &this->GraphList;}
  // Description:
  // Add a curve or region to the graph. It returns the ID of the data entry back
  int AddCurveRegion(vtkImageData *plot,float color0,float color1,float color2, int type, int ignore);

  // Description:
  // Delete a curve/region from the list -> if successfull returns 1 - otherwise 0; 
  int DeleteCurveRegion(int id);

  // Description:
  // Creates a Indirect Lookup Table just with white color => needed to genereate a simple background  
  vtkIndirectLookupTable* CreateUniformIndirectLookupTable();

  // Description:
  // Creates a Lookup Table - if you want to change the color of the IndirectLookupTable
  // you first have to delete the old LookupTable and than recreate it with this function
  // and assign it again to the IndirectLookupTable
  vtkLookupTable* CreateLookupTable(float SatMin, float SatMax, float ValMin, float ValMax, float HueMin, float HueMax); 

  // Description:
  // Change the color of the Indirect Table 
  void ChangeColorOfIndirectLookupTable(vtkIndirectLookupTable* Table, float SatMin, float SatMax, float ValMin, float ValMax, float HueMin, float HueMax);

  unsigned long GetMTime();
protected:
  vtkImageGraph();
  void DeleteVariables();
  ~vtkImageGraph() {this->DeleteVariables();};
  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *data);

  void Draw1DGraph(vtkImageData *data);
  void Draw2DGraph(vtkImageData *data,int NumRegion, float* CurveRegionMin, float* CurveRegionMax);
  void CalculateGraphMinGraphMax (float* CurveRegionMin, float* CurveRegionMax);
  void DrawBackground(unsigned char *outPtr, int outIncY);

  int Dimension;
  int CurveThickness;
 
  int Xlength;
  int Ylength;

  float GraphMin;
  float GraphMax;

  GraphEntryList GraphList;
 
  vtkScalarsToColors *LookupTable;
  int DataBackRange[2];
private:
  vtkImageGraph(const vtkImageGraph&);  // Not implemented.
  void operator=(const vtkImageGraph&);  // Not implemented.
};

//----------------------------------------------------------------------------
// from vtkImagePlot
inline void ConvertColor(float *f, unsigned char *c)
{
    c[0] = (int)(f[0] * 255.0);
    c[1] = (int)(f[1] * 255.0);
    c[2] = (int)(f[2] * 255.0);
}

//----------------------------------------------------------------------------
inline void DrawThickPoint (int Xpos, int Ypos, unsigned char color[3], unsigned char *outPtr, 
                int NumXScalar, int radius) {
  unsigned char *ptr;
  int x ,y;
  Xpos -= radius;
  for (x = -radius; x <= radius; x++) {
    for (y = -radius; y <= radius; y++) SET_PIXEL(Xpos, y+Ypos, color);
    Xpos ++;
  }
}

//----------------------------------------------------------------------------
inline void DrawContinousLine(int xx1, int yy1, int xx2, int yy2, unsigned char color[3],
                              unsigned char *outPtr, int NumXScalar, int radius) {
  float slope; 
  int Yold = yy1, Ynew, x,y;

  if (xx2 != xx1)  {
    slope = float(yy2 - yy1)/float(xx2 - xx1);
    DrawThickPoint(xx1, yy1, color, outPtr, NumXScalar, radius);
    for(x=xx1+1; x <=  xx2; x++) { 
      Ynew = (int) slope*(x - xx1) + yy1; 
      if (slope < 0) for (y = Yold; y >= Ynew; y --) DrawThickPoint(x, y, color, outPtr, NumXScalar, radius); 
      else  for (y = Yold; y <= Ynew; y ++)  DrawThickPoint(x, y, color, outPtr, NumXScalar, radius);
      Yold = Ynew; 
    } 
  } else {
    if (yy1 > yy2) { 
      for (y = yy2; y <= yy1; y++) DrawThickPoint(xx1, y, color, outPtr, NumXScalar, radius); 
    } else for (y = yy1; y <= yy2; y++) DrawThickPoint(xx1, y, color, outPtr, NumXScalar, radius); 
  }
}
#endif

  
