

#-------------------------------------------------------------------------------
# .PROC dup_DefaceFindDICOM2
# 
# .ARGS
# path StartDir
# path AddDir
# string Pattern
# .END
#-------------------------------------------------------------------------------
proc dup_DefaceFindDICOM2 { StartDir AddDir Pattern } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames DICOMPatientIDs 

    set pwd [pwd]
    if [expr [string length $AddDir] > 0] {
        if [catch {cd $AddDir} err] {
            puts stderr $err
            return
        }
    }
    
    vtkDCMParser parser
    foreach match [glob -nocomplain -- $Pattern] {
        #puts stdout [file join $StartDir $match]
        if {[file isdirectory $match]} {
            continue
        }
        set FileName [file join $StartDir $AddDir $match]
        set found [parser OpenFile $match]
        if {[string compare $found "0"] == 0} {
            puts stderr "Can't open file [file join $StartDir $AddDir $match]"
        } else {
            set found [parser FindElement 0x7fe0 0x0010]
            if {[string compare $found "1"] == 0} {
                #
                # image data is available
                #
                
                set DICOMFiles($FindDICOMCounter,FileName) $FileName
                
                if [expr [parser FindElement 0x0010 0x0010] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set PatientName [parser ReadText $Length]
                    if {$PatientName == ""} {
                        set PatientName "noname"
                    }
                } else  {
                    set PatientName 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,PatientName) $PatientName
                AddListUnique DICOMPatientNames $PatientName
                
                if [expr [parser FindElement 0x0010 0x0020] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set PatientID [string trim [parser ReadText $Length]]
                    if {$PatientID == ""} {
                        set PatientID "noid"
                    }
                } else  {
                    set PatientID 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,PatientID) $PatientID
                AddListUnique DICOMPatientIDs $PatientID
                set add {}
                append add "<" $PatientID "><" $PatientName ">"
                AddListUnique DICOMPatientIDsNames $add
                set DICOMFiles($FindDICOMCounter,PatientIDName) $add
                #DYW change to studyID if [expr [parser FindElement 0x0020 0x000d] == "1"] 
                if [expr [parser FindElement 0x0020 0x0010] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set StudyInstanceUID [string trim [parser ReadText $Length]]
                    set zeros [string length $StudyInstanceUID]
                    if { $zeros > 4 } {
                       set StudyInstanceUID [string range $StudyInstanceUID [expr $zeros - 4] end]
                    } else {
                       set zeros [expr 4 - $zeros]
                       for {set lloop $zeros} {$lloop > 0} {inrc lloop -1} {
                          set StudyInstanceUID "0$StudyInstanceUID"
                       }
                    }             
                } else  {
                    set StudyInstanceUID 9999
                }
                set DICOMFiles($FindDICOMCounter,StudyInstanceUID) $StudyInstanceUID
                #DYW change to seriesID if [expr [parser FindElement 0x0020 0x000e] == "1"] 
                if [expr [parser FindElement 0x0020 0x0011] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set SeriesInstanceUID [string trim [parser ReadText $Length]]
                    set zeros [string length $SeriesInstanceUID ]
                    if { $zeros > 3 } {
                       set SeriesInstanceUID [string range $SeriesInstanceUID [expr $zeros - 3] end]
                    } else {
                       set zeros [expr 3 - $zeros]
                       for {set lloop 0} {$lloop < $zeros} {incr lloop } {
                          set SeriesInstanceUID "0$SeriesInstanceUID"
                       }
                    }             
                } else  {
                    set SeriesInstanceUID 999
                }
                set DICOMFiles($FindDICOMCounter,SeriesInstanceUID) $SeriesInstanceUID


                if [expr [parser FindElement 0x0020 0x0020] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set ProjectID [string trim [parser ReadText $Length]]
                    set zeros [string length $ProjectID ]
                    if { $zeros > 4 } {
                       set ProjectID [string range $ProjectID [expr $zeros - 4] end]
                    } else {
                       set zeros [expr 4 - $zeros]
                       for {set lloop 0} {$lloop < $zeros} {incr lloop } {
                          set ProjectID "0$ProjectID"
                       }
                    }             
                } else  {
                    set ProjectID  9999
                }
                set DICOMFiles($FindDICOMCounter,ProjectID) $ProjectID


                if [expr [parser FindElement 0x0020 0x0040] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set SubjectID [string trim [parser ReadText $Length]]
                    
                } else  {
                    set SubjectID 99999999
                }
                set DICOMFiles($FindDICOMCounter,SubjectID) $SubjectID


                if [expr [parser FindElement 0x0018 0x1314] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set FlipAngle [string trim [parser ReadText $Length]]
                } else  {
                    set FlipAngle 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,FlipAngle) $FlipAngle

                
        #set ImageNumber ""
        #if [expr [parser FindElement 0x0020 0x1041] == "1"] {
        #    set NextBlock [lindex [split [parser ReadElement]] 4]
        #    set ImageNumber [parser ReadFloatAsciiNumeric $NextBlock]
        #} 
        #if { $ImageNumber == "" } {
            if [expr [parser FindElement 0x0020 0x0013] == "1"] {
            #set Length [lindex [split [parser ReadElement]] 3]
            #set ImageNumber [parser ReadText $Length]
            #scan [parser ReadText $length] "%d" ImageNumber
            
            set NextBlock [lindex [split [parser ReadElement]] 4]
            set ImageNumber [parser ReadIntAsciiNumeric $NextBlock]
            } else  {
            set ImageNumber 1
            }
        #}
                
                    set zeros [string length $ImageNumber  ]
                    if { $zeros > 3 } {
                       set SeriesInstanceUID [string range $ImageNumber [expr $zeros - 3] end]
                    } else {
                       set zeros [expr 3 - $zeros]
                       for {set lloop 0} {$lloop < $zeros} {incr lloop } {
                          set ImageNumber "0$ImageNumber"
                       }
                    }             
                set DICOMFiles($FindDICOMCounter,ImageNumber) $ImageNumber
                
                incr FindDICOMCounter
                #puts [file join $StartDir $AddDir $match]
            } else {
                #set dim 256
            }
            parser CloseFile
        }
    }
    parser Delete
    
    foreach file [glob -nocomplain *] {
        if [file isdirectory $file] {
            dup_DefaceFindDICOM2 [file join $StartDir $AddDir] $file $Pattern
        }
    }
    cd $pwd
}

