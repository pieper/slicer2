/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
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
// .NAME vtkMrmlOptionsNode - MRML node for storing browser-specific data.
// .SECTION Description
// Option nodes allow browser-specific information to be stored in
// a MRML file.  For example, the 3D Slicer uses Option nodes to store
// the user's 3D viewpoint information since there currently is no
// View node in MRML2.0.

#ifndef __vtkMrmlOptionsNode_h
#define __vtkMrmlOptionsNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlOptionsNode : public vtkMrmlNode
{
public:
  static vtkMrmlOptionsNode *New();
  vtkTypeMacro(vtkMrmlOptionsNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object
  void Copy(vtkMrmlOptionsNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Name of the program that should recognize these options.
  vtkGetStringMacro(Program);
  vtkSetStringMacro(Program);

  // Description:
  // Short description of the contents of this node. ie: 'presets'
  vtkGetStringMacro(Contents);
  vtkSetStringMacro(Contents);

  // Description:
  // List of options expressed in this form: key1='value1' key2='value2'
  vtkGetStringMacro(Options);
  vtkSetStringMacro(Options);

protected:
  vtkMrmlOptionsNode();
  ~vtkMrmlOptionsNode();
  vtkMrmlOptionsNode(const vtkMrmlOptionsNode&) {};
  void operator=(const vtkMrmlOptionsNode&) {};

  // Description:
  // Contains information for use by specified program
  char *Options;
  // Description:
  // Name of program that uses these options (i.e. Slicer)
  char *Program;
  // Description:
  // Type of Options stored in this node (i.e. user preferences)
  char *Contents;
};

#endif









