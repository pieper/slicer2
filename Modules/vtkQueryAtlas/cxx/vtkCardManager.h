/*===========================================================

3D Slicer Software Licence Agreement(c)

Copyright 2003-2004 Massachusetts Institute of Technology 
(MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and 
Women's Hospital, Inc. on behalf of the copyright holders 
and contributors. Permission is hereby granted, without 
payment, to copy, modify, display and distribute this 
software and its documentation, if any, for research 
purposes only, provided that (1) the above copyright notice
and the following four paragraphs appear on all copies of 
this software, and (2) that source code to any 
modifications to this software be made publicly available 
under terms no more restrictive than those in this License 
Agreement. Use of this software constitutes acceptance of 
these terms and conditions.

3D Slicer Software has not been reviewed or approved by 
the Food and Drug Administration, and is for non-clinical, 
IRB-approved Research Use Only. In no event shall data or 
images generated through the use of 3D Slicer Software be 
used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE 
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE 
COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY 
DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS." THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, 
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Authors: Michael McKenna, David Small, Steve Pieper.
    Small Design Firm, Inc.,
    in association with Isomics, Inc.,
    and Partners Health Care.  MA, USA.
    June, 2004

===========================================================*/


// .NAME vtkCardManager - manage a set of vtkCards - load and animate them.
// .SECTION Description


#ifndef __vtkCardManager_h
#define __vtkCardManager_h


#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkSorter.h"

#include <vtkQueryAtlasConfigure.h>


class vtkCollection;

class vtkCard;


class VTK_QUERYATLAS_EXPORT vtkCardManager : public vtkObject
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkCardManager,vtkObject);

    static vtkCardManager *New();

    virtual vtkCard *NewCard();
    virtual void ExtraCardInit(vtkCard *c);
    virtual void ExtraLoadInit();

    virtual void LoadSet();
    void LoadSet(vtkRenderer *ren);
    //void LoadSet(char *dirBase, vtkRenderer *ren);

    //void ActivateLink();

    void SetVisibility(int v);

    vtkCollection *Cards;

    vtkGetMacro(CardZoom, vtkFloatingPointType);
    virtual void SetCardZoom(vtkFloatingPointType c);

    vtkGetMacro(NumCards, int);
    vtkSetMacro(NumCards, int);

    vtkGetObjectMacro(Renderer, vtkRenderer);
    vtkSetObjectMacro(Renderer, vtkRenderer);

    vtkGetObjectMacro(Sorter, vtkSorter);
    vtkSetObjectMacro(Sorter, vtkSorter);

    vtkGetMacro(CardSpacing, vtkFloatingPointType);
    vtkSetMacro(CardSpacing, vtkFloatingPointType);

    vtkGetMacro(ScaleCards, vtkFloatingPointType);
    vtkSetMacro(ScaleCards, vtkFloatingPointType);

    vtkGetMacro(ScaleDownPower, vtkFloatingPointType);
    vtkSetMacro(ScaleDownPower, vtkFloatingPointType);

    vtkGetMacro(ScaleDownFlag, int);
    vtkSetMacro(ScaleDownFlag, int);

    vtkGetMacro(UseFollower, int);
    vtkSetMacro(UseFollower, int);

    //vtkGetStringMacro(DirBase);
    //void SetDirBase(char *);

    //vtkGetStringMacro(SearchString);
    //vtkSetStringMacro(SearchString);



protected:
    vtkCardManager();
    ~vtkCardManager();


    vtkFloatingPointType CardSpacing;
    vtkFloatingPointType CardZoom;
    int NumCards;
    vtkRenderer *Renderer;

    // NOTE - set the Sorter before loading the card set w/ LoadSet()
    vtkSorter *Sorter;

    // param to set before LoadSet to scale all the cards
    vtkFloatingPointType ScaleCards;
    vtkFloatingPointType ScaleDownPower;
    int ScaleDownFlag;

    //char *DirBase;
    //char *SearchString;

    int UseFollower;


private:
    vtkCardManager(const vtkCardManager&);  // Not implemented.
    void operator=(const vtkCardManager&);  // Not implemented.
};

#endif
