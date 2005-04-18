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
# FILE:        imrml.tcl
# PROCEDURES:  
#==========================================================================auto=

package require Iwidgets
package require xml
package require dom

if { [info command class] == "" } {
    namespace import ::itcl::*
}

#########################################################
#
if {0} { ;# comment

2002-11-11 - Steve Pieper
imrml - hierarchy viewer of xml (mrml) trees using:
    itcl/itk for code structure
    Incr Widgets for GUI components
    tclXML/tclDOM for parsing and data structure

    xml parsing based on examples by Mike Halle
}
#
#########################################################

#
# Default resources
#
option add *Imrml.background #cccccc widgetDefault

#
# The class definition
#    - first check it it alread exists
# To redefine the methods and option handlers:
#    - just source this file
# To redefine the class (variables and method signature
#    - ::itcl::delete class ::iwidgets::Imrml
#    - then source this file
#
if { [find classes ::iwidgets::Imrml] == "" } {

    class iwidgets::Imrml {

        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor {}

        itk_option define -xml xml Xml {}
        itk_option define -filename filename Filename {}

        variable _doc

        method read {} {}
        method query {node} {}
        method select {node status} {}
        method expand_recursive {node} {}
        method expand_all {} {}
        method collapse_all {} {}
    }
}


#
# Provide the usual lowercase access command.
#
proc iwidgets::imrml {path_ args} {
  uplevel iwidgets::Imrml $path_ $args
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
body iwidgets::Imrml::constructor {args} {

    #component hull configure -borderwidth 0
    
    itk_component add hier {
        iwidgets::hierarchy $itk_interior.hier \
            -querycommand "$this query %n" -visibleitems 30x15 \
            -labeltext "" -selectcommand "$this select %n %s" \
            -labelpos sw \
            -hscrollmode dynamic -vscrollmode dynamic
    } {
        # placeholder for configuration options...
    }

    itk_component add sheet {
        iwidgets::scrolledframe $itk_interior.sheet \
            -hscrollmode dynamic -vscrollmode dynamic
    } {
        # placeholder for configuration options...
    }

    pack $itk_interior.hier -side left -expand true -fill both
    pack $itk_interior.sheet -side left -expand true -fill both

    #
    # Initialize the widget based on the command line options.
    #
    eval itk_initialize $args
}


body iwidgets::Imrml::destructor {} {
    destroy $itk_interior.hier 
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#-------------------------------------------------------------------------------
# OPTION: -xml
#
# DESCRIPTION: set the xml string of the whole mrml file
#-------------------------------------------------------------------------------
configbody iwidgets::Imrml::xml {

    if {$itk_option(-xml) == ""} {
        return
    }

    set _doc [::dom::DOMImplementation parse $itk_option(-xml)]

    $itk_interior.hier clear
    $itk_interior.hier configure -querycommand "$this query %n"

    update
    $this expand_all
}

#-------------------------------------------------------------------------------
# OPTION: -filename
#
# DESCRIPTION: set the xml string of the whole mrml file
#-------------------------------------------------------------------------------
configbody iwidgets::Imrml::filename {

    if {$itk_option(-filename) == ""} {
        $this configure -xml ""
        return
    }

    $itk_interior.hier configure -labeltext $itk_option(-filename)

    set fp [open $itk_option(-filename) "r"]
    set xml [::read $fp]
    close $fp

    $this configure -xml $xml
}


# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------


body iwidgets::Imrml::read {} {
    # read the -filename and put the value in -xml
}

body iwidgets::Imrml::query {node} {
    # callback from the hierarchy

    if { $node == "" } { ;# look for toplevel node
        if { $itk_option(-xml) == "" } { ;# does xml string exist
            return ""
        } else {
            array set n [set $_doc]
            return "[list [list $_doc $n(node:nodeName)]]"
        }
    } else {
        set rlist ""
        array set nodearr [set $node]
        if {$nodearr(node:childNodes) != ""} {
            foreach ele [set $nodearr(node:childNodes)] {
                array set n [set $ele]

                # skip the #text nodes since they just mirror the real nodes
                if { $n(node:nodeName) == "#text" } {continue}

                if { [info exists n(element:attributeList)] } {
                    set attrs $n(element:attributeList)
                    if [catch "set [set attrs](name)" name] {
                        set name "unnamed"
                    }
                    lappend rlist [list $ele "$n(node:nodeName) ($name)"]
                } else {
                    # if they have no attributes, they aren't nodes
                    # that we're interested in, so skip them
                    #lappend rlist [list $ele "$n(node:nodeType) ($n(node:nodeName))"]
                }
            }
        }
        return $rlist
    }
}

body iwidgets::Imrml::select {node state} {
    # callback from the hierarchy

    # accept this the current (unique) selection
    $itk_interior.hier selection clear
    $itk_interior.hier selection add $node

    # remove old edit widgets
    set cs [$itk_component(sheet) childsite]
    foreach w [winfo children $cs] {
        destroy $w
    }

    # create a set of entries for modifying these values
    array set n [set $node]
    if { [info exists n(element:attributeList)] } {
        set attrs $n(element:attributeList)

        set widgets ""
        foreach a [lsort -dictionary [array names $attrs]] {
            set aa [string tolower $a]
            iwidgets::entryfield $cs.$aa \
                -labeltext "$a " \
                -textvariable [set attrs]($a)
            pack $cs.$aa -side top
            lappend widgets $cs.$aa
        }
        eval iwidgets::Labeledwidget::alignlabels $widgets
    }
}

# ----------------------------------------------------------------------
# method: expand_recursive
#
# Recursively expand all the nodes in the hierarchy.  
# ----------------------------------------------------------------------
body iwidgets::Imrml::expand_recursive {node} {
    set files [$this query $node]

    foreach tagset $files {
        set uid [lindex $tagset 0]
        $itk_interior.hier expand $uid

        if {[$this query $uid] != {}} {
            expand_recursive $uid
        }
    }
}

# ----------------------------------------------------------------------
# method: expand_all
#
# Expand all the file nodes in the hierarchy.  
# ----------------------------------------------------------------------
body iwidgets::Imrml::expand_all {} {
    expand_recursive ""
}

# ----------------------------------------------------------------------
# method: collapse_all
#
# Collapse all the nodes in the hierarchy.
# ----------------------------------------------------------------------
body iwidgets::Imrml::collapse_all {} {
    $itk_interior.hier configure -querycommand "$this query %n"
}

#
# test proc that puts up an instance if imrml
#
proc iwidgets::Imrml::test {} {
    
    tk_messageBox -message "Sorry, the TclDOM interface has changed in non-compatible ways."
    return

    wm withdraw .
    # console show
    set t .imrml_test
    catch "destroy $t"
    toplevel $t
    wm geometry $t 700x500+20+20
    wm title $t "MRML Editor"

    iwidgets::imrml $t.imrml
    pack $t.imrml -fill both -expand true

    button $t.exit -text close -command "destroy $t.imrml"
    pack $t.exit -side bottom -fill x -expand false

    set ret [catch {
        $t.imrml configure -filename $::Mrml(dir)/$::File(filePrefix).xml
    } res]
    if { $ret } { puts $res }

}

