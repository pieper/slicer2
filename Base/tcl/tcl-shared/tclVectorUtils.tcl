#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        tclVectorUtils.tcl
# PROCEDURES:  
#==========================================================================auto=
package provide tclVectorUtils 1.0

namespace eval tclVectorUtils {
    proc VCross {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a1*$b2 - $a2*$b1}] \
                [expr {$a2*$b0 - $a0*$b2}] \
                [expr {$a0*$b1 - $a1*$b0}]]
    }

    proc VDist {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}
        return [expr {sqrt(pow($a0-$b0,2) + pow($a1-$b1,2) + pow($a2-$b2,2))}]
    }

    proc VScale {s v} {
        foreach {v0 v1 v2} $v {}
        return [list [expr {$s*$v0}] [expr {$s*$v1}] [expr {$s*$v2}]]
    }

    proc VAdd {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a0+$b0}] [expr {$a1+$b1}] [expr {$a2+$b2}]]
    }

    proc VLen {a} {
        foreach {a0 a1 a2} $a {}

        return [expr {sqrt(pow($a0*$a0,2) + pow($a1*$a1,2) + pow($a2*$a2,2))}]
    }

    proc VSub {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a0-$b0}] [expr {$a1-$b1}] [expr {$a2-$b2}]]
    }

    proc VNorm {a} {
        set len [VLen $a]
        return [VScale [expr {1.0/$len}] $a]
    }

    namespace export VAdd VSub VScale VCross VNorm
}
