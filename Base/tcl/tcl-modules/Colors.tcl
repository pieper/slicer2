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
# FILE:        Colors.tcl
# PROCEDURES:  
#   ColorsInit
#   ColorsBuildGUI
#   ColorsSetFileName
#   ColorsLoadApply
#   ColorsApply
#   ColorsUpdateMRML
#   ColorsDisplayColors
#   ColorsSelectColor optional
#   ColorsSetColor
#   ColorsAddColor
#   ColorsDeleteColor
#   ColorsColorSample
#   ColorsDisplayLabels
#   ColorsSelectLabel
#   ColorsAddLabel
#   ColorsDeleteLabel
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ColorsInit
#
# Initialize global variables.
# .END
#-------------------------------------------------------------------------------
proc ColorsInit {} {
    global Color Gui Module

    # Define Tabs
    set m Colors
    set Module($m,row1List) "Help Colors Load"
    set Module($m,row1Name) "Help {Edit Colors} {Load Colors}"
    set Module($m,row1,tab) Colors

    # Module Summary Info
    set Module($m,overview) "Add new colors, view color lookup table."
    set Module($m,author) "Core"
    set Module($m,category) "Settings"

    # Define Procedures
    set Module($m,procGUI)  ColorsBuildGUI
    set Module($m,procMRML) ColorsUpdateMRML

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.27 $} {$Date: 2004/04/13 21:00:03 $}]
}

