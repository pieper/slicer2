
#
# experimental slicer daemon - sp 2005-09-23
# - meant to be as simple as possible
#

# this package contains the vtkTclHelper
package require vtkQueryAtlas

#
# returns a listening socket on given port or uses default
# - local only for security, use ssh tunnels for remote access
#
proc slicerd_start { {port 18943} } {

    set sock [socket -server slicerd_sock_cb $port]

    return $sock
}

#
# shuts down the socket
#
proc slicerd_stop { sock } {

    set _tcl ::tcl_$sock
    catch "$_tcl Delete"

    close $sock
}

#
# accepts new connections
#
proc slicerd_sock_cb { sock addr port } {

    #
    # create a tcl helper for this socket
    # then set up a callback for when the socket becomes readable
    #
    set _tcl ::tcl_$sock
    catch "$_tcl Delete"
    vtkTclHelper $_tcl

    # special trick to let the tcl helper know what interp to use
    set tag [$_tcl AddObserver ModifiedEvent ""]
    $_tcl SetInterpFromCommand $tag

    fileevent $sock readable "slicerd_sock_fileevent $sock"
}

#
# handles input on the connection
#
proc slicerd_sock_fileevent {sock} {

    if { [eof $sock] } {
        close $sock
        set _tcl ::tcl_$sock
        catch "$_tcl Delete"
        return
    }

    gets $sock line

    switch -glob $line {
        "ls*" {
            puts "listing of scene"
            foreach volid $::Volume(idList) {
                puts -nonewline $sock "$volid \"[Volume($volid,node) GetName]\" "
            }
            puts $sock ""
            flush $sock
        }
        "get*" {
            if { [llength $line] < 2 } {
                puts $sock "get error: missing id"
                flush $sock
                return
            }
            set volid [lindex $line 1]
            if { [info command Volume($volid,vol)] == "" } {
                # if the id isn't a number, try looking by name as a pattern
                set ids [MainVolumesGetVolumesByNamePattern $volid]
                if { [llength $ids] != 1 } {
                    puts $sock "get error: bad id"
                    flush $sock
                    return
                } 
                set volid $ids ;# there's only one and it's our target
            }
            # TODO: should add direction cosines and label_map status
            set im [Volume($volid,vol) GetOutput]
            puts $sock "image $volid" 
            puts $sock "dimensions [$im GetDimensions]" 
            puts $sock "spacing [$im GetSpacing]" 
            puts $sock "components [$im GetNumberOfScalarComponents]" 
            puts $sock "scalar_type [$im GetScalarType]" 
            ::tcl_$sock SetImageData $im
            fconfigure $sock -translation binary
            ::tcl_$sock SendImageDataScalars
            fconfigure $sock -translation auto
            flush $sock
        }
        "put" {
            gets $sock line
            if { ![string match "image *" $line] } {
                puts $sock "put error: bad protocol"
                flush $sock
                return
            }
            set name [lindex $line 1]
            if { $name == "" } {
                set name "slicerd"
            }

            gets $sock dimensions
            set dimensions [lrange $dimensions 1 3]
            gets $sock space_origin
            set space_origin [lindex $space_origin 1]
            gets $sock space_directions
            set space_directions [lrange $space_directions 1 3]
            gets $sock components
            set components [lindex $components 1]
            gets $sock scalar_type
            set scalar_type [lindex $scalar_type 1]


            # add a mrml node
            set n [MainMrmlAddNode Volume]
            set i [$n GetID]
            MainVolumesCreate $i
    
            $n SetName $name
            $n SetDescription "Imported via slicerd"
    
            set idata ::imagedata-[info cmdcount]
            vtkImageData $idata
            eval $idata SetDimensions $dimensions
            $idata SetNumberOfScalarComponents $components
            $idata SetScalarType $scalar_type
            $idata AllocateScalars

            ::tcl_$sock SetImageData $idata
            fconfigure $sock -translation binary -encoding binary
            ::tcl_$sock ReceiveImageDataScalars $sock
            fconfigure $sock -translation auto

            ::Volume($i,node) SetNumScalars $components
            ::Volume($i,node) SetScalarType $scalar_type

    
            ::Volume($i,node) SetDimensions [lindex $dimensions 0] [lindex $dimensions 1]
            ::Volume($i,node) SetImageRange 1 [lindex $dimensions 2]
            catch "flip Delete"
            vtkImageFlip flip
            flip SetFilteredAxis 1
            flip SetInput $idata
            flip Update
            Volume($i,vol) SetImageData [flip GetOutput]
            Volume($i,vol) Update
            slicerd_parse_space_directions $i $space_origin $space_directions
            $idata Delete
            flip Delete
            MainUpdateMRML

            Slicer SetOffset 0 0
            MainSlicesSetVolumeAll Back $i
            RenderAll
        }
        "eval*" {
            puts $sock [eval $line]
            flush $sock
        }
        default {
            puts $sock "unknown command $line"
            flush $sock
        }
    }
}

