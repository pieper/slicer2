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
# FILE:        birnPipelineFunctions.tcl
# PROCEDURES:  
#   BIRNprocessXMLTemplateFile infile resultsdir atlasdir subjname
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC BIRNprocessXMLTemplateFile
#
# .ARGS
# path infile
# path resultsdir
# path atlasdir
# string subjname
# .END
#-------------------------------------------------------------------------------
proc BIRNprocessXMLTemplateFile {infile resultsdir atlasdir subjname} {

    ######## read template 

    if { [ catch { set fid [ open $infile r ] } errmsg ] } {
    error "can't read xml file: $infile: $errmsg"
    }

    if { [ catch { set template [read $fid]; close $fid } ] } {
    error "error reading xml file: $infile: $errmsg"
    }

    ######## check for variables in template

    foreach svar {@SUBJECT@ @RESULT-END@ @RESULT-START@ @ATLASROOT@ @RESULTDIR@} {
    if { ! [ regexp $svar $template ] } { 
        error "no $svar in template: $infile"
    }
    }

    ######## substitute variables in template

    regsub -all "@SUBJECT@" $template $subjname tmptemplate 

    regsub -all "<@RESULT-START@.*@RESULT-END@>" [ \
        regsub -all "@ATLASROOT@" $tmptemplate $atlasdir \
    ] {} atlastemplate 


    regsub -all "<@RESULT-START@.*@RESULT-END@>" [ \
        regsub -all "@ATLASROOT@" $tmptemplate $resultsdir \
    ] {} worktemplate

    regsub -all "@RESULT-END@" [ \
    regsub -all  "@RESULT-START@" [ \
        regsub -all "@RESULTDIR@|@ATLASROOT@" $tmptemplate $resultsdir \
    ] {Volume} \
    ] {></Volume} resultstemplate


    ######## write output 

    set outfile1 [ file join $resultsdir $subjname "${subjname}.xml" ]
    set outfile2 [ file join $resultsdir $subjname "atlases.xml" ]
    set outfile3 [ file join $resultsdir $subjname "EMResult.xml" ]

    foreach {filename template} [ list $outfile1 $worktemplate $outfile2 \
        $atlastemplate $outfile3 $resultstemplate ] {

    if { [ catch { set fid [ open $filename  w ] } errmsg ] } { 
        error "can't open output xml file: $filename: $errmsg"
    }

    if { [ catch { puts -nonewline $fid $template; close $fid } errmsg ] } {
        error "failed writing output xml file: $filename: $errmsg"
    }
    }


    return [ list $outfile1 $outfile2 $outfile3 ] 
}

