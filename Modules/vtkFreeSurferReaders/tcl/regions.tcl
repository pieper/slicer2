
package require Iwidgets



#
# a toplevel window that displays cortical parcellation options
# - meant to be sourced into the slicer
#

if { [itcl::find class regions] == "" } {

    itcl::class regions {

        public variable model "" {}
        public variable labelfile "" {}
        public variable annotfile "" {}
        public variable browser {c:/Program Files/mozilla.org/Mozilla/mozilla.exe} {}
        public variable site "google" {}

        variable _name ""
        variable _w ""
        variable _labellistbox ""
        variable _modelmenu ""
        variable _id ""

        variable _labels

        variable _ptscalars ""
        variable _ptlabels ""

        constructor {args} {}
        destructor {}

        method apply {} {}
        method query {} {}
        method findptscalars {} {}
        method demo {} {}
    }
}

itcl::body regions::constructor {args} {
    global Model

    set _name [namespace tail $this]
    set _w .$_name

    toplevel $_w
    wm title $_w "Parcellation Options"

    #
    # configuration panel
    #
    iwidgets::Labeledframe $_w.config -labeltext "Configuation" -labelpos nw
    pack $_w.config -fill both -expand true

    set cs [$_w.config childsite]

    set _modelmenu [iwidgets::Optionmenu $cs.model]
    $_modelmenu configure -labeltext "Model:" -command "$this configure -model \[$_modelmenu get\]"
    foreach i $Model(idList) {
        set name [Model($i,node) GetName]
        $_modelmenu insert end $name
        if { $model == "" } {
            set model $name
            set _id $i
        }
    }
    pack $_modelmenu -expand true -fill x

    frame $cs.label
    pack $cs.label -expand true -fill x
    iwidgets::entryfield $cs.label.elabel \
        -textvariable [itcl::scope labelfile] \
        -labeltext "Label File:"
    pack $cs.label.elabel -side left -expand true -fill x
    button $cs.label.blabel -text "Browse..." \
        -command "$this configure -labelfile \[tk_getOpenFile -filetypes { { {Label Files} {.txt} } } -initialfile \[$this cget -labelfile\] \]"
    pack $cs.label.blabel -side left

    frame $cs.annot
    pack $cs.annot -expand true -fill x
    iwidgets::entryfield $cs.annot.eannot \
        -textvariable [itcl::scope annotfile] \
        -labeltext "Annotation File:"
    pack $cs.annot.eannot -side left -expand true -fill x
    button $cs.annot.bannot -text "Browse..." \
        -command "$this configure -annotfile \[tk_getOpenFile -filetypes { {{Parsed Annotations} {.pannot}} {{Annotation Files} {.annot}} } -initialfile \[$this cget -annotfile\] \]"
    pack $cs.annot.bannot -side left

    ::iwidgets::Labeledwidget::alignlabels $_modelmenu $cs.label.elabel $cs.annot.eannot 

    button $cs.apply -text "Apply" -command "$this apply"
    pack $cs.apply


    #
    # fiducials panel
    #

    ::iwidgets::Labeledframe $_w.fiducials -labeltext "Fiducials" -labelpos nw
    pack $_w.fiducials -fill both -expand true

    set cs [$_w.fiducials childsite]

    set _labellistbox $cs.lb
    ::iwidgets::Scrolledlistbox $_labellistbox -hscrollmode dynamic -vscrollmode dynamic
    pack $_labellistbox -fill both -expand true
    
    button $cs.update -text "Update" -command "$this findptscalars"
    pack $cs.update -side left

    button $cs.query -text "Query" -command "$this query"
    pack $cs.query -side left


    eval configure $args
}

itcl::body regions::destructor {} {

    catch "destroy $_w"
}

itcl::configbody regions::model {
    global Model

    set _id -1
    foreach i $Model(idList) {
        if { $model == [Model($i,node) GetName] } {
            set _id $i
            break
        }
    }

    if { $_id == -1 } {
        DevErrorWindow "Can't find model named ${model}."
        return
    }
}

