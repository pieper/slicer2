/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
#ifndef __point_h
#define __point_h

class VTK_EXPORT Point
{
public:
    Point() {
        this->x = 0;
        this->y = 0;
        this->selected = 0;
        this->next = NULL;
    }
    Point(int xa, int ya) {
        this->x = xa;
        this->y = ya;
        this->selected = 0;
        this->next = NULL;
    }
    ~Point() {
        this->next = NULL;
    }

    void Select()    {this->selected = 1;}
    void Deselect()  {this->selected = 0;}
    int IsSelected()   {return this->selected;}
    Point *GetNext() {return this->next;}

public:
    int x;
    int y;
    Point *next;

private:
    int selected;
};

#endif

