
package require Iwidgets
#
# a toplevel window that displays cortical parcellation options
# - meant to be sourced into the slicer
#

if { [itcl::find class regions] == "" } {

    itcl::class regions {

        public variable model "" {}
        public variable annotfile "" {}
        public variable talfile "" {}
        public variable browser {/usr/bin/mozilla} {}
        public variable site "google" {}
        public variable afnidir "/home/ajoyner/afnifile/atlas/Release"
        public variable arrow "" {}
        public variable arrowout "" {}
        public variable tmpdir "" {}

        variable _Blabel ""

        variable _sites "google arrowsmith pubmed jneurosci ibvd mediator all"
        variable _terms ""

        variable _name ""
        variable _w ""
        variable _j ""
        variable _B ""
        variable _labellistbox ""
        variable _modelmenu ""
        variable _id ""

        variable _labels
        variable _mtx
        variable _ptscalars ""
        variable _ptlabels ""
        variable xyz ""
        constructor {args} {}
        destructor {}

        method apply {} {}
        method query {} {}
        method findptscalars {} {}
        method talairach {} {}
        method refresh {args} {}
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
    iwidgets::Labeledframe $_w.config -labeltext "Configuration" -labelpos nw
    pack $_w.config -fill both -expand true -padx 50

    set cs [$_w.config childsite]

    set _modelmenu [iwidgets::Optionmenu $cs.model]
    $_modelmenu configure -labeltext "Model:" -command "$this configure -model \[$_modelmenu get\]"
    refresh model
    pack $_modelmenu -expand true -fill x

    frame $cs.annot
    pack $cs.annot -expand true -fill x
    iwidgets::entryfield $cs.annot.eannot \
        -textvariable [itcl::scope annotfile] \
        -labeltext "Annotation File:"
    pack $cs.annot.eannot -side left -expand true -fill x
    button $cs.annot.bannot -text "Browse..." \
        -command "$this configure -annotfile \[tk_getOpenFile -filetypes { {{Annotations} {.annot}} {{All Files} {.*}} } -initialfile \[$this cget -annotfile\] \]"
    pack $cs.annot.bannot -side left

    frame $cs.tal
    pack $cs.tal -expand true -fill x
    iwidgets::entryfield $cs.tal.etal \
        -textvariable [itcl::scope talfile] \
        -labeltext "Talairach File:"
    pack $cs.tal.etal -side left -expand true -fill x
    button $cs.tal.btal -text "Browse..." \
        -command "$this configure -talfile \[tk_getOpenFile -filetypes { {{AFNI Talairach File} {.HEAD}} {{Freesurfer Talairach File} {.xfm}} } -initialfile \[$this cget -talfile\] \]"
    pack $cs.tal.btal -side left

    ::iwidgets::Labeledwidget::alignlabels $_modelmenu $cs.annot.eannot $cs.tal.etal 

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


    ::iwidgets::Entryfield $cs.terms -labeltext "Extra Terms:" -textvariable [::itcl::scope _terms]
    pack $cs.terms -fill both -expand true
    
    
    button $cs.update -text "Update" -command "$this findptscalars"
    pack $cs.update -side left
    
    ::iwidgets::Optionmenu $cs.site -labeltext "Site:" -command "$this configure -site \[$cs.site get\]"
    foreach s $_sites {
        $cs.site insert end $s
    }
    pack $cs.site -side left

    button $cs.query -text "Query" -command "$this query"
    pack $cs.query -side left

    #
    # try to determine browser automatically
    #
    set mozpaths { 
        "/usr/bin/mozilla" 
        "/usr/local/mozilla/bin/mozilla" 
        "/local/os/bin/mozilla" 
        "c:/Program Files/mozilla.org/Mozilla/mozilla.exe"
    } 
    foreach mozpath $mozpaths {
        if { [file exists $mozpath] } {
            $this configure -browser $mozpath
            break
        }
    }

    #
    # try to determine temp dir automatically
    #
    set tmpdirs { 
        "/var/tmp" 
        "/tmp" 
    }
    catch {lappend tmpdirs "$::env(USERPROFILE)/Local Settings/Temp"}
    lappend tmpdirs "c:/Temp"
    foreach tmpdir $tmpdirs {
        if { [file writable $tmpdir] } {
            $this configure -tmpdir $tmpdir
            break
        }
    }
    if { [$this cget -tmpdir] == "" } {
        puts stderr "can't find usable temp directory in $tmpdirs"
        error -1
    }

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

    if { [file ext $annotfile] != ".annot" } {
        DevErrorWindow "Can only handle annotation files (.annot) currently."
        return
    }

    #
    # parse the annotation file
    # - make a scalar field for the labels if needed
    # - set the colors for the model's lookuptable
    # - put the anatomical labels into a member array for access later
    #
    set scalaridx [[$Model($_id,polyData) GetPointData] SetActiveScalars "labels"] 

    if { $scalaridx == "-1" } {
        set scalars scalars_$_name
        catch "$scalars Delete"
        vtkIntArray $scalars
        $scalars SetName "labels"
        [$Model($_id,polyData) GetPointData] AddArray $scalars
        [$Model($_id,polyData) GetPointData] SetActiveScalars "labels"
    } else {
        set scalars [[$Model($_id,polyData) GetPointData] GetScalars $scalaridx]
    }

    set lut [Model($_id,mapper,viewRen) GetLookupTable]
    $lut SetRampToLinear

    set fssar fssar_$_name
    catch "$fssar Delete"
    vtkFSSurfaceAnnotationReader $fssar
    $fssar SetFileName $annotfile
    $fssar SetOutput $scalars
    $fssar SetColorTableOutput $lut
    $fssar ReadFSAnnotation

    array unset _labels
    array set _labels [$fssar GetColorTableNames]

    set ::Model(scalarVisibilityAuto) 0
    set entries [lsort -integer [array names _labels]]
    MainModelsSetScalarRange $_id [lindex $entries 0] [lindex $entries end]
    MainModelsSetScalarVisibility $_id 1
    Render3D
}

itcl::body regions::query {} {
    global Model

    set terms ""

    foreach l $_ptlabels {

        regsub -all "(-|_)" $l " " t

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


    #Create Pubmed query
    if { $_Blabel != "" } {
        set BrodSwitch [split [lindex $_Blabel 2]]
        puts [lindex $BrodSwitch 0]
        puts "$_Blabel"
        if { [lindex $_Blabel 2] == "*" } {
            set terms [lindex $_Blabel 0]
        } elseif { [lindex $BrodSwitch 0] == "Brodmann" } {
            set area [lindex $BrodSwitch 2]
            set term2 [lindex $_Blabel 0]
            if { $site == "pubmed" } {   
                set terms "\"BA $area\" NOT (barium OR (ba AND \"2+\")) OR (Brodmann AND $area) OR ($term2)"
            } elseif { $site == "arrowsmith" } {
                set termA "&quot BA $area &quot NOT (barium OR (ba AND &quot 2+ &quot)) OR (Brodmann AND $area) AND ($term2)"
            } elseif { $site == "jneurosci" } {
               set terms "\"BA $area\" OR (Brodmann AND $area) AND ($term2)"
            } else { 
                set terms [lindex $_Blabel 2]
                set termA [lindex $_Blabel 2]
            }
        }
    }
    regsub -all "{" $terms "" terms
    regsub -all "}" $terms "" terms
        
    # add the user's additional terms from the entry box   
    foreach t $_terms {
        set terms "$terms+$t"
    }

    regsub -all " " $terms "+" terms
    switch $site {
        "arrowsmith" {
            set query [open $arrow r]
            set queryout [open $arrowout w+]
            for {set m 0} {$m < 27} {incr m} {
                gets $query line
                puts $queryout $line
            }
            seek $queryout 0 current
            puts $queryout "<input type=\"hidden\" name=\"A-query\" value=\"$termA\" /> $termA \n" 
            gets $query line
            gets $query line
            while { ![eof $query] } {
                gets $query line
                puts $queryout $line
            }
            close $query
            close $queryout
            exec $browser -geometry 600x800 $arrowout 2> /dev/null &       
        }

        "google" {
            catch "exec \"$browser\" http://www.google.com/search?q=$terms &"
        }
        "pubmed" {
            catch "exec \"$browser\" http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=PubMed&term=$terms &"
        }
        "jneurosci" {
            catch "exec \"$browser\" http://www.jneurosci.org/cgi/search?volume=&firstpage=&sendit=Search&author1=&author2=&titleabstract=&fulltext=$terms &"
        }
        "ibvd" {
            catch "exec \"$browser\" http://www.cma.mgh.harvard.edu/ibvd/search.php?f_submission=true&f_structure\\\[\\\]=$terms &"
        }
        "all" {
            catch "exec \"$browser\" http://www.google.com/search?q=$terms &"
            catch "exec \"$browser\" http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=PubMed&term=$terms &"
            catch "exec \"$browser\" http://www.jneurosci.org/cgi/search?volume=&firstpage=&sendit=Search&author1=&author2=&titleabstract=&fulltext=$terms &"
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
        set minpt 0
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
        lappend _ptlabels $_labels($s)
        if { $mindist > 2} {
           $_labellistbox insert end "Point Not on Surface" 
        } else {
           $_labellistbox insert end "pt $id $_labels($s) ($s)" 
        }
        $this talairach
        if { $_Blabel != "" } {
            $_labellistbox insert end "pt $id $_Blabel mm"
        }
    }
}

itcl::body regions::talairach {} {
    global Point Model

    if { ![file exists $afnidir] } {
        set _Blabel ""
        return
    }

    #get coordinate from Slicer
    scan $xyz "%f %f %f" x0 y0 z0
        
    #Set Tournoux Talairach -> MNI Talairach conversion matrices
    set tf1(1,1) .9900;set tf1(1,2) 0;set tf1(1,3) 0;set tf1(2,1) 0;set tf1(2,2) .9688;set tf1(2,3) .046
    set tf1(3,1) 0;set tf1(3,2) -.0485;set tf1(3,3) .9189
    set tf2(1,1) .9900;set tf2(1,2) 0;set tf2(1,3) 0;set tf2(2,1) 0;set tf2(2,2) .9688;set tf2(2,3) .042
    set tf2(3,1) 0;set tf2(3,2) -.0485;set tf2(3,3) .8390
    #
    # parse the Talairach Transform
    #
    set switch [split $talfile .]
    #use AFNI talairach file for transformation
    if {[lindex $switch end] == "HEAD"} {
       set fp [open $talfile "r"]
       gets $fp line
       while {$line != "name  = WARP_DATA"} {
            gets $fp line
       }
       gets $fp line
   
       #import 12 Basic Linear Transformations from AFNI Talairach header
       for {set n 0} {$n < 12} {incr n} {
           gets $fp line
           scan $line "%f %f %f %f %f" _mtx(1,1,$n) _mtx(1,2,$n) _mtx(1,3,$n) _mtx(2,1,$n) _mtx(2,2,$n)
           gets $fp line
           scan $line "%f %f %f %f" _mtx(2,3,$n) _mtx(3,1,$n) _mtx(3,2,$n) _mtx(3,3,$n)
           gets $fp line
           gets $fp line
           scan $line "%f %f %f %f %f" _b1 _b2 _b3 bvec(1,$n) bvec(2,$n)
           gets $fp line
           scan $line "%f" bvec(3,$n)
           gets $fp line
       }
       close $fp
                 
       set x0 [expr $x0 * -1.]
       set y0 [expr $y0 * -1.]

       #test for which BLT to use
       if {$x0 <= 0 && $y0 < 0 && $z0 >= 0} { 
          set sw 0 }
       if {$x0 >  0 && $y0 < 0 && $z0 >= 0} { 
          set sw 1 }
       if {$x0 <= 0 && $y0 >= 0 && $y0 <= 23 && $z0 >= 0} {
          set sw 2 }
       if {$x0 >  0 && $y0 >= 0 && $y0 <= 23 && $z0 >= 0} { 
          set sw 3 }
       if {$x0 <= 0 && $y0 > 23 && $z0 >= 0} { 
          set sw 4 }
       if {$x0 >  0 && $y0 > 23 && $z0 >= 0} { 
          set sw 5 }
       if {$x0 <= 0 && $y0 < 0 && $z0 < 0} { 
          set sw 6 }
       if {$x0 >  0 && $y0 < 0 && $z0 < 0} { 
          set sw 7 }
       if {$x0 <= 0 && $y0 >= 0 && $y0 <= 23 && $z0 < 0} { 
          set sw 8 }
       if {$x0 >  0 && $y0 >= 0 && $y0 <= 23 && $z0 < 0} { 
          set sw 9 }
       if {$x0 <= 0 && $y0 > 23 && $z0 < 0} { 
          set sw 10 }
       if {$x0 >  0 && $y0 > 23 && $z0 < 0} { 
          set sw 11 }
       puts "sw $sw" 

       #RAS -> Talairach coordinate transformation
       set tal(1) [expr ($x0 * $_mtx(1,1,$sw)) + ($y0 * $_mtx(1,2,$sw)) + ($z0 * $_mtx(1,3,$sw))]     
       set tal(2) [expr ($x0 * $_mtx(2,1,$sw)) + ($y0 * $_mtx(2,2,$sw)) + ($z0 * $_mtx(2,3,$sw))]
       set tal(3) [expr ($x0 * $_mtx(3,1,$sw)) + ($y0 * $_mtx(3,2,$sw)) + ($z0 * $_mtx(3,3,$sw))]   
       #final step of conversion
       set tal(1) [expr ($tal(1) - $bvec(1,$sw))]
       set tal(2) [expr ($tal(2) - $bvec(2,$sw))]
       set tal(3) [expr ($tal(3) - $bvec(3,$sw))]
    
       set tal(1) [expr $tal(1) * -1.]
       set tal(2) [expr $tal(2) * -1.]
    
       #Use .xfm file for Talairach transformation
    } elseif { [lindex $switch end] == "xfm" } {
       array unset _mtx
       set fp [open $talfile "r"]
       gets $fp line
       while {$line != "Linear_Transform ="} {
           gets $fp line
       }
       gets $fp line
       scan $line "%f %f %f %f" _mtx(1,1) _mtx(1,2) _mtx(1,3) _mtx(1,4)
       gets $fp line
       scan $line "%f %f %f %f" _mtx(2,1) _mtx(2,2) _mtx(2,3) _mtx(2,4)
       gets $fp line
       scan $line "%f %f %f %f" _mtx(3,1) _mtx(3,2) _mtx(3,3) _mtx(3,4)

       close $fp
       set r0 1
       set tal(1) [expr ($x0 * $_mtx(1,1)) + ($y0 * $_mtx(1,2)) + ($z0 * $_mtx(1,3)) + ($r0 * $_mtx(1,4))]      
       set tal(2) [expr ($x0 * $_mtx(2,1)) + ($y0 * $_mtx(2,2)) + ($z0 * $_mtx(2,3)) + ($r0 * $_mtx(2,4))]
       set tal(3) [expr ($x0 * $_mtx(3,1)) + ($y0 * $_mtx(3,2)) + ($z0 * $_mtx(3,3)) + ($r0 * $_mtx(3,4))]
       puts "MNI T-Coord $tal(1) $tal(2) $tal(3)"
    }
    #Tournoux -> MNI conversion
    if {$tal(3) >= 0} {
       set mtal(1) [expr ($tal(1) * $tf1(1,1)) + ($tal(2) * $tf1(1,2)) + ($tal(3) * $tf1(1,3))]       
       set mtal(2) [expr ($tal(1) * $tf1(2,1)) + ($tal(2) * $tf1(2,2)) + ($tal(3) * $tf1(2,3))]
       set mtal(3) [expr ($tal(1) * $tf1(3,1)) + ($tal(2) * $tf1(3,2)) + ($tal(3) * $tf1(3,3))]   
    } else {
       set mtal(1) [expr ($tal(1) * $tf2(1,1)) + ($tal(2) * $tf2(1,2)) + ($tal(3) * $tf2(1,3))]       
       set mtal(2) [expr ($tal(1) * $tf2(2,1)) + ($tal(2) * $tf2(2,2)) + ($tal(3) * $tf2(2,3))]
       set mtal(3) [expr ($tal(1) * $tf2(3,1)) + ($tal(2) * $tf2(3,2)) + ($tal(3) * $tf2(3,3))] 
    } 
    set tal(1) [format "%3.0f" $tal(1)]
    set tal(2) [format "%3.0f" $tal(2)]
    set tal(3) [format "%3.0f" $tal(3)]
    puts "Talairached Coord $tal(1) $tal(2) $tal(3)"
    puts "MNI Coord $mtal(1) $mtal(2) $mtal(3)"
    #write out coordinate for Talairach Daemon query
    set outfile [open "$afnidir/TDpoints.txt" w+]
    seek $outfile 0 start
    puts $outfile "$tal(1) $tal(2) $tal(3)"
    close $outfile
    #Read in labels from Talairach Daemon
    set infile [open "$afnidir/TDresult.txt" r+]
    gets $infile line
    set line [split $line ,]
    if {[lindex $line 0] == "Record Number"} {
        gets $infile line 
        set line [split $line ,]}
    set _Blabel ""
    lappend _Blabel [lindex $line 6]
    lappend _Blabel [lindex $line 7]
    lappend _Blabel [lindex $line 8]
    lappend _Blabel [lindex $line 9]
    puts "$_Blabel"
    close $infile
    set outfile [open "/home/ajoyner/TDpoints.txt" w+]
    seek $outfile 0 start
}

itcl::body regions::refresh {args} {

    switch [lindex $args 0] {
        "model" {
            $_modelmenu delete 0 end
            foreach i $::Model(idList) {
                set name [Model($i,node) GetName]
                $_modelmenu insert end $name
                if { $model == "" } {
                    set model $name
                    set _id $i
                }
            }
        }
        "fiducials" {
        }
    }
}

itcl::body regions::demo {} {
    
    # - read in lh.pial 
    # - make scalars visible


    $this configure -arrow $::PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl/QueryA.html
    $this configure -arrowout [$this cget -tmpdir]/QueryAout.html

    set mydata c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw
    if { [file exists $mydata] } {
        $this configure -annotfile $mydata/label/rh.aparc.annot 
        $this configure -talfile $mydata/mri/transforms/talairach.xfm

    } else {
        $this configure -labelfile "/home/ajoyner/slicer/Simple_surface_labels2002.txt"
        #$this configure -annotfile "/home/ajoyner/slicer/MGH-Siemens15-JJ/label/lh.aparc.pannot"
        #$this configure -talfile "/home/ajoyner/slicer/MGH-Siemens15-JJ/mri/transforms/talairach1.xfm"
        $this configure -annotfile "/home/ajoyner/bert/bert/label/lh_aparc.pannot"
        $this configure -talfile "/home/ajoyner/bn1295/bn1295+tlrc.HEAD"
        $this configure -arrow "/home/ajoyner/regions/QueryA.html"
        $this configure -arrowout "/home/ajoyner/regions/QueryAout.html"
        $this configure -model lh-pial
    }

    $this apply
}