#-------------------------------------------------------------------------------
# .PROC dup_DefaceFindDICOM
# 
# .ARGS
# path StartDir
# string Pattern
# .END
#-------------------------------------------------------------------------------
proc dup_DefaceFindDICOM { StartDir Pattern } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames DICOMPatientIDs 
    global DICOMStudyInstanceUIDList
    global DICOMSeriesInstanceUIDList
    global DICOMFileNameArray
    global DICOMFileNameList DICOMFileNameSelected
    
    if [array exists DICOMFiles] {
        unset DICOMFiles
    }
    if [array exists DICOMFileNameArray] {
        unset DICOMFileNameArray
    }
    set pwd [pwd]
    set FindDICOMCounter 0
    set DICOMPatientNames {}
    set DICOMPatientIDsNames {}
    set DICOMPatientIDs {}
    set DICOMStudyList {}
    set DICOMSeriesList {}
    set DICOMFileNameList {}
    set DICOMFileNameSelected {}
    
    if [catch {cd $StartDir} err] {
        puts stderr $err
        cd $pwd
        return
    }
    dup_DefaceFindDICOM2 $StartDir "" $Pattern
    cd $pwd
}

#-------------------------------------------------------------------------------
# .PROC dup_AddListUnique
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_AddListUnique { list arg } {
    upvar $list list2
    if { [expr [lsearch -exact $list2 $arg] == -1] } {
        lappend list2 $arg
    }
}

#-------------------------------------------------------------------------------
# .PROC dup_DevErrorWindow
#
#  Report an Error to the user. Force them to click OK to continue.<br>
#  Resets the tk scaling to 1 and then returns it to the original value.
#
# .ARGS
#  str message The error message. Default: \"Unknown Error\"
# .END
#-------------------------------------------------------------------------------
proc dup_DevErrorWindow {{message "Unknown Error"}} {
    set oscaling [tk scaling]
    tk scaling 1
    if {$::Module(verbose)} {
        puts "$message"
    }
    tk_messageBox -title Slicer -icon error -message $message -type ok
    tk scaling $oscaling
}

#-------------------------------------------------------------------------------
# .PROC dup_DevWarningWindow
#
#  Report a Warning to the user. Force them to click OK to continue.<br>
#  Resets the tk scaling to 1 and then returns it to the original value.
#
# .ARGS
#  str message The error message. Default: \"Unknown Warning\"
# .END
#-------------------------------------------------------------------------------
proc dup_DevWarningWindow {{message "Unknown Warning"}} {
    set oscaling [tk scaling]
    tk scaling 1
    tk_messageBox -title "Slicer" -icon warning -message $message
    tk scaling $oscaling
}

#-------------------------------------------------------------------------------
# .PROC dup_DevOKCancel
#
#  Ask the user an OK/Cancel question. Force the user to decide before continuing.<br>
#  Returns "ok" or "cancel". <br>
#  Resets the tk scaling to 1 and then returns it to the original value.
# .ARGS
#  str message The message to give.
# .END
#-------------------------------------------------------------------------------
proc dup_DevOKCancel {message} {
    set oscaling [tk scaling]
    
    if {$::Module(verbose)} {
        puts "dup_DevOKCancel: original scaling is $oscaling, changing it to 1 and then back"
    }

    tk scaling 1
    set retval [tk_messageBox -title Slicer -icon question -type okcancel -message $message]
    tk scaling $oscaling

    return $retval
}


# default to disabled tooltips when this file is first sourced
set dup_Tooltips(enabled) 0