#-------------------------------------------------------------------------------
# .PROC ColorsBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc ColorsBuildGUI {} {
    global Gui Module Color Colors

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Colors
    #   Top
    #     Colors
    #     Labels
    #   Bot
    #     Attr
    #     Apply
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
Click on the name of a color in the <B>Color Name</B> listbox to view the
<B>Label</B> values associated with this color.  Use the <B>Add</B> and
<B>Delete</B> buttons to create/remove new colors or labels for colors.
<P>
Your changes will not take effect until you click the <B>Apply</B> button.
<P>
The colors are saved in the MRML file when you select the <B>Save</B> option
from the <B>File</B> menu if they differ from the default colors.
<P>
The Load Colors tab will allow you to open up mrml files with color nodes 
and use the new ones as your default colors.
"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Colors $help
    MainHelpBuildGUI Colors

    #-------------------------------------------
    # Colors frame
    #-------------------------------------------
    set fColors $Module(Colors,fColors)
    set f $fColors

    frame $f.fTop -bg $Gui(activeWorkspace)
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fTop $f.fBot -side top -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Colors->Top
    #-------------------------------------------
    set f $fColors.fTop

    frame $f.fColors -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fLabels -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fColors $f.fLabels -side left -padx 2 -pady 1 -fill x

    #-------------------------------------------
    # Colors->Top->Colors frame
    #-------------------------------------------
    set f $fColors.fTop.fColors

    eval {label $f.lTitle -text "Color Name"} $Gui(WTA)

    set Color(fColorList) [ScrolledListbox $f.list 1 1 -height 5 -width 15]
    bind $Color(fColorList) <ButtonRelease-1> {ColorsSelectColor}

    eval {entry $f.eName -textvariable Color(name) -width 18} $Gui(WEA)
    bind $f.eName <Return> "ColorsAddColor"
    
    frame $f.fBtns -bg $Gui(activeWorkspace)
    eval {button $f.fBtns.bAdd -text "Add" -width 4 \
        -command "ColorsAddColor"} $Gui(WBA)
    eval {button $f.fBtns.bDelete -text "Delete" -width 7 \
        -command "ColorsDeleteColor"} $Gui(WBA)
    pack $f.fBtns.bAdd $f.fBtns.bDelete -side left -padx $Gui(pad)

    pack $f.lTitle -side top -pady 2
    pack $f.list $f.eName $f.fBtns -side top -pady $Gui(pad)

    #-------------------------------------------
    # Colors->Top->Labels frame
    #-------------------------------------------
    set f $fColors.fTop.fLabels

    eval {label $f.lTitle -text "Label"} $Gui(WTA)

    set Color(fLabelList) [ScrolledListbox $f.list 1 1 -height 5 -width 6]
        bind $Color(fLabelList) <ButtonRelease-1> "ColorsSelectLabel"

    eval {entry $f.eName -textvariable Color(label) -width 9} $Gui(WEA)
    bind $f.eName <Return> "ColorsAddLabel"

    frame $f.fBtns -bg $Gui(activeWorkspace)
    eval {button $f.fBtns.bAdd -text "Add" -width 4 \
        -command "ColorsAddLabel"} $Gui(WBA)
    eval {button $f.fBtns.bDelete -text "Del" -width 4 \
        -command "ColorsDeleteLabel"} $Gui(WBA)
    pack $f.fBtns.bAdd $f.fBtns.bDelete -side left -padx $Gui(pad)

    pack $f.lTitle -side top -pady 2
    pack $f.list $f.eName $f.fBtns -side top -pady $Gui(pad) 

    #-------------------------------------------
    # Colors->Bot frame
    #-------------------------------------------
    set f $fColors.fBot

    frame $f.fAttr  -bg $Gui(activeWorkspace)
    frame $f.fApply -bg $Gui(activeWorkspace)
    pack $f.fAttr $f.fApply -side left -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Colors->Bot->Attr frame
    #-------------------------------------------
    set f $fColors.fBot.fAttr

    foreach slider "Red Green Blue Ambient Diffuse Specular Power" {

        eval {label $f.l${slider} -text "${slider}"} $Gui(WLA)

        eval {entry $f.e${slider} -textvariable Color([Uncap $slider]) \
            -width 3} $Gui(WEA)
            bind $f.e${slider} <Return>   "ColorsSetColor"
            bind $f.e${slider} <FocusOut> "ColorsSetColor"

        eval {scale $f.s${slider} -from 0.0 -to 1.0 -length 40 \
            -variable Color([Uncap $slider]) -command "ColorsSetColor" \
            -resolution 0.1} $Gui(WSA) {-sliderlength 15}
        set Color(s$slider) $f.s$slider

        grid $f.l${slider} $f.e${slider} $f.s${slider} \
            -pady 1 -padx 1 -sticky e
    }
    $f.sPower config -from 0 -to 100 -resolution 1 


    #-------------------------------------------
    # Colors->Bot->Apply frame
    #-------------------------------------------
    set f $fColors.fBot.fApply

    eval {button $f.bApply -text "Update" -width 7 \
        -command "ColorsApply; RenderAll"} $Gui(WBA)
    pack $f.bApply -side top -pady $Gui(pad) 



    #-------------------------------------------
    # Load frame
    #-------------------------------------------
    set fLoad $Module(Colors,fLoad)
    set f $fLoad

    frame $f.fTop -bg $Gui(activeWorkspace)
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fTop $f.fBot -side top -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Load->Top
    #-------------------------------------------
    set f $fLoad.fTop
    DevAddFileBrowse $f Color fileName "MRML color file:" "ColorsSetFileName" "xml" "" "Open" "Open a Color MRML file"
    eval {button $f.bLoad -text "Load" -width 7 \
              -command "ColorsLoadApply"} $Gui(WBA)
    pack $f.bLoad -side top -pady $Gui(pad) 

    #-------------------------------------------
    # Load->Bot
    #-------------------------------------------

    set f $fLoad.fBot
    eval {label $f.lWarning -text "Warning: Still experimental,\nNOT fully functional"} $Gui(WLA)
    pack $f.lWarning -side top -pady $Gui(pad)
                                                                                            
}

#-------------------------------------------------------------------------------
# .PROC ColorsSetFileName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ColorsSetFileName {}  {
    global Color
    puts "Color filename = $Color(fileName)"
}

