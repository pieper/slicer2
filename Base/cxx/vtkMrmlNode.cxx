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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vtkMrmlNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlNode::vtkMrmlNode()
{
  this->ID = 0;
  
  // By default nodes have no effect on indentation
  this->Indent = 0;

  // Strings
  this->Description = NULL;
  this->Options = NULL;
  this->Title = NULL;

  // By default all MRML nodes have a blank name
  // Must set name to NULL first so that the SetName
  // macro will not free memory.
  this->Name = NULL;
  this->SetName("");
}

//----------------------------------------------------------------------------
vtkMrmlNode::~vtkMrmlNode()
{
  if (this->Description)
    {
      delete [] this->Description;
      this->Description = NULL;
    }
  
  if (this->Options)
    {
      delete [] this->Options;
      this->Options = NULL;
    }
  
  if (this->Name)
    {
      delete [] this->Name;
      this->Name = NULL;
    }
}

// Create a title to be displayed on the GUI.
// Subclasses should override this to display
// something other than ClassNickName: NodeName
// (i.e. Model: Brain)
//----------------------------------------------------------------------------
char *vtkMrmlNode::GetTitle()
{
  char tmp[200], classname[100], nickname[100];
  int len;

  // make sure we have a name
  if (this->Name == NULL) 
    {
      this->SetName("");
    }

  // get full name of class
  strcpy(classname,this->GetClassName());

  // kill the "Node" text at end of class name
  len = strlen(classname) - 4;
  classname[len] = '\0';
  // ignore initial "vtkMrml" characters
  strcpy(nickname, &classname[7]);

  // Create title from current name (if not blank) and class name
  if (strcmp(this->Name, "") == 0) 
    {
      this->SetTitle(nickname);
    }
  else
    {
      sprintf(tmp, "%s: %s", nickname, this->Name);
      this->SetTitle(tmp);
    }

  // return the current title
  return this->Title;
};

//----------------------------------------------------------------------------
void vtkMrmlNode::Copy(vtkMrmlNode *node)
{
  // Copy everything except: ID
  cerr << "The programmer forgot to define a Copy function in some " 
       << "instance of vtkMrmlNode. \n\n";
  exit(-1);
}

//----------------------------------------------------------------------------
void vtkMrmlNode::Write(ofstream& of, int indent)
{
  // Copy everything except: ID
  cerr << "The programmer forgot to define a Write function in some " 
       << "instance of vtkMrmlNode. \n\n";
  exit(-1);
}

//----------------------------------------------------------------------------
void vtkMrmlNode::MrmlNodeCopy(vtkMrmlNode *node)
{
  // Copy everything except: ID
  this->SetDescription(node->Description);
  this->SetOptions(node->Options);
}

//----------------------------------------------------------------------------
void vtkMrmlNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "ID:          " << this->ID << "\n";

  os << indent << "Indent:      " << this->Indent << "\n";

  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";

  os << indent << "Options:     " <<
    (this->Options ? this->Options : "(none)") << "\n";
}

//----------------------------------------------------------------------------
char* vtkMrmlNode::GetMatrixToString(vtkMatrix4x4 *mat)
{
  int i, j;
  float m[16];
  char *s = new char[200];
 
  for (i=0; i<4; i++)
  {
      for (j=0; j<4; j++)
    {
      m[i*4+j] = mat->GetElement(i, j);
    }
  }
  sprintf(s, "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
    m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8],
    m[9], m[10], m[11], m[12], m[13], m[14], m[15]);

  return s;
}

//----------------------------------------------------------------------------
void vtkMrmlNode::SetMatrixToString(vtkMatrix4x4 *mat, char *s)
{
  int i, j;
  float m[16];
  
  sscanf(s, "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
    &m[0], &m[1], &m[2], &m[3], &m[4], &m[5], &m[6], &m[7], &m[8],
    &m[9], &m[10], &m[11], &m[12], &m[13], &m[14], &m[15]);

  for (i=0; i<4; i++)
  {
    for (j=0; j<4; j++)
    {
       mat->SetElement(i, j, m[i*4+j]);
    }
  }
}