#-------------------------------------------------------------------------------
# .PROC dup_TooltipAdd
# Call this procedure to add a tooltip (floating help text) to a widget. 
# The tooltip will pop up over the widget when the user leaves the mouse
# over the widget for a little while.
# .ARGS
# str widget name of the widget
# str tip text that you would like to appear as a tooltip.
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipAdd {widget tip} {

    # surround the tip string with brackets
    set tip "\{$tip\}"

    # bindings
    bind $widget <Enter> "dup_TooltipEnterWidget %W $tip %X %Y"
    bind $widget <Leave> dup_TooltipExitWidget


    # The following are fixes to make buttons work right with tooltips...

    # put the class (i.e. Button) first in the bindtags so it executes earlier
    # (this makes button highlighting work)
    # just swap the first two list elements
    set btags [bindtags $widget]
    if {[llength $btags] > 1} {
    set class [lindex $btags 1]
    set btags [lreplace $btags 1 1 [lindex $btags 0]]
    set btags [lreplace $btags 0 0 $class]
    }
    bindtags $widget $btags

    # if the button is pressed, this should be like a Leave event
    # (otherwise the tooltip will come up incorrectly)
    if {$class == "Button" || $class == "Radiobutton"} {
    set cmd [$widget cget -command]
    set cmd "dup_TooltipExitWidget; $cmd"
    }
}


#-------------------------------------------------------------------------------
# .PROC dup_TooltipEnterWidget
# Internal procedure for dup_Tooltips.tcl.  Called when the mouse enters the widget.
# This proc works with dup_TooltipExitWidget to pop up the tooltip after a delay.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipEnterWidget {widget tip X Y} {
    global dup_Tooltips


    # do nothing if tooltips disabled
    if {$dup_Tooltips(enabled) == 0} {
    return
    }

    # We are over the widget
    set dup_Tooltips(stillOverWidget) 1

    # reset dup_Tooltips(stillOverWidget) after a delay (to end the "vwait")
    set id [after 500 \
        {if {$dup_Tooltips(stillOverWidget) == 1} {set dup_Tooltips(stillOverWidget) 1}}]

    # wait until dup_Tooltips(stillOverWidget) is set (by us or by exiting the widget).
    # "vwait" allows event loop to be entered (but using an "after" does not)
    vwait dup_Tooltips(stillOverWidget)

    # if dup_Tooltips(stillOverWidget) is 1, the mouse is still over widget.
    # So pop up the tooltip!
    if {$dup_Tooltips(stillOverWidget) == 1} {
    dup_TooltipPopUp $widget $tip $X $Y
    } else {
    # the mouse exited the widget already, so cancel the waiting.
    after cancel $id
    }

}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipExitWidget
# Internal procedure for dup_Tooltips.tcl.  Called when the mouse exits the widget. 
# This proc works with dup_TooltipEnterWidget to pop up the tooltip after a delay.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipExitWidget {} {
    global dup_Tooltips

    # mouse is not over the widget anymore, so stop the vwait.
    set dup_Tooltips(stillOverWidget) 0

    # remove the tooltip if there is one.
    dup_TooltipPopDown
}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipPopUp
# Internal procedure for dup_Tooltips.tcl.  Causes the tooltip window to appear. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipPopUp {widget tip X Y} {
    global dup_Tooltips Gui

    # set tooltip window name
    if {[info exists dup_Tooltips(window)] == 0} {
    set dup_Tooltips(window) .wdup_Tooltips
    }

    # get rid of any other existing tooltip
    dup_TooltipPopDown

    # make a new tooltip window
    toplevel $dup_Tooltips(window)

    # add an offset to make tooltips fall below cursor
    set Y [expr $Y+15]

    # don't let the window manager "reparent" the tip window
    wm overrideredirect $dup_Tooltips(window) 1

    # display the tip text...
    wm geometry $dup_Tooltips(window) +${X}+${Y}
    eval {label $dup_Tooltips(window).l -text $tip } $Gui(WTTA)
    pack $dup_Tooltips(window).l -in $dup_Tooltips(window)
}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipPopDown
# Internal procedure for dup_Tooltips.tcl.  Removes the tooltip window. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipPopDown {} {
    global dup_Tooltips

    catch {destroy $dup_Tooltips(window)}
}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipDisable
# Turn off display of tooltips
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipDisable {} {
    global dup_Tooltips

    # get rid of any other existing tooltip
    dup_TooltipPopDown

    # disable tooltips
    set dup_Tooltips(enabled) 0
}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipEnable
# Turn on display of tooltips
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipEnable {} {
    global dup_Tooltips

    # disable tooltips
    set dup_Tooltips(enabled) 1
}

#-------------------------------------------------------------------------------
# .PROC dup_TooltipToggle
# Toggle tooltip display on/off
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc dup_TooltipToggle {} {
    global dup_Tooltips

    if {$dup_Tooltips(enabled) == 1} {
    set dup_Tooltips(enabled) 0
    } else {
    set dup_Tooltips(enabled) 1
    } 
}

