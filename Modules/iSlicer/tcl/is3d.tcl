
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

if { [info command ::iwidgets::collapsablewidget] == "" } {
    # TODO - add this to the iSlicer package so it comes in with package require
    # - must be done up here to avoid being defined in is3d namespace
    global env
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/collapsablewidget.itk
}

if { [info command ::isvolume] == "" } {
    global env
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/isvolume.tcl
}

#########################################################
#
if {0} { ;# comment

is3d - a widget for looking at Slicer scenes in 3d

currently volume rendering

# TODO : 
    - almost everything
    

}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *is3d.background #000000 widgetDefault
option add *is3d.longitude 0 widgetDefault
option add *is3d.latitude 0 widgetDefault
option add *is3d.distance 800 widgetDefault
option add *is3d.colorscheme "gray" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class is3d] == "" } {

    itcl::class is3d {
      inherit iwidgets::Labeledwidget

      constructor {args} {}
      destructor {}

      # another iswidget (TODO - attach to a scene description rather than a single volume)
      public variable isvolume

      #
      # itk_options for widget options that may need to be
      # inherited or composed as part of other widgets
      # or become part of the option database
      #
      itk_option define -background background Background {}
      itk_option define -longitude longitude Longitude {}
      itk_option define -latitude latitude Latitude {}
      itk_option define -distance distance Distance {}
      itk_option define -colorscheme colorscheme Colorscheme {}

      # widgets for the control area
      variable _controls

      # vtk objects in the render
      variable _name
      variable _tkrw
      variable _ren
      variable _mapper
      variable _actor

      # volume rendering vtk objects
      variable _cast
      variable _opaxfer
      variable _gradxfer
      variable _colxfer
      variable _volprop
      variable _compfunc
      variable _volmapper
      variable _vol

      # methods
      method expose {} {}
      method winresize {} {}
      method actor {} {return $_actor}
      method mapper {} {return $_mapper}
      method ren {} {return $_ren}
      method tkrw {} {return $_tkrw}
      method rw {} {return [$_tkrw GetRenderWindow]}
      method camera {} {return [$_ren GetActiveCamera]}
      method longlatdist { {long 0} {lat 0} {dist 0} } {}
      method screensave { filename {imagetype "PNM"}} {} ;# TODO should be moved to superclass
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body is3d::constructor {args} {
    component hull configure -borderwidth 0


    # make a unique name associated with this object
    set _name [namespace tail $this]
    # remove dots from name so it can be used in widget names
    regsub -all {\.} $_name "_" _name

    #
    # build the controls
    # - TODO - split controls into separate class as it gets more complex
    #

    set _controls $itk_interior.controls_$_name
    ::iwidgets::collapsablewidget $_controls -labeltext "3d Controls"
    pack $_controls -side top -expand false -fill x
    set cs [$_controls childsite]

    set _slider $cs.slider
    scale $_slider -from -180 -to 180 -orient horizontal -command "$this configure -longitude "
    pack $_slider -side top -expand true -fill x

    
    #
    # build the vtk 3d viewer
    #
    set _tkrw $itk_interior.tkrw
    vtkTkRenderWidget $_tkrw -width 512 -height 512

    pack $_tkrw -expand true -fill both
    bind $_tkrw <Expose> "$this expose"
    bind $_tkrw <Configure> "$this winresize"

    set _ren ::ren_$_name
    set _mapper ::mapper_$_name
    set _actor ::actor_$_name
    catch "$_ren Delete"
    catch "$_mapper Delete"
    catch "$_actor Delete"

    vtkRenderer $_ren
    [$this rw] AddRenderer $_ren


    #
    # Initialize the widget based on the command line options.
    #
    eval itk_initialize $args
}


itcl::body is3d::destructor {} {
    destroy $_tkrw 
    catch "$_ren Delete"
    catch "$_cast Delete"
    catch "$_gradxfer Delete"
    catch "$_colxfer Delete"
    catch "$_volprop Delete"
    catch "$_compfunc Delete"
    catch "$_volmapper Delete"
    catch "$_vol Delete"
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#-------------------------------------------------------------------------------
# OPTION: -background
#
# DESCRIPTION: background color of the image viewer
#-------------------------------------------------------------------------------
itcl::configbody is3d::background {

  if {$itk_option(-background) == ""} {
    return
  }

  set scanned [scan $itk_option(-background) "#%02x%02x%02x" r g b]

  if { $scanned == 3 } {
      $_ren SetBackground [expr ($r/255.)] [expr ($g/255.)] [expr ($b/255.)]
      $this expose
  }

}

#-------------------------------------------------------------------------------
# OPTION: -longitude
#
# DESCRIPTION: rotate around the interior-superior axis
#-------------------------------------------------------------------------------
itcl::configbody is3d::longitude {
    $this longlatdist $itk_option(-longitude) $itk_option(-latitude) $itk_option(-distance) 
}

#-------------------------------------------------------------------------------
# OPTION: -latitude
#
# DESCRIPTION: rotate from the equator toward the north pole
#-------------------------------------------------------------------------------
itcl::configbody is3d::latitude {
    $this longlatdist $itk_option(-longitude) $itk_option(-latitude) $itk_option(-distance) 
}

#-------------------------------------------------------------------------------
# OPTION: -distance
#
# DESCRIPTION: distance from FocalPoint for long/lat view
#-------------------------------------------------------------------------------
itcl::configbody is3d::distance {
    $this longlatdist $itk_option(-longitude) $itk_option(-latitude) $itk_option(-distance) 
}

#-------------------------------------------------------------------------------
# OPTION: -colorscheme
#
# DESCRIPTION: pre-canned rendering parameters
#-------------------------------------------------------------------------------
itcl::configbody is3d::colorscheme {

    switch $itk_option(-colorscheme) {
        "gray" {
            # Halazar's suggestions:
            $_opaxfer RemoveAllPoints
            $_opaxfer AddPoint  0   0.0
            $_opaxfer AddPoint  10   0.0
            $_opaxfer AddPoint  20   0.8
            $_opaxfer AddPoint  40   1.0

            $_colxfer RemoveAllPoints
            $_colxfer AddRGBPoint      0.0  0.0 0.0 0.0
            $_colxfer AddRGBPoint      4.0  0.5 0.5 0.5
            $_colxfer AddRGBPoint     20.0  1.0 1.0 1.0

            $_gradxfer RemoveAllPoints
            $_gradxfer AddPoint  0   0.0
            $_gradxfer AddPoint  10   0.5
            $_gradxfer AddPoint  30   1.0

            #$_opaxfer RemoveAllPoints
            #$_opaxfer AddPoint  0   0.0
            #$_opaxfer AddPoint  20   0.0
            #$_opaxfer AddPoint  20   0.5
            #$_opaxfer AddPoint  65   0.8
            #$_opaxfer AddPoint  100   0.9
            #$_opaxfer AddPoint  30   1.0

            #$_colxfer RemoveAllPoints
            #$_colxfer AddRGBPoint      0.0 0.0 0.0 0.0
            #$_colxfer AddRGBPoint     10.0 0.5 0.5 0.5
            #$_colxfer AddRGBPoint     10.0 1.0 1.0 1.0
            #$_colxfer AddRGBPoint     128.0 1.0 1.0 1.0
        }
        "default" {
            $_opaxfer RemoveAllPoints
            $_opaxfer AddPoint  20   0.0
            $_opaxfer AddPoint  255  0.2

            $_colxfer RemoveAllPoints
            $_colxfer AddRGBPoint      0.0 0.0 0.0 0.0
            $_colxfer AddRGBPoint     64.0 1.0 0.0 0.0
            $_colxfer AddRGBPoint    128.0 0.0 0.0 1.0
            $_colxfer AddRGBPoint    192.0 0.0 1.0 0.0
            $_colxfer AddRGBPoint    255.0 0.0 0.2 0.0

            $_gradxfer RemoveAllPoints
            $_gradxfer AddPoint  0   0.0
            $_gradxfer AddPoint  10   0.5
            $_gradxfer AddPoint  30   1.0
        }
    }
}

itcl::configbody is3d::isvolume {

    set _cast ::cast_$_name
    set _opaxfer ::opaxfer_$_name
    set _gradxfer ::gradxfer_$_name
    set _colxfer ::colxfer_$_name
    set _volprop ::volprop_$_name
    set _compfunc ::compfunc_$_name
    set _volmapper ::volmapper_$_name
    set _vol ::vol_$_name
    catch "$_cast Delete"
    catch "$_opaxfer Delete"
    catch "$_gradxfer Delete"
    catch "$_colxfer Delete"
    catch "$_volprop Delete"
    catch "$_compfunc Delete"
    catch "$_volmapper Delete"
    catch "$_vol Delete"

    # volume rendering only works on char
    vtkImageCast $_cast
    $_cast SetInput [$isvolume imagedata]
    $_cast SetOutputScalarTypeToUnsignedChar
    
    # Create transfer mapping scalar value to opacity
    vtkPiecewiseFunction $_opaxfer

    # Create transfer mapping gradient to opacity
    vtkPiecewiseFunction $_gradxfer

    # Create transfer mapping scalar value to color
    vtkColorTransferFunction $_colxfer

    $this configure -colorscheme "gray"

    # The property describes how the data will look
    vtkVolumeProperty $_volprop
    $_volprop SetColor $_colxfer
    $_volprop SetScalarOpacity $_opaxfer
    $_volprop SetGradientOpacity $_gradxfer
    $_volprop ShadeOn

    # The mapper / ray cast function know how to render the data
    vtkVolumeRayCastCompositeFunction  $_compfunc
    if {1} {
        vtkVolumeRayCastMapper $_volmapper
        $_volmapper SetVolumeRayCastFunction $_compfunc
        $_volmapper SetSampleDistance 0.5
    } else {
        vtkVolumeTextureMapper2D $_volmapper
    }
    $_volmapper SetInput [$_cast GetOutput]

    # The volume holds the mapper and the property and
    # can be used to position/orient the volume
    vtkVolume $_vol
    $_vol SetMapper $_volmapper
    $_vol SetProperty $_volprop

    $_ren AddVolume $_vol
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------


itcl::body is3d::expose {} {
    $_tkrw Render
}

itcl::body is3d::winresize {} {
    global View

    # don't have the output extent change with the window size
    return;

}

proc is3d::dtor {d} {
    # pi rad = 180 deg => rad/deg = pi/180
    return [expr $d * 3.14159 / 180.]
}

itcl::body is3d::longlatdist { {long 0} {lat 0} {dist 0} } {

    # (0 1 0) is looking down nose 


    set long [is3d::dtor $long]
    set lat [is3d::dtor $lat]

    set x [expr $dist * sin($long)]
    set y [expr $dist * cos($long)]
    set z 0

    [$this camera] SetPosition $x $y $z
    [$this camera] SetViewUp 0 0 1

}

itcl::body is3d::screensave { filename {imagetype "PNM"} } {
# TODO should be moved to superclass

    set wif ::wif_$_name
    set imgw ::imgw_$_name
    catch "$wif Delete"
    catch "$imgw Delete"

    vtkWindowToImageFilter $wif 
    $wif SetInput [[$this tkrw] GetRenderWindow]

    switch $imagetype {
        "PNM" - "PPM" {
            vtkPNMWriter $imgw 
        }
        "JPG" - "JPEG" {
            vtkJPEGWriter $imgw 
        }
        "BMP" {
            vtkBMPWriter $imgw 
        }
        "PS" - "PostScript" - "postscript" {
            vtkPostScriptWriter $imgw 
        }
        "TIF" - "TIFF" {
            vtkTIFFWriter $imgw 
        }
        "PNG" {
            vtkPNGWriter $imgw 
        }
        default {
            error "unknown image format $imagetype; options are PNM, JPG, BMP, PS, TIFF, PNG"
        }
    }
        
    $imgw SetInput [$wif GetOutput]
    $imgw SetFileName $filename
    $imgw Write

    $imgw Delete
    $wif Delete
} 

proc is3d_demo {} {

    catch "destroy .is3ddemo"
    toplevel .is3ddemo
    wm title .is3ddemo "is3d demo"
    wm geometry .is3ddemo +30+50

    pack [isvolume .is3ddemo.isv] -fill both -expand true
    .is3ddemo.isv configure -resolution 256 
    .is3ddemo.isv configure -orientation axial
    .is3ddemo.isv configure -orientation coronal
    pack [is3d .is3ddemo.is3d -isvolume .is3ddemo.isv] -fill both -expand true
}

proc is3d_demo_movie { filename {steps 10} } {

    raise .is3ddemo

    set delta [expr 360. / $steps]

    set f 1
    for {set l 0} {$l <= 360} { set l [expr $l + $delta] } {
        puts -nonewline "$f..." ; flush stdout
        .is3ddemo.is3d configure -longitude $l
        .is3ddemo.is3d expose
        update
        #.is3ddemo.is3d screensave [format c:/tmp/is%04d.ppm $f]
        .is3ddemo.is3d screensave [format c:/tmp/is%04d.png $f] PNG
        incr f
    }

    return

    puts ""
    puts "encoding..."

    set ret [catch "exec c:/apps/cygwin/usr/local/bin/ffmpeg.exe -i c:/tmp/is%04d.ppm -y $filename" res]
    puts $res

    puts "deleting..."
    for {set ff 1} {$ff <= $f} {incr ff} {
        file delete [format c:/tmp/is%04d.ppm $ff]
    }
    puts "done."
}

