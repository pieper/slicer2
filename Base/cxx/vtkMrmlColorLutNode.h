/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
// .NAME vtkMrmlColorLutNode - 
// .SECTION Description
// 

#ifndef __vtkMrmlColorLutNode_h
#define __vtkMrmlColorLutNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlColorLutNode : public vtkMrmlNode
{
public:
  static vtkMrmlColorLutNode *New();
  const char *GetClassName() {return "vtkMrmlColorLutNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the nodes attributes to this object
  void Copy(vtkMrmlColorLutNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Get/Set for Name
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

  // Description:
  // Get/Set for FileName
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);

  // Description:
  // Get/Set for HueRange
  vtkGetVector2Macro(HueRange, float);
  vtkSetVector2Macro(HueRange, float);

  // Description:
  // Get/Set for SaturationRange
  vtkGetVector2Macro(SaturationRange, float);
  vtkSetVector2Macro(SaturationRange, float);

  // Description:
  // Get/Set for ValueRange
  vtkGetVector2Macro(ValueRange, float);
  vtkSetVector2Macro(ValueRange, float);

  // Description:
  // Get/Set for AnnoColor
  vtkGetVector3Macro(AnnoColor, float);
  vtkSetVector3Macro(AnnoColor, float);

  // Description:
  // Get/Set for NumberOfColors
  vtkGetMacro(NumberOfColors, int);
  vtkSetMacro(NumberOfColors, int);

private:
  vtkMrmlColorLutNode();
  ~vtkMrmlColorLutNode();
  vtkMrmlColorLutNode(const vtkMrmlColorLutNode&) {};
  void operator=(const vtkMrmlColorLutNode&) {};

  char *Name;
  char *FileName;

  float HueRange[2];
  float SaturationRange[2];
  float ValueRange[2];
  float AnnoColor[3];

  int NumberOfColors;
};

#endif