itcl::body regions::apply {} {
    global Model

    if { $model == "" || $labelfile == "" || $annotfile == "" } {
        DevErrorWindow "Please select a model, label file, and annotation file."
        return
    }

    if { [file ext $annotfile] != ".pannot" } {
        DevErrorWindow "Can only handle parsed annotations (.pannot) currently."
        return
    }

    set lut [Model($_id,mapper,viewRen) GetLookupTable]

    $lut SetRampToLinear

    #
    # parse the label file
    #
    array unset _labels
    set fp [open $labelfile "r"]
    while { ![eof $fp] } {
        gets $fp line
        scan $line "%d %s %d %d %d %d" idx name r g b other
        set _labels($idx,name) $name
        set _labels($idx,rgb) "$r $g $b"
        set packed_rgb [format "%02x%02x%02x" $b $g $r]
        set _labels($idx,packed_rgb) $packed_rgb
        set _labels($packed_rgb,idx) $idx

        set rr [expr $r / 255.]
        set gg [expr $g / 255.]
        set bb [expr $b / 255.]
        $lut SetTableValue $idx $rr $gg $bb 1
    }
    close $fp

    #
    # parse the annotation file
    #

    set scalars [[$Model($_id,polyData) GetPointData] GetScalars]

    if { $scalars == "" } {
        set scalars ${_name}_scalars
        vtkFloatArray $scalars
        $scalars SetNumberOfComponents 1
        [$Model($_id,polyData) GetPointData] SetScalars $scalars
    }

    set fp [open $annotfile "r"]
    gets $fp nn

    $scalars SetNumberOfTuples $nn
    for {set n 0} {$n < $nn} {incr n} {
        gets $fp line
        set i [lindex $line 0]
        set c [lindex $line 1]

        if { ![info exists _labels($c,idx)] } {
            puts stderr "No label for color $c, index $i"
            break
        } 

        $scalars SetValue $i $_labels($c,idx)
    }

    close $fp

    MainModelsSetScalarVisibility $_id 1
    Render3D
}

itcl::body regions::query {} {
    global Model

    set terms ""

    foreach l $_ptlabels {

        regsub -all "(-|_)" $l " " t
        puts "$l go $t"

        # special case abbreviations 
        if { [lindex $t 0] == "G" } {
            if { [lsearch $terms "gyrus" ] == -1 } {
                lappend terms "gyrus"
            }
            set t [lreplace $t 0 0]
        }
        if { [lindex $t 0] == "S" } {
            if { [lsearch $terms "sulcus"] == -1 } {
                lappend terms "sulcus"
            }
            set t [lreplace $t 0 0]
        }

        # add any new terms
        foreach tt $t {
            if { [lsearch $terms $tt] == -1 } {
                lappend terms $tt
            }
        }
    }

    regsub -all " " $terms "+" terms

    switch $site {
        "google" {
            puts [catch "exec \"$browser\" http://www.google.com/search?q=$terms" res]
            puts $res
        }
        "pubmed" {
            catch "exec \"$browser\" http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=PubMed&term=$terms"
        }
        "mediator" {
            tk_messageBox -title "Slicer" -message "Mediator interface not yet implemented." -type ok -icon error
        }
    }
}


# calculate distance, but bail early if there's no chance it's closer
proc regions::dist {currmin x0 y0 z0 x1 y1 z1} {

    set dx [expr abs($x1 - $x0)]
    set dy [expr abs($y1 - $y0)]
    set dz [expr abs($z1 - $z0)]
    if { $dx >= $currmin || $dy > $currmin || $dz >= $currmin } {
        return [expr $dx + $dy + $dz]
    }
    set xx [expr ($x1 - $x0) * ($x1 - $x0)]
    set yy [expr ($y1 - $y0) * ($y1 - $y0)]
    set zz [expr ($z1 - $z0) * ($z1 - $z0)]
    return [expr sqrt( $xx + $yy +$zz ) ]
}

itcl::body regions::findptscalars {} {
    global Point Model

    $_labellistbox delete 0 end
    set _ptlabels ""
    set _ptscalars ""
    foreach id $Point(idList) {
        set xyz [Point($id,node) GetXYZ]

        set mapper [$Point($id,actor) GetMapper]
        if { [$mapper GetInput] != $Model($_id,polyData) } {
            puts "Point $id wasn't picked on $model"
        }

        set pts [$Model($_id,polyData) GetPoints]
        set cell [$Model($_id,polyData) GetCell $Point($id,cellId)]
        
        set npts [$cell GetNumberOfPoints]
        
        set pxyz [$pts GetPoint [$cell GetPointId 0]]
        set mindist [eval regions::dist 100000 $xyz $pxyz]

        for {set n 0} {$n < $npts} {incr n} {
            set pxyz [$pts GetPoint [$cell GetPointId $n]]
            set dist [eval regions::dist $mindist $xyz $pxyz]
            if { $dist < $mindist } {
                set mindist $dist
                set minpt [$cell GetPointId $n]
            }
        }
        set scalars [[$Model($_id,polyData) GetPointData] GetScalars]
        set s [$scalars GetValue $minpt]
        lappend _ptscalars $s
        lappend _ptlabels $_labels($s,name)
        $_labellistbox insert end "pt $id $_labels($s,name) ($s)"
    }
}


itcl::body regions::demo {} {
    global env

    # - read in lh.pial 
    # - make scalars visible

    # $this configure -labelfile "C:/pieper/bwh/freesurfer/Simple_surface_labels2002.txt"
    # $this configure -annotfile "C:/pieper/bwh/data/MGH-Siemens15-JJ/label/lh.aparc.pannot"
    # $this configure -model lh-pial

    $this configure -labelfile "$env(SLICER_HOME)/Modules/vtkFreeSurferReaders/tcl/Simple_surface_labels2002.txt"
    $this configure -annotfile "$env(SLICER_HOME)/../../slicerdata/freesurferInterop/label/lh.aparc.pannot"
    $this configure -model lh-pial

    $this apply

}

