#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        IbrowserDisplayGUI.tcl
# PROCEDURES:  
#   IbrowserBuildDisplayFrame
#   IbrowserUpdateMainViewer
#   IbrowserSetIntervalParam
#   IbrowserSetIntervalParamActiveVol
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserBuildDisplayFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildDisplayFrame { } {
    global Gui Module Volume

    set fDisplay $::Module(Ibrowser,fDisplay)

    set w $fDisplay

    #--- Frames: use packer to put them in place.
    frame $w.fSelect -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $w.fWinLvl -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $w.fThresh -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $w.fHistogram -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $w.fInterpolate -bg $Gui(activeWorkspace) 
    pack $w.fSelect $w.fWinLvl $w.fThresh $w.fHistogram $w.fInterpolate \
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x


    #-------------------------------------------
    # Select volume frame
    #-------------------------------------------
    set f $w.fSelect
    DevAddLabel $f.lSelect "Select:"
    eval { scale $f.index -orient horizontal \
               -from 0 -to $::Ibrowser(MaxDrops) -resolution 1 \
               -bigincrement 1 -length 150 -state disabled -showvalue 1 \
               -variable ::Ibrowser(ViewDrop) } \
               $Gui(WSA) { -showvalue 1 }

    #--- Save a ref to the scale so we can find it later
    #--- Update the main viewer based on slider motion
    #--- and update the IbrowserController slider too.
    set ::Ibrowser(indexSlider) $f.index
    bind $f.index <B1-Motion> {
        IbrowserUpdateIndexFromDisplayGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    grid $f.lSelect $f.index -pady $Gui(pad) -padx $Gui(pad) -sticky e


    #-------------------------------------------
    # WinLvl frame
    #-------------------------------------------
    set f $w.fWinLvl

    #-------------------------------------------
    # Auto W/L
    #-------------------------------------------
    eval {label $f.lAuto -text "  Win/Lev:"} $Gui(WLA)
    frame $f.fAuto -bg $Gui(activeWorkspace) 
    grid $f.lAuto $f.fAuto -pady $Gui(pad)  -padx $Gui(pad) -sticky e
    grid $f.fAuto -columnspan 2 -sticky w

    foreach value "1 0" text "Auto Manual" width "5 7" {
        eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(autoWindowLevel) \
                  -command "IbrowserSetIntervalParam AutoWindowLevel" \
            } $Gui(WCA)
        pack $f.fAuto.rAuto$value -side left -fill x
    }

    #-------------------------------------------
    # W/L Sliders
    #-------------------------------------------
    foreach slider "Window Level" {
        eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
        eval {entry $f.e${slider} -width 5 \
            -textvariable Volume([Uncap ${slider}])} $Gui(WEA)
        bind $f.e${slider} <Return>   \
            "IbrowserSetIntervalParam ${slider}"
        bind $f.e${slider} <FocusOut> \
            "IbrowserSetIntervalParam ${slider}"
        eval {scale $f.s${slider} -from 1 -to 1024 \
            -variable Volume([Uncap ${slider}]) -length 90 -resolution 1 \
                  -command "IbrowserSetIntervalParamActiveVol ${slider}" \
             } $Gui(WSA)
        bind $f.s${slider} <ButtonRelease-1> "IbrowserSetIntervalParam ${slider}"

        grid $f.l${slider} $f.e${slider} $f.s${slider} \
           -pady $Gui(pad) -padx $Gui(pad)
        grid $f.l${slider} -sticky e
        grid $f.s${slider} -sticky w

        #grid $f.l${slider} $f.e${slider} $f.s${slider} \
        #   -pady $Gui(pad) -padx $Gui(pad) -columnspan 3
        #grid $f.l$slider -sticky e 
        #grid $f.s$slider -sticky w
        set Volume(s$slider) $f.s$slider
    }
    # Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
    lappend Volume(sWindowList) $f.sWindow
    lappend Volume(sLevelList) $f.sLevel

    #-------------------------------------------
    # Popup->Thresh frame
    #-------------------------------------------
    set f $w.fThresh

    #-------------------------------------------
    # Auto Threshold
    #-------------------------------------------
    eval {label $f.lAuto -text "Threshold:"} $Gui(WLA)
    frame $f.fAuto -bg $Gui(activeWorkspace)
    grid $f.lAuto $f.fAuto -pady $Gui(pad) -padx $Gui(pad) -sticky e
    grid $f.fAuto -columnspan 2 -sticky w

    foreach value "1 0" text "Auto Manual" width "5 7" {
        eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(autoThreshold) \
                  -command "IbrowserSetIntervalParam AutoThreshold;"} $Gui(WCA)
    }
    eval {checkbutton $f.cApply \
        -text "Apply" -variable Volume(applyThreshold) \
              -command "IbrowserSetIntervalParam ApplyThreshold" -width 6 \
        -indicatoron 0} $Gui(WCA)
    
    grid $f.fAuto.rAuto1 $f.fAuto.rAuto0 $f.cApply
    grid $f.cApply -padx $Gui(pad)

    #-------------------------------------------
    # Threshold Sliders
    #-------------------------------------------
    foreach slider "Lower Upper" {
        eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
        eval {entry $f.e${slider} -width 5 \
            -textvariable Volume([Uncap ${slider}]Threshold)} $Gui(WEA)
            bind $f.e${slider} <Return>   \
            "IbrowserSetIntervalParam ${slider}Threshold"
            bind $f.e${slider} <FocusOut> \
            "IbrowserSetIntervalParam ${slider}Threshold"

        eval {scale $f.s${slider} -from 1 -to 1024 \
            -variable Volume([Uncap ${slider}]Threshold) -length 90 -resolution 1 \
                  -command "IbrowserSetIntervalParamActiveVol ${slider}Threshold" \
             } $Gui(WSA)
        bind $f.s${slider} <ButtonRelease-1> "IbrowserSetIntervalParam ${slider}Threshold"
        
        grid $f.l${slider} $f.e${slider} $f.s${slider} \
             -padx $Gui(pad) -pady $Gui(pad)
        grid $f.l$slider -sticky e
        grid $f.s$slider -sticky w
        set Volume(s$slider) $f.s$slider
    }
    # Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
    lappend Volume(sLevelList) $f.sLower
    lappend Volume(sLevelList) $f.sUpper

    #-------------------------------------------
    # Histogram frame
    #-------------------------------------------
    set f $w.fHistogram

    frame $f.fHistBorder -bg $Gui(activeWorkspace) -relief sunken -bd 2
    frame $f.fLut -bg $Gui(activeWorkspace)
    pack $f.fLut $f.fHistBorder -side left -padx $Gui(pad) -pady $Gui(pad)
    
    #-------------------------------------------
    # Lut frame
    #--- having some trouble with this one.
    #-------------------------------------------
    set f $w.fHistogram.fLut

    DevAddLabel $f.lLUT "Palette:"
    eval {menubutton $f.mbLUT \
        -text "$::Lut([lindex $::Lut(idList) 0],name)" \
            -relief raised -bd 2 -width 9 \
        -menu $f.mbLUT.menu} $Gui(WMBA)
        eval {menu $f.mbLUT.menu} $Gui(WMA)
        # Add menu items
        foreach l $::Lut(idList) {
            $f.mbLUT.menu add command -label $::Lut($l,name) \
                -command  "IbrowserSetIntervalParam LutID $l"
        }
        set Volume(mbLUT) $f.mbLUT

    pack $f.lLUT $f.mbLUT -pady $Gui(pad) -side top

    #-------------------------------------------
    # HistBorder frame
    #-------------------------------------------
    set f $w.fHistogram.fHistBorder

    if {$Volume(histogram) == "On"} {
        MakeVTKImageWindow hist
        histMapper SetInput [Volume(0,vol) GetHistogramPlot]

        set wid [expr $Volume(histWidth) - 8]
        
        vtkTkRenderWidget $f.fHist -rw histWin \
            -width $wid -height $Volume(histHeight)  
        bind $f.fHist <Expose> {ExposeTkImageViewer %W %x %y %w %h}
        pack $f.fHist
    }

    #-------------------------------------------
    # Interpolate frame
    #-------------------------------------------
    set f $w.fInterpolate

    DevAddLabel $f.lInterpolate "Interpolation:"
    pack $f.lInterpolate -pady $Gui(pad) -padx $Gui(pad) -side left -fill x

    foreach value "1 0" text "On Off" width "4 4" {
        eval {radiobutton $f.rInterp$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(interpolate) \
            -command "IbrowserSetIntervalParam Interpolate"} $Gui(WCA)
        pack $f.rInterp$value -side left -fill x
    }

    

}




