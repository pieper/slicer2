
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

isframes - a widget for looking at a sequence of frames

# TODO : 
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isframes.frame 0 widgetDefault
option add *isframes.start 0 widgetDefault
option add *isframes.end 0 widgetDefault
option add *isframes.skip 1 widgetDefault
option add *isframes.filepattern "" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isframes] == "" } {

    itcl::class isframes {
      inherit iwidgets::Labeledwidget

      constructor {args} {}
      destructor {}

      #
      # itk_options for widget options that may need to be
      # inherited or composed as part of other widgets
      # or become part of the option database
      #
      itk_option define -filepattern filepattern Filepattern ""
      itk_option define -frame frame Frame 0
      itk_option define -start start Start 0
      itk_option define -end end End 0
      itk_option define -skip skip Skip 1

      # widgets for the control area
      variable _task
      variable _slider

      # state variables
      variable _first
      variable _last

      # vtk objects in the frame render
      variable _name
      variable _tkrw
      variable _ren
      variable _mapper
      variable _actor

      # methods
      method expose {}   {}
      method actor  {}   {return $_actor}
      method mapper {}   {return $_mapper}
      method ren    {}   {return $_ren}
      method tkrw   {}   {return $_tkrw}
      method rw     {}   {return [$_tkrw GetRenderWindow]}

      method next     {}   {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isframes::constructor {args} {
    component hull configure -borderwidth 0


    # make a unique name associated with this object
    set _name [namespace tail $this]
    # remove dots from name so it can be used in widget names
    regsub -all {\.} $_name "_" _name

    #
    # build the controls
    # - TODO - split this into separate class as it gets more complex
    #

    set _task $itk_interior.task
    istask $_task -taskcommand "$this next; update"
    pack $_task -side top -expand false -fill x

    set _slider $itk_interior.slider
    scale $_slider -orient horizontal -command "$this configure -frame "
    pack $_slider -side top -expand false -fill x

    
    #
    # build the vtk image viewer
    #
    iwidgets::scrolledframe $itk_interior.sframe \
        -hscrollmode dynamic -vscrollmode dynamic 
    pack $itk_interior.sframe -fill both -expand true
    set cs [$itk_interior.sframe childsite]
    set _tkrw $cs.tkrw
    vtkTkRenderWidget $_tkrw -width 256 -height 256

    pack $_tkrw -expand true -fill both
    bind $_tkrw <Expose> "$this expose"

    set _ren ::ren_$_name
    set _mapper ::mapper_$_name
    set _actor ::actor_$_name
    catch "$_ren Delete"
    catch "$_mapper Delete"
    catch "$_actor Delete"

    vtkRenderer $_ren
    [$this rw] AddRenderer $_ren
    vtkImageMapper $_mapper
    $_mapper SetColorWindow 255
    $_mapper SetColorLevel 128
    vtkActor2D $_actor
    $_actor SetMapper $_mapper
    $_ren AddActor2D $_actor

    #
    # Initialize the widget based on the command line options.
    #
    eval itk_initialize $args
}


itcl::body isframes::destructor {} {
    destroy $_tkrw 
    $_ren Delete
    $_mapper Delete
    $_actor Delete
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#-------------------------------------------------------------------------------
# OPTION: -filepattern
#
# DESCRIPTION: e.g. c:/tmp/frames-%0d.jpg
#-------------------------------------------------------------------------------
itcl::configbody isframes::filepattern {
}

#-------------------------------------------------------------------------------
# OPTION: -start , -end, -skip
#
# DESCRIPTION: first and last frames of movie, used to adjust slider
#-------------------------------------------------------------------------------
itcl::configbody isframes::start {
    $_slider configure -from $itk_option(-start)
}
itcl::configbody isframes::end {
    $_slider configure -to $itk_option(-end)
}
itcl::configbody isframes::skip {
    $_slider configure -resolution $itk_option(-skip)
}

#-------------------------------------------------------------------------------
# OPTION: -frame
#
# DESCRIPTION: frame number for the current sequence
#-------------------------------------------------------------------------------
itcl::configbody isframes::frame {

    if { $itk_option(-frame) == "" || $itk_option(-filepattern) == "" } {
        return
    }

    if { [$_slider get] != $itk_option(-frame) } {
        $_slider set $itk_option(-frame)
    }

    set imgr ::imgr_$_name
    catch "$imgr Delete"

    set filename [format $itk_option(-filepattern) $itk_option(-frame)]

    set ext [string tolower [file extension $filename]]
    switch $ext {
        ".pnm" - ".ppm" - ".pgm" {
            vtkPNMReader $imgr 
        }
        ".jpg" - ".jepg" {
            vtkJPEGReader $imgr 
        }
        ".bmp" {
            vtkBMPReader $imgr 
        }
        ".ps"  {
            vtkPostScriptReader $imgr 
        }
        ".tif" - ".tiff" {
            vtkTIFFReader $imgr 
        }
        ".png" {
            vtkPNGReader $imgr 
        }
        default {
            error "unknown image format $ext; options are .ppm, .jpg, .bmp, .ps, .tif, .png"
        }
    }

    $imgr SetFileName $filename
    $imgr Update
    set dims [[$imgr GetOutput] GetDimensions]
    $_tkrw configure -width [lindex $dims 0] -height [lindex $dims 1]
    $_mapper SetInput [$imgr GetOutput]
    $imgr Delete
    
    $this expose
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------


itcl::body isframes::expose {} {
    $_tkrw Render
}


itcl::body isframes::next {} {
    
    set f [expr $itk_option(-frame) + $itk_option(-skip)]
    if { $f > $itk_option(-end) } {
        set f $itk_option(-start)
    }
    $this configure -frame $f
}


# ------------------------------------------------------------------

proc isframes_demo {} {

    catch "destroy .isframesdemo"
    toplevel .isframesdemo
    wm title .isframesdemo "isframes demo"
    wm geometry .isframesdemo 400x700

    pack [isframes .isframesdemo.isf] -fill both -expand true
    .isframesdemo.isf configure -filepattern c:/tmp/slicer-%04d.png -start 1 -end 65
    .isframesdemo.isf configure -frame 1
}



