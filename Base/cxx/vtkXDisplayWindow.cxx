/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
  this->RenderWindow->SetDisplayId(XOpenDisplay(str));
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
  this->ImageWindow->SetDisplayId(XOpenDisplay(str));
  return this->ImageWindow;
}

