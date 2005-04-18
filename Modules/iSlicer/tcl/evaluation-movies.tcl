#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        evaluation-movies.tcl
# PROCEDURES:  
#   eval_movies
#   eval_3d_movie
#   eval_slice_movie
#==========================================================================auto=

#
# ./slicer2-linux-x86 --load-dicom /home/pieper/data/1.2.840.113619.2.5.1762874864.1932.1015502256.640.UID/000004.SER/ --script Modules/iSlicer/tcl/evaluation-movies.tcl --exec "eval_movies /var/tmp/Deface 10 5; exit"

#
#
package require iSlicer

#-------------------------------------------------------------------------------
# .PROC eval_movies
# 
# .ARGS
# path dir
# int steps
# int skip
# .END
#-------------------------------------------------------------------------------
proc eval_movies { {dir /tmp} {steps 120} {skip 1} } {
    
    catch "destroy .eval"
    toplevel .eval
    wm title .eval "Evaluation Movies Render Window"
    wm geometry .eval +0+0

    isvolume .eval.isv
    .eval.isv volmenu_update
    .eval.isv configure -resolution 256 -volume 1
    .eval.isv configure -orientation coronal
    .eval.isv configure -orientation axial
    pack .eval.isv -side left
    pack [is3d .eval.is3d -isvolume .eval.isv -background #000000] -side left

    raise .eval

    if { ![file exists $dir] } {
        file mkdir $dir
    }


    eval_3d_movie $dir $steps
    eval_slice_movie $dir axial $skip
    eval_slice_movie $dir sagittal $skip
    eval_slice_movie $dir coronal $skip

    catch "destroy .eval"
}

#-------------------------------------------------------------------------------
# .PROC eval_3d_movie
# 
# .ARGS
# path dir
# int steps
# .END
#-------------------------------------------------------------------------------
proc eval_3d_movie { dir steps } {

    #
    # render and encode the face volume
    #

    set delta [expr 360. / $steps]

    set f 0
    for {set l 0} {$l <= 360} { set l [expr $l + $delta] } {
        puts -nonewline "$f..." ; flush stdout
        .eval.is3d configure -longitude $l
        .eval.is3d expose
        update
        .eval.is3d screensave [format $dir/face-%04d.png $f] PNG
        incr f
    }
}

#-------------------------------------------------------------------------------
# .PROC eval_slice_movie
# 
# .ARGS
# path dir
# int orientation
# int skip
# .END
#-------------------------------------------------------------------------------
proc eval_slice_movie { dir orientation { skip 1 } } {

    #
    # render and encode the slice movies
    #

    .eval.isv configure -orientation $orientation
    .eval.isv expose
    update

    for {set s 0} {$s <= 256} { incr s $skip } {
        puts -nonewline "$s..." ; flush stdout
        .eval.isv configure -slice $s
        update
        .eval.isv screensave [format $dir/slices-$orientation-%04d.png $s] PNG
    }
}

