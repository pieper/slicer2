/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkCard.h,v $
  Date:      $Date: 2005/12/20 22:56:14 $
  Version:   $Revision: 1.1.8.1 $

=========================================================================auto=*/
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


// .NAME vtkCard - create a "card" that hold text and image elements.
// .SECTION Description


#ifndef __vtkCard_h
#define __vtkCard_h


#include "vtkObject.h"
#include "vtkTexture.h"
#include "vtkFollower.h"
#include "vtkCollection.h"
#include "vtkRenderer.h"

#include "simpleVectors.h"

#include <vtkQueryAtlasConfigure.h>


class vtkTextureText;
class vtkFollower;
class vtkRectangle;
class vtkCamera;
class vtkActor;
class vtkLineSource;


#define VTK_TEXT_CARD_EDGE_COUNT (8)


class VTK_QUERYATLAS_EXPORT vtkCard : public vtkObject 
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkCard,vtkObject);

    // Description:
    // Manage a set of vtkTextureText objects with a surrounding rectangle-box.
    static vtkCard *New();

    vtkGetObjectMacro(MainText, vtkTextureText);  
    void SetMainText(vtkTextureText *t);

    vtkGetObjectMacro(Box, vtkFollower);  
    vtkGetObjectMacro(BoxRectangle, vtkRectangle);  

    vtkGetMacro(BorderWidth, vtkFloatingPointType);  
    vtkGetMacro(BoxEdgeWidth, vtkFloatingPointType);  
    vtkGetMacro(BoxEdgeBias, vtkFloatingPointType);  
    vtkGetMacro(BoxDistanceZ, vtkFloatingPointType);  

    void SetBorderWidth(vtkFloatingPointType b);
    void SetBoxEdgeWidth(vtkFloatingPointType b);
    void SetBoxEdgeBias(vtkFloatingPointType b);
    void SetBoxDistanceZ(vtkFloatingPointType d);

    vtkGetMacro(UseBoxEdge, bool);
    void SetUseBoxEdge(bool b);

    vtkGetMacro(UseTexturedBoxEdge, bool);
    void SetUseTexturedBoxEdge(bool b);


    // TODO - These default value methods should be a static method
    vtkGetMacro(DefaultBoxDistanceZ, vtkFloatingPointType);
    vtkSetMacro(DefaultBoxDistanceZ, vtkFloatingPointType);

    vtkGetMacro(DefaultBorderWidth, vtkFloatingPointType);
    vtkSetMacro(DefaultBorderWidth, vtkFloatingPointType);

    vtkGetMacro(DefaultBoxEdgeWidth, vtkFloatingPointType);
    vtkSetMacro(DefaultBoxEdgeWidth, vtkFloatingPointType);

    vtkGetMacro(DefaultUseBoxEdge, bool);
    vtkSetMacro(DefaultUseBoxEdge, bool);

    vtkGetMacro(DefaultBoxEdgeBias, vtkFloatingPointType);
    vtkSetMacro(DefaultBoxEdgeBias, vtkFloatingPointType);

    static vtkFloatingPointType DefaultBoxColor[3];
    vtkFloatingPointType BoxColor[3];

    static vtkFloatingPointType DefaultBoxEdgeColor[3];
    vtkFloatingPointType BoxEdgeColor[3];

    static void SetDefaultBoxColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b);
    void SetBoxColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b);

    static void SetDefaultBoxEdgeColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b);
    void SetBoxEdgeColor(vtkFloatingPointType r, vtkFloatingPointType g, vtkFloatingPointType b);

    // TODO - add methods for GetBoxColor, GetDefaultBoxColor, GetBoxEdgeColor, GetDefaultBoxEdgeColor
    // TODO - add methods for SetXXColor using vtkFloatingPointType[3]


    vtkFollower *CreateBox();
    vtkFollower *UpdateBox();
    void CreateBoxEdge();
    void RemoveBoxEdge();

    vtkGetMacro(BoxWidthMinimum, vtkFloatingPointType);
    vtkSetMacro(BoxWidthMinimum, vtkFloatingPointType);
    vtkGetMacro(BoxHeightMinimum, vtkFloatingPointType);
    vtkSetMacro(BoxHeightMinimum, vtkFloatingPointType);


    void SetPosition(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void SetPosition (vtkFloatingPointType _arg[3]);

    //void SetPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void AddPositionOffset(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);


    void SetOffsetActorAndMarker(vtkActor *a, vtkFloatingPointType markerX, vtkFloatingPointType markerY, vtkFloatingPointType markerZ, 
                                          vtkFloatingPointType offX, vtkFloatingPointType offY, vtkFloatingPointType offZ);
    void SetOffsetActorAndMarker(vtkActor *a, vtkActor *m, vtkFloatingPointType offX, vtkFloatingPointType offY, vtkFloatingPointType offZ);
    void SetOffsetActorAndMarker(vtkActor *a, vtkActor *m, vtkFloatingPointType off[3]);    
    void UpdateOffsetActorAndMarker();

    //BTX
    void GetPosition (vtkFloatingPointType data[3]);
    //ETX

    void SetScale(vtkFloatingPointType s);

    void Center();
    void CenterOffset();

    // Description:
    // Call Modified() if a text property, such as position, size, text, etc. has changed.
    // The box will then be updated.
    void Modified();

    void SetOthersVisibility(bool v);
    void OthersVisibilityOn();
    void OthersVisibilityOff();
    vtkGetMacro(OthersVisibility, bool);

    void SetVisibility(bool v);
    void SetOpacityBase(vtkFloatingPointType o);

    void SetCamera(vtkCamera *cam);


    // used mostly so that we can resort the card rendering order
    void RemoveActors(vtkRenderer *r);
    void RemoveActors();
    void AddActors(vtkRenderer *r);
    void AddActors();


    static vtkCollection *GetCards();

    vtkCollection *GetOtherTexts();


    vtkActor *CreateLine(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);

    void SetLinePoint1(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void SetLinePoint1Local(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
    void SetLinePoint2(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);

    vtkGetObjectMacro(LineActor, vtkActor);
    vtkGetObjectMacro(LineSource, vtkLineSource);

    vtkGetObjectMacro(Texture, vtkTexture);
    vtkSetObjectMacro(Texture, vtkTexture);

    vtkGetObjectMacro(Renderer, vtkRenderer);
    vtkSetObjectMacro(Renderer, vtkRenderer);

    vtkGetMacro(LevelNum, int);
    vtkSetMacro(LevelNum, int);
   
    vtkGetMacro(LevelX, vtkFloatingPointType);
    vtkSetMacro(LevelX, vtkFloatingPointType);
   
    void SetDim(bool d);

    //vtkFollower *ImageFollower;
    //vtkGetObjectMacro(ImageFollower, vtkFollower);
    //vtkSetObjectMacro(ImageFollower, vtkFollower);

    // macro wasn't working?
    //vtkGetObjectMacro(ImageFollowers, vtkCollection *);
    vtkCollection *GetImageFollowers();
    vtkCollection *GetImageRectangles();

    static char *LoadString(char *filename);


protected:
    vtkCard();
    ~vtkCard();

    vtkTextureText *MainText;
    vtkFollower *Box;
    vtkRectangle *BoxRectangle;

    // NOTE - using a hardcoded edge count
    vtkFollower *BoxEdge[VTK_TEXT_CARD_EDGE_COUNT];
    vtkRectangle *BoxEdgeRectangle[VTK_TEXT_CARD_EDGE_COUNT];

    static bool DefaultUseBoxEdge;
    bool UseBoxEdge;
    bool UseTexturedBoxEdge;
    int BoxEdgeCount;

    vtkFloatingPointType BoxWidthMinimum;
    vtkFloatingPointType BoxHeightMinimum;

    vtkActor *OffsetActor;
    vtkActor *OffsetMarker;
//BTX
    Vector3D<vtkFloatingPointType> OffsetVector;
    Vector3D<vtkFloatingPointType> OffsetMarkerVector;
//ETX

    vtkFloatingPointType UnscaledBorderWidth;
    vtkFloatingPointType BorderWidth;
    vtkFloatingPointType UnscaledBoxEdgeWidth;
    vtkFloatingPointType BoxEdgeWidth;
    vtkFloatingPointType UnscaledBoxEdgeBias;
    vtkFloatingPointType BoxEdgeBias;
    vtkFloatingPointType UnscaledBoxDistanceZ;
    vtkFloatingPointType BoxDistanceZ;

    vtkFloatingPointType ScaleFactor;

    static vtkFloatingPointType DefaultBoxDistanceZ;
    static vtkFloatingPointType DefaultBorderWidth;
    static vtkFloatingPointType DefaultBoxEdgeWidth;
    static vtkFloatingPointType DefaultBoxEdgeBias;

    bool OthersVisibility;

    vtkCollection *OtherTexts;

    // NOTE - The ImageFollower and it's Rectangle must be added in the same order
    // TODO - make an object to hold the two together - could add the Follower to Rectangle for convenience
    vtkCollection *ImageFollowers;
    vtkCollection *ImageRectangles;

    vtkActor *LineActor;
    //Vector3D LineEnd;
    vtkLineSource *LineSource;

    // make mostly transparent - used with SortCommand, so that objects in front of a selected object don't obscure.
    bool Dim;

    vtkFloatingPointType BoxOpacity;
    vtkFloatingPointType TextOpacity;

    vtkTexture *Texture;

    vtkRenderer *Renderer;

    // An extra paramater, e.g. Card Num, or some other parameter - used with BIRNCards.
    int LevelNum;
    vtkFloatingPointType LevelX;


private:
    vtkCard(const vtkCard&);  // Not implemented.
    void operator=(const vtkCard&);  // Not implemented.
};

#endif


