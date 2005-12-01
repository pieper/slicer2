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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlModelNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlModelNode* vtkMrmlModelNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlModelNode");
  if(ret)
  {
    return (vtkMrmlModelNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlModelNode;
}

//----------------------------------------------------------------------------
vtkMrmlModelNode::vtkMrmlModelNode()
{
  // Strings
  this->ModelID = NULL;
  this->FileName = NULL;
  this->Color = NULL;
  this->FullFileName = NULL;

  // Numbers
  this->Opacity = 1.0;
  this->Visibility = 1;
  this->Clipping = 0;
  this->BackfaceCulling = 1;
  this->ScalarVisibility = 0;
  this->VectorVisibility = 0;
  this->TensorVisibility = 0;
  
  // Arrays
  this->ScalarRange[0] = 0;
  this->ScalarRange[1] = 100;

  this->RasToWld = vtkMatrix4x4::New();

  // Scalars
  this->LUTName = "-1";
  this->ScalarFileNamesVec.clear();
  
}

//----------------------------------------------------------------------------
vtkMrmlModelNode::~vtkMrmlModelNode()
{
  this->RasToWld->Delete();

  if (this->ModelID)
  {
    delete [] this->ModelID;
    this->ModelID = NULL;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
  if (this->FullFileName)
  {
    delete [] this->FullFileName;
    this->FullFileName = NULL;
  }
  if (this->Color)
  {
    delete [] this->Color;
    this->Color = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Model";

  // Strings
  if (this->ModelID && strcmp(this->ModelID, ""))
  {
    of << " id='" << this->ModelID << "'";
  }
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->FileName && strcmp(this->FileName, "")) 
  {
    of << " fileName='" << this->FileName << "'";
  }
  if (this->Color && strcmp(this->Color, "")) 
  {
    of << " color='" << this->Color << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  if (this->LUTName && strcmp(this->LUTName,""))
  {
      of << " lutName='" << this->LUTName << "'";
  }
  
  // Numbers
  if (this->Opacity != 1.0)
  {
    of << " opacity='" << this->Opacity << "'";
  }
  if (this->Visibility != 1)
  {
    of << " visibility='" << (this->Visibility ? "true" : "false") << "'";
  }
  if (this->Clipping != 0)
  {
    of << " clipping='" << (this->Clipping ? "true" : "false") << "'";
  }
  if (this->BackfaceCulling != 1)
  {
    of << " backfaceCulling='" << (this->BackfaceCulling ? "true" : "false") << "'";
  }
  if (this->ScalarVisibility != 0)
  {
    of << " scalarVisibility='" << (this->ScalarVisibility ? "true" : "false") << "'";
  }

  // Arrays
  if (this->ScalarRange[0] != 0 || this->ScalarRange[1] != 100)
  {
    of << " scalarRange='" << this->ScalarRange[0] << " "
       << this->ScalarRange[1] << "'";
  }

  // Scalars
  if (this->ScalarFileNamesVec.size() > 0)
  {
      of << " scalarFiles='";
      for (int idx = 0; idx < this->ScalarFileNamesVec.size(); idx++)
      {
          of << this->GetScalarFileName(idx);
          if (idx+1 < this->ScalarFileNamesVec.size())
          {
              of << " ";
          }
      }
      of << "'";
  }
  of << "></Model>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ModelID
void vtkMrmlModelNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlModelNode *node = (vtkMrmlModelNode *) anode;

  // Strings
  this->SetFileName(node->FileName);
  this->SetFullFileName(node->FullFileName);
  this->SetColor(node->Color);

  // Vectors
  this->SetScalarRange(node->ScalarRange);
  
  // Numbers
  this->SetOpacity(node->Opacity);
  this->SetVisibility(node->Visibility);
  this->SetScalarVisibility(node->ScalarVisibility);
  this->SetBackfaceCulling(node->BackfaceCulling);
  this->SetClipping(node->Clipping);

  // Matrices
  this->SetRasToWld(node->RasToWld);

  // Scalars

}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::SetRasToWld(vtkMatrix4x4 *rasToWld)
{
  this->RasToWld->DeepCopy(rasToWld);
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "ModelID: " <<
    (this->ModelID ? this->ModelID : "(none)") << "\n";
  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "FullFileName: " <<
    (this->FullFileName ? this->FullFileName : "(none)") << "\n";
  os << indent << "Color: " <<
    (this->Color ? this->Color : "(none)") << "\n";

  os << indent << "Opacity:           " << this->Opacity << "\n";
  os << indent << "Visibility:        " << this->Visibility << "\n";
  os << indent << "ScalarVisibility:  " << this->ScalarVisibility << "\n";
  os << indent << "BackfaceCulling:   " << this->BackfaceCulling << "\n";
  os << indent << "Clipping:          " << this->Clipping << "\n";

  os << "ScalarRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->ScalarRange[idx];
  }
  os << ")\n";

  // Matrices
  os << indent << "RasToWld:\n";
    this->RasToWld->PrintSelf(os, indent.GetNextIndent());

    os << indent << "Look up table ID: " << this->LUTName << endl;

    // Scalars
    os << indent << "Number of scalar file names: " << this->ScalarFileNamesVec.size() << endl;
    if (this->ScalarFileNamesVec.size() > 0)
    {
        for (idx = 0; idx < this->ScalarFileNamesVec.size(); idx++)
        {
            os << indent << indent << "Scalar File " << idx << ": " << this->ScalarFileNamesVec[idx].c_str() << endl;
        }
    }
}

//----------------------------------------------------------------------------
int vtkMrmlModelNode::GetNumberOfScalarFileNames ()
{
    return this->ScalarFileNamesVec.size();
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::AddScalarFileName(char *newFileName)
{
    char tmpStr[1024];
    int i;
    int found = 0;
    sscanf(newFileName, "%s", tmpStr);
    std::string val = tmpStr;
    // check that it's not there already
    for (i=0; i<this->ScalarFileNamesVec.size(); i++)
    {
        if (this->ScalarFileNamesVec[i] == tmpStr)
        {
            found++;
        }
    }
    if (found == 0)
    {
        this->ScalarFileNamesVec.push_back(val);
        vtkDebugMacro(<<"Added scalar file " << newFileName);
    }
    else
    {
        vtkDebugMacro(<<"Didn't add scalar file name, found in list already: " << newFileName);
    }
                    
}

//----------------------------------------------------------------------------
const char *vtkMrmlModelNode::GetScalarFileName(int idx)
{
    return this->ScalarFileNamesVec[idx].c_str();
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::DeleteScalarFileNames()
{
    this->ScalarFileNamesVec.clear();
}