#-------------------------------------------------------------------------------
# .PROC ColorsLoadApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ColorsLoadApply {} {
    global Color
    puts "ColorsLoadApply: experimental load of a new xml file with colour nodes"
    MainMrmlDeleteColors
    MainMrmlAddColorsFromFile $Color(fileName)
    # TODO: update the gui's Edit Colors panel now, and the Select a Color canvas
    MainColorsUpdateMRML
    RenderAll
}
#-------------------------------------------------------------------------------
# .PROC ColorsApply
#
# Update all uses of colors in the Slicer to show the current colors
# .END
#-------------------------------------------------------------------------------
proc ColorsApply {} {
    global Color

    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC ColorsUpdateMRML
#
# This routine is called when the MRML tree changes
# .END
#-------------------------------------------------------------------------------
proc ColorsUpdateMRML {} {
    global Color

    if {$Color(idList) == ""} {
        return
    }
    
    ColorsDisplayColors
    ColorsSelectColor $Color(activeID)
    ColorsSelectLabel 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayColors
# 
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayColors {} {
    global Color Mrml

    # Clear old
    $Color(fColorList) delete 0 end

    # Append new
    set tree Mrml(colorTree) 
    set node [$tree InitColorTraversal]
    while {$node != ""} {
        $Color(fColorList) insert end [$node GetName]
        set node [$tree GetNextColor]
    }
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectColor
# 
# .ARGS
# i optional color index in Color(idList)
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectColor {{i ""}} {
    global Color Mrml

    if {$i == ""} {
        set i [$Color(fColorList) curselection]
    }
    if {$i == ""} {return}
    $Color(fColorList) selection set $i $i
    set c [lindex $Color(idList) $i]

    MainColorsSetActive $c

    ColorsColorSample
    ColorsDisplayLabels
}

#-------------------------------------------------------------------------------
# .PROC ColorsSetColor
# Used by the color sliders
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ColorsSetColor {{value ""}} {
    global Color

    # Set the new color from the GUI into the node
    set c $Color(activeID)
    if {$c == ""} {return}

    Color($c,node) SetDiffuseColor $Color(red) $Color(green) $Color(blue)
    foreach param "Ambient Diffuse Specular Power" {
        Color($c,node) Set$param $Color([Uncap $param])
    }

    # Draw Sample
    ColorsColorSample
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddColor
#
# .END
#-------------------------------------------------------------------------------
proc ColorsAddColor {} {
    global Color Gui
    
    set c [MainColorsAddColor $Color(name) $Color(diffuseColor) \
        $Color(ambient) $Color(diffuse) $Color(specular) $Color(power)]
    
    if {$c != ""} {
        # make sure new color is selected on GUI
        MainColorsSetActive $c

        MainUpdateMRML
    }
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteColor
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteColor {} {
    global Color

    MainMrmlDeleteNode Color $Color(activeID)
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC ColorsColorSample
# .END
#-------------------------------------------------------------------------------
proc ColorsColorSample {} {
    global Color
    
    set color "$Color(red) $Color(green) $Color(blue)"
    foreach slider "Red Green Blue" {
    ColorSlider $Color(s$slider) $color
    }
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayLabels
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayLabels {} {
    global Color

    # Clear old
    $Color(fLabelList) delete 0 end

    # Append new
    set c $Color(activeID)
    if {$c == ""} {return}

    set Color(label) ""
    foreach label [Color($c,node) GetLabels] {
        $Color(fLabelList) insert end $label
    }
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectLabel {{i ""}} {
    global Color

    if {$i == ""} {
        set i [$Color(fLabelList) curselection]
    }
    if {$i == ""} {return}
    $Color(fLabelList) selection set $i $i
    
    set c $Color(activeID)
    if {$c == ""} {
        set Color(label) ""
    } else {
        set labels [Color($c,node) GetLabels]
        set Color(label) [lindex $labels $i]
    }
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddLabel
#
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc ColorsAddLabel {} {
    global Color Color Gui

    # Convert to integer
    set index [MainColorsAddLabel $Color(activeID) $Color(label)]

    ColorsDisplayLabels
    ColorsSelectLabel $index
    return 1
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteLabel {} {
    global Color Color

    MainColorsDeleteLabel $Color(activeID) $Color(label)

    ColorsDisplayLabels
    ColorsSelectLabel 0
}
