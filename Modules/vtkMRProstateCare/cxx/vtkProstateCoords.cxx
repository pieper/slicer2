/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkProstateCoords.cxx,v $
  Date:      $Date: 2006/07/20 18:31:05 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkProstateCoords.h"
#include "bio_coords.h"

#include <string>

vtkStandardNewMacro(vtkProstateCoords);

vtkProstateCoords::vtkProstateCoords()
{
}

vtkProstateCoords::~vtkProstateCoords()
{
    if (this->FileNameIn != NULL) 
    {
        delete [] FileNameIn;
    }
    if (this->FileNameOut != NULL) 
    {
        delete [] FileNameOut;
    }
}


int vtkProstateCoords::SetFileName(int in, const char *name)
{
    if (name == NULL)
    {
        vtkErrorMacro( <<"vtkProstateCoords::SetFileName: File name is null.");
        return 1;
    }

    std::string fName(name);
    int size = fName.size();
    if (size == 0)
    {
        vtkErrorMacro( <<"vtkProstateCoords::SetFileName: File name is empty.");
        return 1;
    }

    if (in)
    {
        if (this->FileNameIn == NULL) 
        {
            this->FileNameIn = new char [size];
        }
        strcpy(this->FileNameIn, name);
    } else {
        if (this->FileNameOut == NULL) 
        {
            this->FileNameOut = new char [size];
        }
        strcpy(this->FileNameOut, name);
    }
}


int vtkProstateCoords::Run()
{
    bio_struct bs;

    parse_input(this->FileNameIn, &bs);
    compute_transform(&bs);
    compute_CRD(&bs);
    write_output(this->FileNameOut, &bs);
    free_bs(&bs);

    return 0;
}