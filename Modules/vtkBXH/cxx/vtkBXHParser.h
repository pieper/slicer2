
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
