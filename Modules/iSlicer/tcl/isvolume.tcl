
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

if { [info command ::iwidgets::collapsablewidget] == "" } {
    # TODO - add this to the iSlicer package so it comes in with package require
    # - must be done up here to avoid being defined in isvolume namespace
    global env
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/collapsablewidget.itk
}

#########################################################
#
if {0} { ;# comment

isvolume - a widget for looking at Slicer volumes 

# TODO : 
    - built-in pan-zoom through reslice
    - export reslice output for use with MI
    - reslice sag, cor
    - default key and mouse bindings
    - overlay/label options
    - split controls to different widget class
    - it is not a good idea to access volumes by Name
      because names are not unique. By Id number is appropriate.    
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isvolume.background #000000 widgetDefault
option add *isvolume.orientation Axial widgetDefault
option add *isvolume.volume "None" widgetDefault
option add *isvolume.slice 128 widgetDefault
option add *isvolume.interpolation linear widgetDefault
option add *isvolume.resolution 256 widgetDefault
option add *isvolume.transform "" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isvolume] == "" } {

    itcl::class isvolume {
      inherit iwidgets::Labeledwidget

      constructor {args} {}
      destructor {}

      #
      # itk_options for widget options that may need to be
      # inherited or composed as part of other widgets
      # or become part of the option database
      #
      itk_option define -background background Background {}
      itk_option define -orientation orientation Orientation {}
      itk_option define -volume volume Volume "None"
      itk_option define -slice slice Slice 0
      itk_option define -interpolation interpolation Interpolation {linear}
      itk_option define -resolution resolution Resolution {256}
      itk_option define -transform transform Transform {}

      # widgets for the control area
      variable _controls
      variable _slider
      variable _orientmenu
      variable _resmenu
      variable _volmenu

      # vtk objects in the slice render
      variable _name
      variable _tkrw
      variable _ren
      variable _mapper
      variable _actor
      variable _None_ImageData

      # vtk objects for reslicing
      variable _ijkmatrix
      variable _reslice
      variable _xform
      variable _changeinfo

      # internal state variables
      variable _VolIdMap
      variable _volume_serial 0

      # methods
      method expose {}   {}
      method actor  {}   {return $_actor}
      method mapper {}   {return $_mapper}
      method ren    {}   {return $_ren}
      method tkrw   {}   {return $_tkrw}
      method rw     {}   {return [$_tkrw GetRenderWindow]}

      # Note: use SetUpdateExtent to get full volume in the imagedata
      method imagedata {} {return [$_reslice GetOutput]}

      method screensave { filename {imagetype "PNM"} } {} ;# TODO should be moved to superclass

      method volmenu_update {} {}
      method transform_update {} {}
      method slicer_volume { {name ""} } {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isvolume::constructor {args} {
    component hull configure -borderwidth 0


    # make a unique name associated with this object
    set _name [namespace tail $this]
    # remove dots from name so it can be used in widget names
    regsub -all {\.} $_name "_" _name

    #
    # build the controls
    # - TODO - split this into separate class as it gets more complex
    #

    set _controls $itk_interior.controls_$_name
    ::iwidgets::collapsablewidget $_controls -labeltext "Slice Controls"
    pack $_controls -side top -expand false -fill x
    set cs [$_controls childsite]

    set _slider $cs.slider
    scale $_slider -orient horizontal 
    $_slider set 128
    $_slider configure -command "$this configure -slice "
    pack $_slider -side top -expand true -fill x

    set _orientmenu $cs.orientmenu
    iwidgets::optionmenu $_orientmenu -labeltext "Or:" -labelpos w \
        -command "$this configure -orientation \[$_orientmenu get\]"
    $cs.orientmenu insert end "Axial"
    $cs.orientmenu insert end "Sagittal"
    $cs.orientmenu insert end "Coronal"

    set _resmenu $cs.resmenu
    iwidgets::optionmenu $_resmenu -labeltext "Res:" -labelpos w
    $cs.resmenu insert end "64"
    $cs.resmenu insert end "128"
    $cs.resmenu insert end "256"
    $cs.resmenu insert end "512"
    $cs.resmenu insert end "1024"
    $cs.resmenu select 2 ;# can't access itk_option from in constructor to get default? :(
    $_resmenu configure -command "$this configure -resolution \[$_resmenu get\]"

    set _volmenu $cs._volmenu
    iwidgets::optionmenu $_volmenu -labeltext "Bg:" -labelpos w \
        -command "$this configure -volume \[$_volmenu get\]"

    $this volmenu_update

    pack $_orientmenu $_resmenu $_volmenu -side top -expand true -fill x
    ::iwidgets::Labeledwidget::alignlabels $_orientmenu $_resmenu $_volmenu
    
    #
    # build the vtk image viewer
    #
    iwidgets::scrolledframe $itk_interior.sframe \
        -hscrollmode dynamic -vscrollmode dynamic -width 300 -height 300
    pack $itk_interior.sframe -fill both -expand true
    set cs [$itk_interior.sframe childsite]
    set _tkrw $cs.tkrw
    vtkTkRenderWidget $_tkrw -width 256 -height 256

    pack $_tkrw -expand true -fill both
    bind $_tkrw <Expose> "$this expose"

    set _ren ::ren_$_name
    set _mapper ::mapper_$_name
    set _actor ::actor_$_name
    set _None_ImageData ::imagedata_$_name
    catch "$_ren Delete"
    catch "$_mapper Delete"
    catch "$_actor Delete"
    catch "$_None_ImageData Delete"

    # put some default data into the None volume
    vtkImageData $_None_ImageData 
    set esrc ::tmpesource_$_name 
    vtkImageEllipsoidSource $esrc
    $esrc SetRadius 20 40 10
    $esrc SetCenter 64 64 64
    $esrc SetInValue 100
    $esrc SetWholeExtent 0 127 0 127 0 127
    $esrc SetOutput $_None_ImageData
    $esrc Update
    $esrc SetOutput ""
    $esrc Delete
    $_None_ImageData SetSpacing 2 2 2


    vtkRenderer $_ren
    [$this rw] AddRenderer $_ren
    vtkImageMapper $_mapper
    $_mapper SetInput $_None_ImageData 
    vtkActor2D $_actor
    $_actor SetMapper $_mapper
    $_ren AddActor2D $_actor

    # for reslicing
    set _ijkmatrix ::ijkmatrix_$_name
    set _reslice ::reslice_$_name
    set _xform ::xform_$_name
    set _changeinfo ::changeinfo_$_name
    catch "$_ijkmatrix Delete"
    catch "$_reslice Delete"
    catch "$_xform Delete"
    catch "$_changeinfo Delete"

    vtkMatrix4x4 $_ijkmatrix
    vtkImageReslice $_reslice
      $_reslice SetInput $_None_ImageData
      $_reslice SetInterpolationModeToLinear
    vtkTransform $_xform
    vtkImageChangeInformation $_changeinfo
      $_changeinfo CenterImageOn

    #
    # Initialize the widget based on the command line options.
    #
    eval itk_initialize $args
}


itcl::body isvolume::destructor {} {
    destroy $_tkrw 
    $_ren Delete
    $_mapper Delete
    $_actor Delete
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#-------------------------------------------------------------------------------
# OPTION: -background
#
# DESCRIPTION: background color of the image viewer
#-------------------------------------------------------------------------------
itcl::configbody isvolume::background {

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
# OPTION: -slice
#
# DESCRIPTION: slice number for the current volume
#-------------------------------------------------------------------------------
itcl::configbody isvolume::slice {

    if {$itk_option(-slice) == ""} {
        return
    }
    $_mapper SetZSlice $itk_option(-slice)
    $_slider set $itk_option(-slice)
    $this expose
}

#-------------------------------------------------------------------------------
# OPTION: -volume
#
# DESCRIPTION: which slicer volume to display in this isvolume
# The argument can be the volume name or the volume Id. The volume
# Id is strongly prefered because it is unique.
#-------------------------------------------------------------------------------
itcl::configbody isvolume::volume {
    global Volume

    set volname $itk_option(-volume)

    if { $volname == "" } {
        set volname "None"
    }

    set id $_VolIdMap($volname)

    if { ![info exists _VolIdMap($id)] } {
        error "bad volume id $id for $volname"
    }

    if { $id == "None" || $id == $Volume(idNone)} {
        $_changeinfo SetInput $_None_ImageData 
    } else {
        $_changeinfo SetInput [Volume($id,vol) GetOutput]
    }

    $_reslice SetInput [$_changeinfo GetOutput]

    $_mapper SetInput [$_reslice GetOutput]
    $_mapper SetColorWindow [Volume($id,node) GetWindow]
    $_mapper SetColorLevel [Volume($id,node) GetLevel]

    $this transform_update
    $this expose
}

#-------------------------------------------------------------------------------
# OPTION: -orientation
#
# DESCRIPTION: which slicer volume to display in this isvolume
#-------------------------------------------------------------------------------
itcl::configbody isvolume::orientation {

    # don't change the orientation of the None volume
    # also allow for an empty orientation option
    if { $itk_option(-orientation) == "" || 
            $itk_option(-volume) == "" || $itk_option(-volume) == "None" } {
        return
    } else {
        $_mapper SetInput [$_reslice GetOutput]
    }

    $this transform_update
    $this configure -resolution $itk_option(-resolution)
}

itcl::configbody isvolume::interpolation {
    switch $itk_option(-interpolation) {
        "linear" { set mode Linear }
        "cubic" { set mode Cubic }
        "nearest" -
        "nearestneighbor" { set mode NearestNeighbor }
        default {
            error "must be nearest, linear, or cubic"
        }
    }
    $_reslice SetInterpolationModeTo$mode
    $this expose
}

itcl::configbody isvolume::resolution {

    set opos [expr ([$this cget -slice] * 1.0) / [$_slider cget -to] ]
    set res $itk_option(-resolution)
    set spacing [expr $::View(fov) / (1.0 * $res)]

    $_reslice SetOutputSpacing $spacing $spacing $spacing 
    set ext [expr $res -1]
    $_reslice SetOutputExtent 0 $ext 0 $ext 0 $ext

    $_slider configure -from 0
    $_slider configure -to $res
    $this configure -slice [expr round( $opos * $res )]
    $_tkrw configure -width $res -height $res

    $this transform_update
}

itcl::configbody isvolume::transform {

    $this transform_update
    $this expose
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------


itcl::body isvolume::expose {} {
    $_tkrw Render
}

# ------------------------------------------------------------------

#-------------------------------------------------------------------------------
# METHOD: volmenu_update
#
# DESCRIPTION: create the array of volume names and ids
# - use the id to form a unique name in the menu
# - id map can be accessed by name, id, or name__id
#-------------------------------------------------------------------------------
itcl::body isvolume::volmenu_update {} {

    if { [info exists itk_option(-volume)] && $itk_option(-volume) != "" } {
        set v $itk_option(-volume)
        if { [info exists _VolIdMap($v)] } {
            set current_id $_VolIdMap($v)
        } else {
            set current_id -1
        }
    } else {
        set current_id -1
    }

    array unset _VolIdMap

    foreach id $::Volume(idList) {
        set name [Volume($id,node) GetName]
        set _VolIdMap($name)  $id
        set _VolIdMap($id)    $id
        set _VolIdMap(${name}__$id)  $id 
    }

    set ocmd [$_volmenu cget -command]
    $_volmenu configure -command ""
    $_volmenu delete 0 end
    foreach id $::Volume(idList) {
        set name [Volume($id,node) GetName]
        $_volmenu insert end  ${name}__$id
    }
    $_volmenu configure -command $ocmd

    set idindex [lsearch $::Volume(idList) $current_id]
    if { $idindex == -1 } {
        $_volmenu select end
    } else {
        $_volmenu select $idindex
    }
}


#-------------------------------------------------------------------------------
# METHOD: transform_update
#
# DESCRIPTION: recalculate the transform parameters
#             
#            
#           
#-------------------------------------------------------------------------------
itcl::body isvolume::transform_update {} {

    if { ![info exists itk_option(-volume)] || $itk_option(-volume) == "" } {
        return
    }

    set id $_VolIdMap($itk_option(-volume))

    #
    # first, make the transform to put the images
    # into axial RAS space.  Center the volume resliced output
    # around the origin of RAS space
    # - size of RAS space is cube of size fov
    # - output extent of RAS is a to-be-defined resolution
    # - a to-be-defined pan-zoom transform will map to the screen
    #

    $_ijkmatrix Identity
    switch [Volume($id,node) GetScanOrder] {
        #                    -R        -A        -S
        "LR" { set axes {  0  0 -1  -1  0  0   0  1  0 } }
        "RL" { set axes {  0  0  1  -1  0  0   0  1  0 } }
        "IS" { set axes {  1  0  0   0  1  0   0  0  1 } }
        "SI" { set axes {  1  0  0   0  1  0   0  0 -1 } }
        "PA" { set axes {  1  0  0   0  0  1   0  1  0 } }
        "AP" { set axes {  1  0  0   0  0  1   0 -1  0 } }
        #TODO - gantry tilt not supported
    }

    set ii 0
    for {set i 0} {$i < 3} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            # transpose for inverse - reslice transform requires it
            $_ijkmatrix SetElement $j $i [lindex $axes $ii]
            incr ii
        }
    }


    # TODO - add other orientations here...
    catch "transposematrix Delete"
    vtkMatrix4x4 transposematrix
    switch $itk_option(-orientation) {
        "axial" -
        "Axial" {
            # nothing, this is the default
            transposematrix Identity
        }
        "sagittal" -
        "Sagittal" {
            transposematrix DeepCopy \
                0  0  1  0 \
               -1  0  0  0 \
                0  1  0  0 \
                0  0  0  1    
        }
        "coronal" -
        "Coronal" {
            transposematrix DeepCopy \
                1  0  0  0 \
                0  0  1  0 \
                0  1  0  0 \
                0  0  0  1    
        }
        "AxiSlice" -
        "SagSlice" -
        "CorSlice" {
            # nothing yet
        }
    }
    #
    # need to invert the X axis to handle slicer transform
    #
    for {set i 0} {$i < 4} {incr i} {
        transposematrix SetElement 0 $i [expr -1 * [transposematrix GetElement 0 $i]]
    }

    #
    # make a matrix of the supplied transform - positions the volume
    # in RAS space (inverted for use with ImageReslice)
    # - use the config option transform if supplied, otherwise get from node
    #
    catch "transformmatrix Delete"
    vtkMatrix4x4 transformmatrix

    if { $itk_option(-transform) != "" } {
        eval transformmatrix DeepCopy $itk_option(-transform)
    } else {
        transformmatrix DeepCopy [Volume($id,node) GetRasToWld]
    }

    transformmatrix Invert
    for {set i 0} {$i < 4} {incr i} {
        transformmatrix SetElement 0 $i [expr -1 * [transformmatrix GetElement 0 $i]]
    }

    #
    # now combine the matrices
    # - _ijkmatrix goes from image voxels to RAS
    # - transformmatrix moves the volume in RAS space
    # - transposematrix picks an orientation to reslice RAS space
    #
    $_ijkmatrix Multiply4x4 $_ijkmatrix transformmatrix $_ijkmatrix
    $_ijkmatrix Multiply4x4 $_ijkmatrix transposematrix $_ijkmatrix
    transformmatrix Delete
    transposematrix Delete

    $_xform SetMatrix $_ijkmatrix
    $_reslice SetResliceTransform $_xform 

}


# ------------------------------------------------------------------

itcl::body isvolume::screensave { filename {imagetype "PNM"} } {
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

# ------------------------------------------------------------------

itcl::body isvolume::slicer_volume { {name ""} } {

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i
    
    # find a name for the image data that hasn't been taken yet
    while {1} {
        set id id_$_name$_volume_serial
        if { [info command $id] == "" } {
            break ;# found a free name
        } else {
            incr _volume_serial ;# need to try again
        }
    }

    # set the name and description of the volume
    if { $name == "" } { 
        $n SetName isvolume-$_volume_serial
    } else {
        $n SetName $name
    }
    $n SetDescription "Resampled volume"
    incr _volume_serial

    #
    # need to construct a volume from the slicer output
    #

    vtkImageData $id
    eval [$this imagedata] SetUpdateExtent [[$this imagedata] GetWholeExtent]
    [$this imagedata] Update
    $id DeepCopy [$this imagedata]

    eval ::Volume($i,node) SetSpacing [$id GetSpacing]

    switch $itk_option(-orientation) {
        Axial { ::Volume($i,node) SetScanOrder IS }
        Sagittal { ::Volume($i,node) SetScanOrder LR }
        Coronal { ::Volume($i,node) SetScanOrder PA }
    }
    ::Volume($i,node) SetNumScalars 1
    ::Volume($i,node) SetScalarType [$id GetScalarType]
    ::Volume($i,node) SetDimensions [lindex [$id GetDimensions] 0] [lindex [$id GetDimensions] 1]
    ::Volume($i,node) SetImageRange 0 [expr $itk_option(-resolution) - 1]

    ::Volume($i,node) ComputeRasToIjkFromScanOrder [::Volume($i,node) GetScanOrder]

    MainUpdateMRML

    Volume($i,vol) SetImageData $id

    Slicer SetOffset 0 0
    MainSlicesSetVolumeAll Back $i
    RenderAll
}

# ------------------------------------------------------------------

proc isvolume_demo {} {

    catch "destroy .isvolumedemo"
    toplevel .isvolumedemo
    wm title .isvolumedemo "isvolume demo"
    #wm geometry .isvolumedemo 400x700

    pack [isvolume .isvolumedemo.isv] -fill both -expand true

    if { [lsearch $::Module(idList) iSlicer] == -1 } {
        lappend ::Module(idList) iSlicer
        set ::Module(iSlicer,procMRML) iSlicerUpdateMRML
    }
}

proc iSlicerUpdateMRML {} {
    
    foreach isv [itcl::find objects -class isvolume] {
        $isv volmenu_update
    }
}

proc isvolume_transform_test {} {

    lappend ::Module(idList) iSlicer
    set ::Module(iSlicer,procMRML) iSlicerUpdateMRML
}



