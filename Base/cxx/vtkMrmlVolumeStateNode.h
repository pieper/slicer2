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
// .NAME vtkMrmlVolumeStateNode - MRML node to save volume options.
// .SECTION Description
// Volume State nodes save options of a referenced volume node. Options are
// things like the LUT, opacity and if the volume is faded.

#ifndef __vtkMrmlVolumeStateNode_h
#define __vtkMrmlVolumeStateNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeStateNode : public vtkMrmlNode
{
public:
  static vtkMrmlVolumeStateNode *New();
  vtkTypeMacro(vtkMrmlVolumeStateNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // ID of the referenced volume
  vtkSetStringMacro(VolumeRefID);
  vtkGetStringMacro(VolumeRefID);

  // Description:
  // Color look up table
  vtkSetStringMacro(ColorLUT);
  vtkGetStringMacro(ColorLUT);

  // Description:
  // Is the volume shown in the foreground?
  vtkBooleanMacro(Foreground,int);
  vtkSetMacro(Foreground,int);
  vtkGetMacro(Foreground,int);

  // Description:
  // Is the volume shown in the background?
  vtkBooleanMacro(Background,int);
  vtkSetMacro(Background,int);
  vtkGetMacro(Background,int);

  // Description:
  // Is the volume faded?
  vtkBooleanMacro(Fade,int);
  vtkSetMacro(Fade,int);
  vtkGetMacro(Fade,int);
  
  // Description:
  // Opacity of the volume
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);
 
protected:
  vtkMrmlVolumeStateNode();
  ~vtkMrmlVolumeStateNode();
  vtkMrmlVolumeStateNode(const vtkMrmlVolumeStateNode&) {};
  void operator=(const vtkMrmlVolumeStateNode&) {};

  // Strings
  char *VolumeRefID;
  char *ColorLUT;
  
  // Booleans
  int Foreground;
  int Background;
  int Fade;
  
  // Numbers
  float Opacity;

};

#endif
