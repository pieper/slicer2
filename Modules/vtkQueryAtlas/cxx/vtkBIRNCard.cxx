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


#include "vtkBIRNCard.h"
#include "vtkRectangle.h"
#include "vtkTextureText.h"

#include "vtkFollower.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLineSource.h"
#include "vtkCommand.h"
#include "vtkJPEGReader.h"


vtkCxxRevisionMacro(vtkBIRNCard, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkBIRNCard);


vtkBIRNCard::vtkBIRNCard()
{
    this->SearchString = NULL;
    this->DirBase = NULL;
    this->DirName = NULL;
}


vtkBIRNCard::~vtkBIRNCard() {
}


void vtkBIRNCard::CreateCard() {
    vtkFloatingPointType textScale = 1.0;
    vtkFloatingPointType spacerY = 0.8;
    vtkFloatingPointType cardWidth = 12.0;
    vtkTextureText *tt;
    vtkFollower *followerText;
    vtkFloatingPointType imageScale = 1.0;

    // TODO - char limit - do a test?  do a dynamic alloc? (would need to free)
    char dir[BIRN_PATH_LEN];
    char filename[BIRN_PATH_LEN];

    if (this->DirBase == NULL) {
        vtkErrorMacro("vtkBIRNCard::CreateCard(): Error - DirBase has not been set\n");
        return;
    }

    // READ DATA FILES


    sprintf(dir, "%slevel%d/", this->DirBase, this->LevelNum);

    this->DirName = strdup(dir);

    sprintf(filename, "%sLevel.txt", dir);
    char *levelName = LoadString(filename);

    sprintf(filename, "%sName.txt", dir);
    char *nameName = LoadString(filename);

    sprintf(filename, "%sDetails.txt", dir);
    char *detailsName = LoadString(filename);

    sprintf(filename, "%sColor.txt", dir);
    char *colorString= LoadString(filename);
    vtkFloatingPointType r, g, b;
    sscanf(colorString, "%f %f %f", &r, &g, &b);
    SetBoxEdgeColor(r / 255., g / 255., b / 255.);

    // v44 - thicker edge
    this->SetBoxEdgeWidth(.4);
    
    // v53 - edge was out a little too far - could see a gap
    //this->SetBoxEdgeBias(.35);
    this->SetBoxEdgeBias(.3);

    // image loading support

    vtkJPEGReader *jpegReader;
    vtkTexture *tex;
    vtkRectangle *plane;
    vtkPolyDataMapper *planeMapper;
    vtkFollower *planeActor;


    // LEVEL ICON IMAGE

    jpegReader = vtkJPEGReader::New();
    sprintf(filename, "%sicons/level%d.jpg", this->DirBase, this->LevelNum);
    jpegReader->SetFileName(filename);
    tex = vtkTexture::New();
    tex->InterpolateOn();
    tex->SetInput(jpegReader->GetOutput());

    plane = vtkRectangle::New();
    planeMapper = vtkPolyDataMapper::New();
    planeMapper->SetInput(plane->GetOutput());
    planeActor = vtkFollower::New();
    planeActor->SetMapper(planeMapper);
    planeActor->SetTexture(tex);
    planeActor->GetProperty()->SetOpacity(0.99);

    imageScale = 1.6;
    vtkFloatingPointType levelIconX = cardWidth - imageScale - .2;
    plane->SetPositionOffset(levelIconX, -imageScale + .55, 0);


    //plane->SetScalePoints(imageScale, imageScale);
    plane->SetWidth(imageScale);
    plane->SetHeight(imageScale);

    ImageFollowers->AddItem(planeActor);
    ImageRectangles->AddItem(plane);


    // TITLE TEXT

    textScale = 1.0;

    // NOTE - reusing 'filename' for convenience.
    sprintf(filename, "Level %d - %s", this->LevelNum + 1, levelName);


    tt = vtkTextureText::New();
    tt->GetFontParameters()->SetBlur(2);
    tt->SetText(filename);


    //tt->SetBoxSize(cardWidth / textScale, 100);
    tt->SetBoxWidth((levelIconX - .25) / textScale);
    tt->SetBoxHeight(100);
    tt->WrappedOn();


    tt->CreateTextureText();


    SetMainText(tt);



    tt->GetFollower()->SetScale(textScale, textScale, 1);
    tt->GetFollower()->GetProperty()->SetColor(0, 0, 0);



    followerText = tt->GetFollower();
    vtkFloatingPointType bounds[6];
    vtkFloatingPointType flowY = 0;
    followerText->GetBounds(bounds);
    flowY = bounds[2] - spacerY;



    SetBoxWidthMinimum(cardWidth);


    // DESCRIPTION TEXT

    textScale = .8;
    // TODO - char limit - this is just for a linefeed add-on?  don't even need that anymore w/o the centering
    char desc[2000];
    // need a linefeed for centering to work
    sprintf(desc, "%s\n", nameName);


    tt = vtkTextureText::New();
    tt->GetFontParameters()->SetFontFileName("ARIALI.TTF");
    tt->GetFontParameters()->SetBlur(2);
    tt->SetText(desc);
    

    //tt->SetBoxSize(cardWidth / textScale, 100);
    tt->SetBoxWidth((levelIconX - .25) / textScale);
    tt->SetBoxHeight(100);
    tt->WrappedOn();
    //tt->SetAlignment(VTK_TEXT_ALIGNMENT_CENTERED);

    tt->CreateTextureText();


    tt->GetFollower()->SetScale(textScale, textScale, 1);
    tt->GetFollower()->GetProperty()->SetColor(.5, 0, 0);
    // v52 OLD: use SetPositionOffset after the scale, since it needs that info (or do a Modified() later)
    
    // v52 - SetPositionOffset now in local space!
    //tt->SetPositionOffset(0, flowY, 0);
    tt->SetPositionOffset(0, flowY / textScale, 0);
    GetOtherTexts()->AddItem(tt);

    followerText = tt->GetFollower();
    followerText->GetBounds(bounds);
    // image will be closer - scale spacerY down some
    flowY = bounds[2] - spacerY * .5;



    // MAIN IMAGE

    jpegReader = vtkJPEGReader::New();
    sprintf(filename, "%sImage.jpg", dir);
    jpegReader->SetFileName(filename);
    tex = vtkTexture::New();
    tex->InterpolateOn();
    tex->SetInput(jpegReader->GetOutput());
    //jpegReader->GetOutput()->Print(cout);

    plane = vtkRectangle::New();
    planeMapper = vtkPolyDataMapper::New();
    planeMapper->SetInput(plane->GetOutput());
    planeActor = vtkFollower::New();
    planeActor->SetMapper(planeMapper);
    planeActor->SetTexture(tex);
    planeActor->GetProperty()->SetOpacity(0.99);

    imageScale = cardWidth * 0.8;
    //plane->SetPositionOffset(1, -imageScale - 1.5, 0);
    //plane->SetPositionOffset(1, flowY - imageScale, 0);
    //plane->SetPositionOffset(cardWidth * 0.1, flowY - imageScale, 0);

    // v45 - use the image's aspect ratio
    int exti[6];
    jpegReader->Update();
    jpegReader->GetDataExtent(exti);
    vtkFloatingPointType ar = (vtkFloatingPointType)exti[1] / (vtkFloatingPointType)exti[3];

    //vtkFloatingPointType imageBottom =  flowY - imageScale;
    vtkFloatingPointType imageBottom =  flowY - imageScale / ar;

    plane->SetPositionOffset(0.1, imageBottom, 0);

    //plane->SetScalePoints(imageScale, imageScale);

    //plane->SetScalePoints(imageScale, imageScale / ar);
    plane->SetWidth(imageScale);
    plane->SetHeight(imageScale / ar);

    ImageFollowers->AddItem(planeActor);
    ImageRectangles->AddItem(plane);

    planeActor->GetBounds(bounds);
    // don't update the flowY till after the next icon is placed
    //flowY = bounds[2] - spacerY;
    vtkFloatingPointType nextFlowY = bounds[2] - spacerY;


    // SPECIES ICON IMAGE

    sprintf(filename, "%sSpecies.txt", dir);
    char *speciesLoad = LoadString(filename);
    // TODO - char limit
    char speciesString[200];
    sscanf(speciesLoad, "%s", speciesString);

    jpegReader = vtkJPEGReader::New();
    if (strcmp(speciesString, "human") == 0) sprintf(filename, "%sicons/HumanIcon.jpg", this->DirBase);
    else sprintf(filename, "%sicons/MouseIcon.jpg", this->DirBase);
    jpegReader->SetFileName(filename);
    tex = vtkTexture::New();
    tex->InterpolateOn();
    tex->SetInput(jpegReader->GetOutput());

    plane = vtkRectangle::New();
    planeMapper = vtkPolyDataMapper::New();
    planeMapper->SetInput(plane->GetOutput());
    planeActor = vtkFollower::New();
    planeActor->SetMapper(planeMapper);
    planeActor->SetTexture(tex);
    planeActor->GetProperty()->SetOpacity(0.99);

    imageScale = 1.6;
    levelIconX = cardWidth - imageScale - .2;
    plane->SetPositionOffset(levelIconX, imageBottom, 0);
    //plane->SetScalePoints(imageScale, imageScale);
    plane->SetWidth(imageScale);
    plane->SetHeight(imageScale);

    ImageFollowers->AddItem(planeActor);
    ImageRectangles->AddItem(plane);

    // increment past the main image now
    flowY = nextFlowY;



    // ADDITIONAL TEXTS

    textScale = 0.6;
    sprintf(desc, "%s\n", detailsName);

    tt = vtkTextureText::New();
    tt->GetFontParameters()->SetFontFileName("EUROSTAR.TTF");
    tt->GetFontParameters()->SetBlur(2);
    tt->SetText(detailsName);

    tt->SetBoxWidth(cardWidth / textScale);
    tt->SetBoxHeight(100);
    tt->WrappedOn();

    tt->CreateTextureText();


    tt->GetFollower()->SetScale(textScale, textScale, 1);
    tt->GetFollower()->GetProperty()->SetColor(0, 0, 0);

    // v52 - SetPositionOffset now in local space!
    tt->SetPositionOffset(0, flowY / textScale, 0);
    GetOtherTexts()->AddItem(tt);

    followerText = tt->GetFollower();
    followerText->GetBounds(bounds);
    flowY = bounds[2] - spacerY;


    if (this->SearchString != NULL) {
        textScale = 0.6;
        tt = vtkTextureText::New();
        tt->GetFontParameters()->SetBlur(2);
        tt->SetText(this->SearchString);

        tt->SetBoxWidth(cardWidth / textScale);
        tt->SetBoxHeight(100);
        tt->WrappedOn();
        //tt->SetAlignment(VTK_TEXT_ALIGNMENT_FLUSH_RIGHT);

        tt->CreateTextureText();


        tt->GetFollower()->SetScale(textScale, textScale, 1);
        tt->GetFollower()->GetProperty()->SetColor(0, 0, 0);

        // v52 - SetPositionOffset now in local space!
        tt->SetPositionOffset(0, flowY / textScale, 0);
        GetOtherTexts()->AddItem(tt);

        followerText = tt->GetFollower();
        followerText->GetBounds(bounds);
        flowY = bounds[2] - spacerY;
    }

    AddActors(this->Renderer);
}


void vtkBIRNCard::ActivateLink() {
    char filename[BIRN_PATH_LEN];
    sprintf(filename, "%sLink.mht", DirName);
    printf("vtkBIRNCard::ActivateLink: %d,  %s\n", this->LevelNum, filename);

    // TODO MED - need a Solaris equivelant for ShellExecute - exec?
    // remove for now
    //ShellExecute(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);
}


void vtkBIRNCard::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    /*  TODO - do print
    os << indent << "X Resolution: " << this->XResolution << "\n";
    os << indent << "Y Resolution: " << this->YResolution << "\n";

    os << indent << "Origin: (" << this->Origin[0] << ", "
    << this->Origin[1] << ", "
    << this->Origin[2] << ")\n";
    ...
    */
}

