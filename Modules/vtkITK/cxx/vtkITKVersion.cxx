#ifndef _vtkITKVersion_cxx
#define _vtkITKVersion_cxx

#include "vtkITKVersion.h"

vtkITKVersion* vtkITKVersion::New()
{
    return new vtkITKVersion;
}

char * vtkITKVersion::GetITKVersion()
{
    return ITK_VERSION_STRING;
}

void vtkITKVersion::PrintSelf(ostream& os, vtkIndent indent)
{
}

#endif
