/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkImageDrawObjects - Abstract Filter used in slicer to plot graphs
#ifndef __vtkImageDrawObjects_h
#define __vtkImageDrawObjects_h
#include "vtkImageToImageFilter.h"
#include "vtkImageGraph.h"
#include "vtkSlicer.h"

//BTX

// Description:
// Type of Objects:
//  0 = a line 
class VTK_SLICER_BASE_EXPORT ObjectList : public GraphList {
public:
   ObjectList* GetNext() {return this->Next;} 
   int AddObject(int pos[4],float col[3],int type, int thick);  
   int DeleteObject(int delID);

   void SetPosition(int pos[4]) {memcpy(this->Position,pos,4*sizeof(int));}
   int* GetPosition() {return this->Position;}  

   void SetThickness(int thick) {this->Thickness = thick;}
   int GetThickness() {return this->Thickness;}  
 
   ObjectList* GetObject(int id); 

   ~ObjectList();
   ObjectList();
protected:
   int Position[4];
   int Thickness;
   ObjectList* Next; 
}; 

//ETX
class VTK_SLICER_BASE_EXPORT vtkImageDrawObjects : public vtkImageToImageFilter  {
public:
  static vtkImageDrawObjects *New();
  vtkTypeMacro(vtkImageDrawObjects,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Add an object to the graph. It returns the ID of the data entry back
  int AddObject(int Xpos0, int Ypos0,int Xpos1, int Ypos1, float color0, float color1,float color2,int type, int thick);

  // Description:
  // Deletes an object from the list -> if successfull returns 1 - otherwise 0; 
  int DeleteObject(int id);

  // Description: 
  // Set the color of an object 
  void SetObjectColor(int id, float color0, float color1,float color2);
  float* GetObjectColor(int id);

  // Description: 
  // Set/Get the position of an object 
  void SetObjectPosition(int id, int Xpos0, int Ypos0,int Xpos1, int Ypos1);
  int* GetObjectPosition(int id);

  // Description: 
  // Set/Get the position of an object 
  void SetObjectThickness(int id, int thick);
  int GetObjectThickness(int id);

protected:
  vtkImageDrawObjects() {};
  ~vtkImageDrawObjects() {};

  //void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  // void ExecuteInformation() {this->vtkImageToImageFilter::ExecuteInformation(); };
  void ExecuteData(vtkDataObject *data);

  ObjectList List; 
private:
  vtkImageDrawObjects(const vtkImageDrawObjects&);  // Not implemented.
  void operator=(const vtkImageDrawObjects&);  // Not implemented.
};

#endif

  
