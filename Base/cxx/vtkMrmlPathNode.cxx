/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlPathNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlPathNode* vtkMrmlPathNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlPathNode");
  if(ret)
  {
    return (vtkMrmlPathNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlPathNode;
}

//----------------------------------------------------------------------------
vtkMrmlPathNode::vtkMrmlPathNode()
{

  // vtkMrmlNode's attributes
  this->ID = 0;
  this->Indent = 1;
  this->Description = NULL;
  this->Options = NULL;

//  this->cPathColor = NULL;
//  this->cLandColor = NULL;
//  this->fPathColor = NULL;
//  this->fLandColor = NULL;

}

//----------------------------------------------------------------------------
vtkMrmlPathNode::~vtkMrmlPathNode()
{

//  if (this->cLandColor)
//    {
//      delete [] this->cLandColor;
//      this->cLandColor = NULL;
//    }
//  if (this->cPathColor)
//    {
//      delete [] this->cPathColor;
//      this->cPathColor = NULL;
//    }
//  if (this->fLandColor)
//    {
//      delete [] this->fLandColor;
//      this->fLandColor = NULL;
//    }
//  if (this->fPathColor)
//    {
//      delete [] this->fPathColor;
//      this->fPathColor = NULL;
//    }
}

//----------------------------------------------------------------------------
void vtkMrmlPathNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Path";

  // Strings
//  if (this->cLandColor && strcmp(this->cLandColor, "")) 
//    {
//      of << " cLandColor='" << this->cLandColor << "'";
//    }
//  if (this->cPathColor && strcmp(this->cPathColor, "")) 
//    {
//      of << " cPathColor='" << this->cPathColor << "'";
//    }
//  if (this->fLandColor && strcmp(this->fLandColor, "")) 
//    {
//      of << " fLandColor='" << this->fLandColor << "'";
//    }
//  if (this->fPathColor && strcmp(this->fPathColor, "")) 
//    {
//      of << " fPathColor='" << this->fPathColor << "'";
//    }
  of << "></Path>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlPathNode::Copy(vtkMrmlPathNode *node)
{
  vtkMrmlNode::Copy(node);

  //Strings, how do we do with color?


}

//----------------------------------------------------------------------------
void vtkMrmlPathNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

//  os << indent << "cLandColor: " <<
//    (this->cLandColor ? this->cLandColor : "(none)") << "\n";
//  os << indent << "cPathColor: " <<
//    (this->cPathColor ? this->cPathColor : "(none)") << "\n";
//  os << indent << "fLandColor: " <<
//    (this->fLandColor ? this->fLandColor : "(none)") << "\n";
//  os << indent << "fPathColor: " <<
//    (this->fPathColor ? this->fPathColor : "(none)") << "\n";

}


