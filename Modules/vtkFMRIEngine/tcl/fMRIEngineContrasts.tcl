proc FMRIEngineAddOrEditContrast {} {
    global FMRIEngine 

    # Error checking
    set name [string trim $FMRIEngine(entry,contrastName)]
    if {$name == ""} {
        DevErrorWindow "Input a unique name for this contrast."
        return
    }

    set vec [string trim $FMRIEngine(entry,contrastVector)]
    if {$vec == ""} {
        DevErrorWindow "Input a contrast vector."
        return
    }

    if {! [info exists FMRIEngine($name,contrastName)]} {
        $FMRIEngine(contrastsListBox) insert end $name 
    } else {
        if {$name == $FMRIEngine($name,contrastName) &&
            $vec == $FMRIEngine($name,contrastVector)} {
            DevErrorWindow "The following contrast has been added:\nName: $name\nVector: $vec"
        }
    }

    set FMRIEngine($name,contrastName) $name
    set FMRIEngine($name,contrastVector) $vec
}


proc FMRIEngineDeleteContrast {} {
    global FMRIEngine 

    set curs [$FMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$FMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            unset -nocomplain FMRIEngine($name,contrastName) 
            unset -nocomplain FMRIEngine($name,contrastVector)
        }

        $FMRIEngine(contrastsListBox) delete $curs 
    }
}


proc FMRIEngineShowContrastToEdit {} {
    global FMRIEngine 

    set curs [$FMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$FMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            set FMRIEngine(entry,contrastName) $name
            set FMRIEngine(entry,contrastVector) $FMRIEngine($name,contrastVector) 
        }
    }
}


 
