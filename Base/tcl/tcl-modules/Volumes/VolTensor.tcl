#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        VolTensor.tcl
# PROCEDURES:  
#   VolTensorInit
#   VolTensorBuildGui
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC VolTensorInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolTensorInit {} {
    global Volume

    # Define Procedures for communicating with Volumes.tcl
    #---------------------------------------------
    set m VolTensor
    
    # procedure for building GUI in this module's frame
    set Volume(readerModules,$m,procGUI)  ${m}BuildGUI

    # Define Module Description to be used by Volumes.tcl
    #---------------------------------------------

    # name for menu button
    set Volume(readerModules,$m,name)  Tensor

    # tooltip for help
    set Volume(readerModules,$m,tooltip)  \
            "This tab displays information\n
    for the currently selected diffusion tensor volume."

    # Global variables used inside this module
    #---------------------------------------------
    set Volume(tensors,pfSwap) 0
    set Volume(tensors,DTIdata) 0  
}

#-------------------------------------------------------------------------------
# .PROC VolTensorBuildGui
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolTensorBuildGui {} {

    puts "Hello World!!!"
}
