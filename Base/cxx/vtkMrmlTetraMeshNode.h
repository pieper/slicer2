/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkMrmlTetraMeshNode - MRML node for representing a TetraMesh.
// .SECTION Description
// TetraMesh nodes describe data sets that can be thought of as stacks of 2D 
// images that form a 3D TetraMesh.  TetraMesh nodes describe where the images 
// are stored on disk, how to render the data (window and level), and how 
// to read the files.  This information is extracted from the image 
// headers (if they exist) at the time the MRML file is generated.  
// Consequently, MRML files isolate MRML browsers from understanding how 
// to read the myriad of file formats for medical data. 

#ifndef __vtkMrmlTetraMeshNode_h
#define __vtkMrmlTetraMeshNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlTetraMeshNode : public vtkMrmlNode
{
  public:
  static vtkMrmlTetraMeshNode *New();
  vtkTypeMacro(vtkMrmlTetraMeshNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // TetraMesh ID
  vtkSetStringMacro(TetraMeshID);
  vtkGetStringMacro(TetraMeshID);
  
  // Description:
  // The FileName to save the data to
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Numerical ID of the color lookup table to use for rendering the TetraMesh
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

protected:
  vtkMrmlTetraMeshNode();
  ~vtkMrmlTetraMeshNode();
  vtkMrmlTetraMeshNode(const vtkMrmlTetraMeshNode&) {};
  void operator=(const vtkMrmlTetraMeshNode&) {};

  void ComputeRasToIjk(vtkMatrix4x4 *matRotate, 
    float ox, float oy, float oz);

  // Strings
  char *TetraMeshID;
  char *FileName;
  char *LUTName;

  // Numbers
  unsigned short DisplaySurfaces;
  unsigned short DisplayEdges;
  unsigned short DisplayNodes;
  float            NodeScaling;
  unsigned short   NodeSkip;
  unsigned short DisplayScalars;
  float            ScalarScaling;
  unsigned short   ScalarSkip;
  unsigned short DisplayVectors;
  float            VectorScaling;
  unsigned short   VectorSkip;
};

#endif

