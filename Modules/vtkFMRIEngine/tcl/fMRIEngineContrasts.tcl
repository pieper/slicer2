proc fMRIEngineAddOrEditContrast {} {
    global fMRIEngine 

    # Error checking
    set name [string trim $fMRIEngine(entry,contrastName)]
    if {$name == ""} {
        DevErrorWindow "Input a unique name for this contrast."
        return
    }

    set vec [string trim $fMRIEngine(entry,contrastVector)]
    if {$vec == ""} {
        DevErrorWindow "Input a contrast vector."
        return
    }

    if {! [info exists fMRIEngine($name,contrastName)]} {
        $fMRIEngine(contrastsListBox) insert end $name 
    } else {
        if {$name == $fMRIEngine($name,contrastName) &&
            $vec == $fMRIEngine($name,contrastVector)} {
            DevErrorWindow "The following contrast has been added:\nName: $name\nVector: $vec"
        }
    }

    set fMRIEngine($name,contrastName) $name
    set fMRIEngine($name,contrastVector) $vec
}


proc fMRIEngineDeleteContrast {} {
    global fMRIEngine 

    set curs [$fMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$fMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            unset -nocomplain fMRIEngine($name,contrastName) 
            unset -nocomplain fMRIEngine($name,contrastVector)
        }

        $fMRIEngine(contrastsListBox) delete $curs 
    }
}


proc fMRIEngineShowContrastToEdit {} {
    global fMRIEngine 

    set curs [$fMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$fMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            set fMRIEngine(entry,contrastName) $name
            set fMRIEngine(entry,contrastVector) $fMRIEngine($name,contrastVector) 
        }
    }
}


 
