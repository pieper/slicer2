
#ifndef __vtkMrmlTractGroupNode_h
#define __vtkMrmlTractGroupNode_h

#include "vtkDTMRIConfigure.h"
#include "vtkMrmlNode.h"
#include <list>

class VTK_DTMRI_EXPORT vtkMrmlTractGroupNode : public vtkMrmlNode
{
 public:
  static vtkMrmlTractGroupNode *New();
  vtkTypeMacro(vtkMrmlTractGroupNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy to another node of the same type. Not implemented.
  void Copy(vtkMrmlNode *node) {};

  // Description:
  // ID number of this group of tracts.
  vtkGetMacro(TractGroupID, int);
  vtkSetMacro(TractGroupID, int);

  // Description:
  // Put the ID number of this tract onto the list of tracts in this group.
  void AddTractToGroup (int tractID);

 protected:
  vtkMrmlTractGroupNode();
  ~vtkMrmlTractGroupNode();
  vtkMrmlTractGroupNode(const vtkMrmlTractGroupNode&) {};
  void operator=(const vtkMrmlTractGroupNode&) {};


  int TractGroupID;
  //BTX
  std::list< int > *TractIDs;
  //ETX


};

#endif