#-------------------------------------------------------------------------------
# .PROC IbrowserUpdateMainViewer
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserUpdateMainViewer { n } {
    
    #--- If the requested drop number to view is appropriate for the 
    #--- selectedInterval, update the main viewer to display this drop.
    #--- check here to see if the none interval is in the FG or BG.
    if { [ info exists ::Ibrowser(FGInterval) ] &&
          [ info exists ::Ibrowser(BGInterval)]} {

        set inumFG $::Ibrowser(FGInterval)
        set inumBG $::Ibrowser(BGInterval)

        #--- update the slices window.
        if { $inumFG > 0 } {
            if { $::Ibrowser($inumFG,numDrops) > $n } {
                #--- What's active in the foreground?
                MainSlicesSetVolumeAll Fore $::Ibrowser($inumFG,$n,MRMLid)
            } else {
                if { $::Ibrowser($inumFG,holdStatus) == $::IbrowserController(Info,Ival,nohold) } {
                    MainSlicesSetVolumeAll Fore "None"
                } else {
                    MainSlicesSetVolumeAll Fore $::Ibrowser($inumFG,lastMRMLid)
                }
            }
        }

        #--- update BG if there's a volume in the FG
        if { $inumBG > 0 } {

            if { $::Ibrowser($inumBG,numDrops) > $n } {
                #--- What's active in the background?
                MainSlicesSetVolumeAll Back $::Ibrowser($inumBG,$n,MRMLid)
            } else {
                if { $::Ibrowser($inumBG,holdStatus) == $::IbrowserController(Info,Ival,nohold) } {
                    MainSlicesSetVolumeAll Back "None"
                } else {
                    MainSlicesSetVolumeAll Back $::Ibrowser($inumBG,lastMRMLid)
                }
            }
        }
        RenderAll
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserSetIntervalParam
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetIntervalParam { setThis { thing -10} } {
    global Volume
    
    #---sets display parameters for the volumes in the active interval
    if {$thing == -10 } {
        set ignoreVal 1
    } else {
        set ignoreVal 0
    }
    set activeVol $::Volume(activeID)
    set activeIval $::Ibrowser(activeInterval)
    set start $::Ibrowser($activeIval,firstMRMLid)
    set stop $::Ibrowser($activeIval,lastMRMLid)

    set pcount 0
    set numvols [ expr $stop - $start ]
    IbrowserRaiseProgressBar

    #for each mrml volume, set "setThis"
    if { $activeIval != $::Ibrowser(idNone) } {
        for { set i $start } { $i <= $stop } { incr i } {
            #--- set up the progress bar
            set progress [ expr double ($pcount) / double ($numvols) ]
            IbrowserUpdateProgressBar $progress "::"
            #--- without something to make the application idle,
            #--- tk will not handle the drawing of progress bar.
            #--- Instead of calling update, which could cause
            #--- some unstable event loop, we just print some
            #--- '.'s to the tkcon. Slows things down a little,
            #--- but not terribly much. Better ideas are welcome.
            IbrowserPrintProgressFeedback
            
            #--- set the volume param
            set ::Volume(activeID) $i
            if { $ignoreVal } {
                MainVolumesSetParam $setThis
            } else {
                MainVolumesSetParam $setThis $thing
            }
            incr pcount
        }
        IbrowserEndProgressFeedback
        #---shouldn't this be the one in the viewer?
        #--- and so shouldn't it be the one rendered?
        set ::Volume(activeID) $activeVol
        IbrowserUpdateMainViewer $::Ibrowser($activeIval,$::Ibrowser(ViewDrop),MRMLid)

        set tt "Set $setThis for all volumes in $::Ibrowser($activeIval,name)"
        IbrowserSayThis $tt 0
    }        
    IbrowserLowerProgressBar
}





#-------------------------------------------------------------------------------
# .PROC IbrowserSetIntervalParamActiveVol
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetIntervalParamActiveVol { setThis { thing -10} } {
    global Volume
    
    #---sets display parameters for the volumes in the active interval
    if {$thing == -10 } {
        set ignoreVal 1
    } else {
        set ignoreVal 0
    }
    set activeVol $::Volume(activeID)
    set activeIval $::Ibrowser(activeInterval)

    #for each mrml volume, set "setThis"
    if { $activeIval != $::Ibrowser(idNone) } {
        set i $activeVol
        #--- set the volume param
        set ::Volume(activeID) $i
        if { $ignoreVal } {
            MainVolumesSetParam $setThis
        } else {
            MainVolumesSetParam $setThis $thing
        }
        #---shouldn't this be the one in the viewer?
        #--- and so shouldn't it be the one rendered?
        #IbrowserUpdateMainViewer $::Ibrowser($activeIval,$::Ibrowser(ViewDrop),MRMLid)
        #puts "ActiveVol same?: viewdrop = $::Ibrowser(ViewDrop) active = $activeVol" 
        IbrowserUpdateMainViewer $activeVol
    }        
}
