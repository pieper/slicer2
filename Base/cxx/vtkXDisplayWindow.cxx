/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include "vtkXDisplayWindow.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkXDisplayWindow* vtkXDisplayWindow::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkXDisplayWindow");
  if(ret)
    {
    return (vtkXDisplayWindow*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkXDisplayWindow;
}

vtkXDisplayWindow::vtkXDisplayWindow()
{
  this->RenderWindow = NULL;
  this->ImageWindow = NULL;
}

vtkXDisplayWindow::~vtkXDisplayWindow()
{
  if(this->RenderWindow) {
    this->RenderWindow->Delete();
    this->RenderWindow = NULL;
  }
  if(this->ImageWindow) {
    this->ImageWindow->Delete();
    this->ImageWindow = NULL;
  }
}

vtkRenderWindow* vtkXDisplayWindow::GetRenderWindow(int screen)
{
  if (this->RenderWindow != NULL) {
    this->RenderWindow->Delete();
    this->RenderWindow = NULL;
  }
  char str[80];
  sprintf(str, ":0.%d", screen);
  fprintf(stderr, "vtkXDisplayWindow: Creating display '%s'.\n", str);

  this->RenderWindow = vtkRenderWindow::New();
#ifndef _WIN32
  this->RenderWindow->SetDisplayId(XOpenDisplay(str));
#endif
  return this->RenderWindow;
}

vtkImageWindow* vtkXDisplayWindow::GetImageWindow(int screen)
{
  if (this->ImageWindow != NULL) {
    this->ImageWindow->Delete();
    this->ImageWindow = NULL;
  }

  char str[80];
  sprintf(str, ":0.%d", screen);
  fprintf(stderr, "vtkXDisplayWindow: Creating display '%s'.\n", str);

  this->ImageWindow = vtkImageWindow::New();
#ifndef _WIN32
  this->ImageWindow->SetDisplayId(XOpenDisplay(str));
#endif
  return this->ImageWindow;
}

