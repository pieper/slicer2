/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkBXHParser.h,v $
  Date:      $Date: 2005/12/20 22:54:58 $
  Version:   $Revision: 1.2.8.1 $

=========================================================================auto=*/

#ifndef __vtkBXHParser_h
#define __vtkBXHParser_h


#include <vtkBXHConfigure.h>
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"


class VTK_BXH_EXPORT vtkBXHParser : public vtkXMLDataParser 
{
public: 
    static vtkBXHParser *New();
    vtkTypeMacro(vtkBXHParser, vtkXMLDataParser);

    char *ReadElementValue(vtkXMLDataElement *element);

    vtkBXHParser();
    ~vtkBXHParser();

private:
    char *ElementValue;
};


#endif