#
# convert nrrd-style space directions line into vtk/slicer info
# - unfortunately, this is some nasty math to do in tcl
#
proc slicerd_parse_space_directions {volid space_origin space_directions} {

    #
    # parse the 'space directions' and 'space origin' information into
    # a slicer RasToIjk and related matrices by telling the mrml node
    # the RAS corners of the volume
    #

    regsub -all "\\(" $space_origin " " space_origin
    regsub -all "\\)" $space_origin " " space_origin
    regsub -all "\\," $space_origin " " space_origin
    regsub -all "\\(" $space_directions " " space_directions
    regsub -all "\\)" $space_directions " " space_directions
    regsub -all "\\," $space_directions " " space_directions


    #
    # normalize and save length for each space direction vector
    #
    set spacei 0
    foreach dir [lrange $space_directions 0 2] {
        set spacei [expr $spacei + $dir * $dir]
    }
    set spacei [expr sqrt($spacei)]
    set unit_space_directions ""
    foreach dir [lrange $space_directions 0 2] {
        lappend unit_space_directions [expr $dir / $spacei]
    }

    set spacej 0
    foreach dir [lrange $space_directions 3 5] {
        set spacej [expr $spacej + $dir * $dir]
    }
    set spacej [expr sqrt($spacej)]
    foreach dir [lrange $space_directions 3 5] {
        lappend unit_space_directions [expr $dir / $spacej]
    }

    set spacek 0
    foreach dir [lrange $space_directions 6 8] {
        set spacek [expr $spacek + $dir * $dir]
    }
    set spacek [expr sqrt($spacek)]
    foreach dir [lrange $space_directions 6 8] {
        lappend unit_space_directions [expr $dir / $spacek]
    }
    
puts "SetSpacing $spacei $spacej $spacek"

    ::Volume($volid,node) SetSpacing $spacei $spacej $spacek

puts $unit_space_directions

    #
    # fill the ijk to ras matrix
    #
    catch "Ijk_matrix Delete"
    vtkMatrix4x4 Ijk_matrix
    Ijk_matrix Identity
    for {set i 0} {$i < 3} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set val [lindex $space_directions [expr 3 * $i + $j]]
            Ijk_matrix SetElement $j $i $val
        }
        set val [lindex $space_origin $i]
        Ijk_matrix SetElement $i 3 $val
    }

puts "Ijk_matrix Print"
puts [Ijk_matrix Print]

    set dims [[Volume($volid,vol) GetOutput] GetDimensions]

    # first top left - start at zero, and add origin to all later
    set ftl "0 0 0"
    # first top right = width * row vector
    set ftr [lrange [Ijk_matrix MultiplyPoint [lindex $dims 0] 0 0 0] 0 2]
    # first bottom right = ftr + height * column vector
    set column_vec [lrange [Ijk_matrix MultiplyPoint 0 [lindex $dims 1] 0 0] 0 2]
    set fbr ""
    foreach ftr_e $ftr column_vec_e $column_vec {
        lappend fbr [expr $ftr_e + $column_vec_e]
    }
    # last top left = ftl + slice vector  (and ftl is zero)
    set ltl [lrange [Ijk_matrix MultiplyPoint 0 0 [lindex $dims 2] 0] 0 2]

    puts "ftl ftr fbr ltl"
    puts "$ftl   $ftr   $fbr   $ltl"

    # add the origin offset 
    set origin [lrange [Ijk_matrix MultiplyPoint 0 0 0 1] 0 2]
    foreach corner "ftl ftr fbr ltl" {
        set new_corner ""
        foreach corner_e [set $corner] origin_e $origin {
            lappend new_corner [expr $corner_e + $origin_e]
        }
        set $corner $new_corner
    }

    puts "ftl ftr fbr ltl"
    puts "$ftl   $ftr   $fbr   $ltl"
    eval Volume($volid,node) ComputeRasToIjkFromCorners "0 0 0" $ftl $ftr $fbr "0 0 0" $ltl

    Ijk_matrix Delete
}
