package require Iwidgets
package require http
package require tls
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
        public variable wlabel "Brodmann" {}
        public variable arrow "" {}
        public variable arrowout "" {}
        public variable javapath "" {}
        public variable tmpdir "" {}
        public variable fsurferlab "" {}
        public variable talgyrlab "" {} 
        public variable Brodmannlab "" {}
        public variable range "" {}
        public variable umlsfile "" {}
        public variable umlslabel0 "" {}
        public variable umlslabel1 "" {}
        public variable umlsid "" {}
        public variable which "" {}
        public variable ucsddata "" {}
        public variable regionspath "" {}
        public variable dir "" {}
        public variable talumls "" {}
        public variable fsumls "" {}

        variable _sites "arrowsmith pubmed jneurosci UMLS-Browser google ibvd mediator all "
        variable _wlabels "Brodmann Talairach Freesurfer" 
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
        variable _ptlabels ""
        variable _xyz ""
        constructor {args} {}
        destructor {}

        method apply {} {}
        method query {} {}
        method statistics {} {}
        method findptscalars {} {}
        method talairach {} {}
        method refresh {args} {}
        method umls {} {}
        method fdemo {} {}
        method demo {} {}
    }
}

itcl::body regions::constructor {args} {
    global Model

    set _name [namespace tail $this]
    set _w .$_name

    toplevel $_w
    wm title $_w "BIRN Query Atlas"

    # put the app name and logo at the bottom
    catch {
        set im [image create photo -file $::PACKAGE_DIR_BIRNDUP/../../../images/new-birn.ppm]
        pack [label $w.logo -image $im -bg white] -fill x -anchor s -side left
    }


    #
    # configuration panel
    #
    iwidgets::Labeledframe $_w.config -labeltext "Configuration" -labelpos nw
    pack $_w.config -fill both -expand true -padx 50

    set cs [$_w.config childsite]

    set _modelmenu [iwidgets::Optionmenu $cs.model]
    $_modelmenu configure -labeltext "Model:" -command "$this configure -model \[$_modelmenu get\]"
    refresh model
    foreach i $Model(idList) {
        set name [Model($i,node) GetName]
        $_modelmenu insert end $name
        if { $model == "" } {
            set model $name
            set _id $i
        }
    }
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
    
    radiobutton $cs.r1 -text "Raw Image (Use AFNI Talairach Header)" -variable which -value 1
    radiobutton $cs.r2 -text "Talaraiched Image (Do not use AFNI Talairach Header)" -variable which -value 2
    pack $cs.r1 $cs.r2
    
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

    ::iwidgets::Optionmenu $cs.wlabel -labeltext "Label Type:" -command "$this configure -wlabel \[$cs.wlabel get\]"
    foreach s $_wlabels {
        $cs.wlabel insert end $s
    }
    pack $cs.wlabel -side left

    button $cs.query -text "Query" -command "$this query"
    pack $cs.query -side left
    button $cs.stats -text "Get Statistics" -command "$this statistics"
    pack $cs.stats -side left
    button $cs.smart -text "SMART Atlas" -command "exec $browser http://imhotep.ucsd.edu:7873/haiyun/jnlp/atlas.jnlp &"
    pack $cs.smart -side bottom
    button $cs.connect -text "Connectivity Tool (BAMS)" -command "exec $browser http://brancusi.usc.edu/bkms/about.html &"
    pack $cs.connect -side left
    button $cs.braininfo -text "BrainInfo" -command "exec $browser http://braininfo.rprc.washington.edu/mainmenu.html &"
    pack $cs.braininfo -side left
    
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
    
    set scalars [[$Model($_id,polyData) GetPointData] GetArray "labels"] 
    if { $scalars == "" } {
        set scalars scalars_$_name
        catch "$scalars Delete"
        vtkIntArray $scalars
        $scalars SetName "labels"
        [$Model($_id,polyData) GetPointData] AddArray $scalars
        [$Model($_id,polyData) GetPointData] SetActiveScalars "labels"
    } 

    set lutid [expr 1 + [llength $::Lut(idList)]]
    lappend ::Lut(idList) $lutid
    set ::Lut($lutid,name) "Freesurfer"
    set lut Lut($lutid,lut)
    catch "$lut Delete"
    vtkLookupTable $lut

    set fssar fssar_$_name
    catch "$fssar Delete"
    vtkFSSurfaceAnnotationReader $fssar
    $fssar SetFileName $annotfile
    $fssar SetOutput $scalars
    $fssar SetColorTableOutput $lut
    set ret [$fssar ReadFSAnnotation]
    if { $ret == 6 } {
        set fstcldir [file normalize $::PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl]
        $fssar SetColorTableFileName $fstcldir/Simple_surface_labels2002.txt
        $fssar UseExternalColorTableFileOn
        set ret [$fssar ReadFSAnnotation]
        puts $ret
    }

    array set _labels [$fssar GetColorTableNames]
    if { ![info exists _labels(-1)] } {
        set _labels(-1) "unknown"
    }

    ModelsSetScalarsLut $_id $lutid "false"
    set ::Model(scalarVisibilityAuto) 0
    set entries [lsort -integer [array names _labels]]
    MainModelsSetScalarRange $_id [lindex $entries 0] [lindex $entries end]
    MainModelsSetScalarVisibility $_id 1
    Render3D
}

itcl::body regions::statistics {} {
    global Model
    set csvfile [open $dir/test.csv r+]
    set idfile [open $dir/idfile.txt w+]
    gets $csvfile line
    while { ![eof $csvfile] } {
        gets $csvfile line
        set ucid [split $line ,]
        puts $idfile [lindex $ucid 0]
    }
    close $csvfile
    close $idfile    
    
    exec $javapath/soap_client/run.sh $dir/idfile.txt UCSD00156805 & 
    after 5000
    set results [open $javapath/soap_client/results.txt r+]

    gets $results line
    gets $results line
    gets $results line
       
    gets $results line
       
    #list containing all stats variables
    set stats [split $line]

    close $results
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
    switch $wlabel {
           "Brodmann" {
           #Create Pubmed query
           if { $Brodmannlab != "" } {
              set BrodSwitch [split $Brodmannlab]
              if { [lindex $BrodSwitch 0] == "Brodmann" } {
                 set area [lindex $BrodSwitch 2]
                 if { $site == "pubmed" } {   
                     set terms "\"BA $area\" NOT (barium OR (ba AND \"2+\")) OR (Brodmann AND $area)"
                 } elseif { $site == "arrowsmith" } {
                     set terms "&quot BA $area &quot NOT (barium OR (ba AND &quot 2+ &quot)) OR (Brodmann AND $area)"
                 } elseif { $site == "jneurosci" } {
                     set terms "\"BA $area\" OR (Brodmann AND $area)"
                 } else { 
                     set terms $Brodmannlab
                   }
               } else {
                    set terms $Brodmannlab
               }            
             }
           }
            
           "Talairach" {
              set terms $talgyrlab 
              set umlsid $talumls
           }

           "Freesurfer" {
              set terms $fsurferlab
              set umlsid $fsumls
           }
           
           "UMLS" {
              set terms $umlsid
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
            puts $queryout "<input type=\"hidden\" name=\"A-query\" value=\"$terms\" /> $terms \n" 
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
        "UMLS-Browser" {
            set address http://ape.ucsd.edu/srb/cgi-bin/umls.cgi?id=$umlsid
            puts $address
            catch "exec \"$browser\" $address &"
        }
        "jneurosci" {
            catch "exec \"$browser\" http://www.jneurosci.org/cgi/search?volume=&firstpage=&sendit=Search&author1=&author2=&titleabstract=&fulltext=$terms &"
        }
        "ibvd" {
            catch "exec \"$browser\" http://www.cma.mgh.harvard.edu/ibvd/search.php?f_submission=true&f_free=$terms &"
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
    
    foreach id $Point(idList) {

        set _xyz [Point($id,node) GetXYZ]
        set ptinfo "pt $id"

        # if there's a surface registered, do the lookup on that
        if { $_id != "" } {
            set minpt 0
            set mapper [$Point($id,actor) GetMapper]
            if { [$mapper GetInput] != $Model($_id,polyData) } {
                    puts "Point $id wasn't picked on $model"
            }
            set pts [$Model($_id,polyData) GetPoints]
            set cell [$Model($_id,polyData) GetCell $Point($id,cellId)]
            set npts [$cell GetNumberOfPoints]
            set pxyz [$pts GetPoint [$cell GetPointId 0]]
            set mindist [eval regions::dist 100000 $_xyz $pxyz]
            for {set n 0} {$n < $npts} {incr n} {
                set pxyz [$pts GetPoint [$cell GetPointId $n]]
                set dist [eval regions::dist $mindist $_xyz $pxyz]
                if { $dist < $mindist } {
                    set mindist $dist
                    set minpt [$cell GetPointId $n]
                }
            }
            set scalars [[$Model($_id,polyData) GetPointData] GetScalars]
            set scalars [[$Model($_id,polyData) GetPointData] GetArray "labels"] 
            if { $scalars == "" } {
                DevErrorWindow "No labels loaded for Model [Model(0,node) GetName)]"
                return
            }
            set s [$scalars GetValue $minpt]
            lappend _ptlabels $_labels($s)
            set fsurferlab $_labels($s)
            set umlslabel0 $fsurferlab
            $this umls
            set fsumls $umlsid
            set ptinfo ""
        if { $mindist > 2} {
            set ptinfo "" 
        } elseif { $umlsid == "" } {
            set ptinfo "pt $id - Freesurfer - $fsurferlab ($s)"
        } else {
            set ptinfo "pt $id - Freesurfer - $fsurferlab ($s) - UMLS ID $umlsid"
        }
        }
        if { [Point($id,node) GetDescription] != "" } {
            regsub -all -- "_" [Point($id,node) GetDescription] " " lab
            set ptinfo "$ptinfo - $lab"
            lappend _ptlabels $lab
        }
        
        $_labellistbox insert end $ptinfo
        # put the label on two lines for the fiducial
        regsub -all -- "-" $ptinfo "\n        " ptinfo
        Point($id,node) SetName $ptinfo
       
        $this talairach
        
        set umlslabel0 $talgyrlab
        set umlsid ""
        $this umls
        set talumls $umlsid
        if { $talgyrlab != "" && $umlsid == "" } {
            $_labellistbox insert end "pt $id - Talairach - $talgyrlab"
        } elseif { $talgyrlab != "" && $umlsid != "" } {
            $_labellistbox insert end "pt $id - Talairach - $talgyrlab - Talairach UMLS ID $umlsid"
        }
        if { $Brodmannlab != ""} {
            $_labellistbox insert end "pt $id - Brodmann - $Brodmannlab - $range mm"
        } 
 
    }
 FiducialsUpdateMRML
}

itcl::body regions::umls {} {
    set umls [open $umlsfile r]
    gets $umls line
    set n 0
    while { ![eof $umls] } {
        set labelline [split $line ,]    
        set label($n,0) [lindex $labelline 1]
        set label($n,1) [lindex $labelline 2]
        set label($n,2) [lindex $labelline 3]
        set n [expr ($n + 1)]
        gets $umls line
        }
    close $umls
    for {set i 0} {$i < $n} {incr i} {
        if {$umlslabel0 == $label($i,0)} {
            set umlsid $label($i,1)
        }
    }
}

itcl::body regions::talairach {} {
    global Point Model
    global which
    if { ![file exists $talfile] } {
        puts stderr "no talairach file"
        return
    }
    #get coordinate from Slicer
    scan $_xyz "%f %f %f" x0 y0 z0
        
    #Set Tournoux Talairach -> MNI Talairach conversion matrices
    set tf1(1,1) .9900;set tf1(1,2) 0;set tf1(1,3) 0;set tf1(2,1) 0;set tf1(2,2) .9688;set tf1(2,3) .046
    set tf1(3,1) 0;set tf1(3,2) -.0485;set tf1(3,3) .9189
    set tf2(1,1) .9900;set tf2(1,2) 0;set tf2(1,3) 0;set tf2(2,1) 0;set tf2(2,2) .9688;set tf2(2,3) .042
    set tf2(3,1) 0;set tf2(3,2) -.0485;set tf2(3,3) .8390
    #
    # parse the Talairach Transform
    #
    if { $which == 1} {
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
       set tal(1) [expr round($tal(1))]
       set tal(2) [expr round($tal(2))]
       set tal(3) [expr round($tal(3))]
       #puts "Talairached Coord $tal(1) $tal(2) $tal(3)"
       #puts "MNI Coord $mtal(1) $mtal(2) $mtal(3)"
    } elseif { $which == 2 } {
       set tal(1) [expr round($x0)]
       set tal(2) [expr round($y0)]
       set tal(3) [expr round($z0)]
    }
    puts "$tal(1) $tal(2) $tal(3)"
    #open socket, send coordinate to Talairach Daemon query
    if {[catch {set sock [socket localhost 19000]}]} {
         # exec "$javapath/runtd.sh" &
         set cwd [pwd]
         cd $javapath
         set cmd "java -classpath build/classes RegionsServer 19000 database.dat database.txt"
         set ret [catch "exec $cmd &" res]
         cd $cwd
         puts $cmd
    }
    set count 0
    while {[catch {set sock [socket localhost 19000]}]} {
         after 500
         incr count
         puts -nonewline "." ; flush stdout
         if { $count > 10 } {
             puts "no response from talairach server"
             return
         }
    }    
    #set sock [socket localhost 19000]
    puts $sock "$tal(1) $tal(2) $tal(3)          "
    flush $sock

    set lab [gets $sock]
    close $sock
    set lab [split $lab ,]
    set talgyrlab [lindex $lab 2]
    set Brodmannlab [lindex $lab 4]
    set range [lindex $lab 5]
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

proc QueryAtlas_fdemo { {demodata c:/pieper/bwh/data/fbirn-phantom-staple/average7} } {
    
    set fstcldir [file normalize $::PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl]

    regions r

    r configure -arrow $fstcldir/QueryA.html
    r configure -arrowout [r cget -tmpdir]/QueryAout.html

    if { ![file exists $demodata] } {
        set demodata $::env(SLICER_HOME)/../data/fbirn-phantom-staple/average7
    }

    set modelid [vtkFreeSurferReadersLoadModel $demodata/surf/lh.pial]
    #r configure -annotfile $demodata/label/lh_aparc.annot 
    r configure -annotfile $demodata/label/lh.atlas2002_simple.annot 
    r configure -talfile $demodata/mri/transforms/talairach.xfm
    r configure -arrow "$fstcldir/QueryA.html"
    r configure -arrowout "$fstcldir/QueryAout.html"
    r configure -umlsfile "$fstcldir/label2UMLS.txt"
    r configure -javapath [file normalize "$fstcldir/../talairach"]
    r configure -model lh-pial
    r apply

    set lutid [MainLutsGetLutIDByName "InvGray"]
    foreach scalarfile [glob $demodata/../../fbirn-phantom-staple/stap*/*lh*.vtk] {
        ModelsAddScalars $scalarfile
        ModelsSetScalarsLut $modelid $lutid "true"
    }

    # 
    # create a Sum Of Activtations scalar field
    #
    # TODO there should be a self demo mode for slicer --demo with a menu of options...

    catch "fdemo_sum Delete"
    set ptdata [$::Model($modelid,polyData) GetPointData]
    set narrays [$ptdata GetNumberOfArrays]
    for {set i 0} {$i < $narrays} {incr i} {
        set arr [$ptdata GetArray $i]
        if { [$arr GetNumberOfComponents] == 1 &&
                [$arr GetName] != "labels" } {
            set arrtuples [$arr GetNumberOfTuples]
            if { [info command fdemo_sum] == "" } {
                vtkShortArray fdemo_sum
                fdemo_sum SetNumberOfTuples $arrtuples
                for {set tuple 0} {$tuple < $arrtuples} {incr tuple} {
                    fdemo_sum SetTuple1 $tuple 0
                }
            }
            if { $arrtuples != [fdemo_sum GetNumberOfTuples] } { 
                puts stderr "bad number of tuples for $arr"
                return
            }
            puts "adding scalars [$arr GetName]"
            for {set tuple 0} {$tuple < $arrtuples} {incr tuple} {
                set sum [fdemo_sum GetTuple1 $tuple]
                set newval [$arr GetTuple1 $tuple]
                fdemo_sum SetTuple1 $tuple [expr $sum + $newval]
            }
        }
    }
    $ptdata AddArray fdemo_sum
    fdemo_sum SetName "Sum Of Staple Activations"
    $ptdata SetActiveScalars [fdemo_sum GetName]

    # update the auto scalar range
    set ::Model(scalarVisibilityAuto) 1
    ModelsPropsApplyButNotToNew
}

proc QueryAtlas_mdemo { {demodata c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw} } {
    
    set fstcldir [file normalize $::PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl]

    regions r

    r configure -arrow $fstcldir/QueryA.html
    r configure -arrowout [r cget -tmpdir]/QueryAout.html

    if { ![file exists $demodata] } {
        set demodata $::env(SLICER_HOME)/../data/MGH-Siemens15-SP.1-uw
    }

    vtkFreeSurferReadersLoadVolume $demodata/mri/orig/COR-.info
    set asegid [vtkFreeSurferReadersLoadVolume $demodata/mri/aseg/COR-.info 1]
    MainSlicesSetVolumeAll Fore $asegid

    set ::Color(fileName) $fstcldir/ColorsFreesurfer.xml
    ColorsLoadApply
    MainColorsUpdateMRML

    r configure -talfile $demodata/mri/transforms/talairach.xfm
    r configure -arrow "$fstcldir/QueryA.html"
    r configure -arrowout "$fstcldir/QueryAout.html"
    r configure -umlsfile "$fstcldir/label2UMLS.txt"
    r configure -javapath [file normalize "$fstcldir/../talairach"]

    MainSlicesSetVisibilityAll 1
    RenderAll
}

itcl::body regions::demo {} {
    
    # - read in lh.pial 
    # - make scalars visible

    set fstcldir [file normalize $::PACKAGE_DIR_VTKFREESURFERREADERS/../../../tcl]

    $this configure -arrow $fstcldir/QueryA.html
    $this configure -arrowout [$this cget -tmpdir]/QueryAout.html

    set mydata c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw
    
    if { [file exists $mydata] } {
       vtkFreeSurferReadersLoadModel $mydata/surf/lh.pial
        $this configure -annotfile $mydata/label/lh.aparc.annot 
        $this configure -talfile $mydata/mri/transforms/talairach.xfm
        $this configure -arrow "$fstcldir/QueryA.html"
        $this configure -arrowout "$fstcldir/QueryAout.html"
        $this configure -umlsfile "$fstcldir/label2UMLS.txt"
        $this configure -javapath [file normalize "$fstcldir/../talairach"]
        $this configure -model lh-pial
    } else {
        set ucsddata /home/ajoyner/apps/slicer2.2-dev-linux-x86-2003-09-05
        set regionspath /Modules/vtkFreeSurferReaders
        $this configure -annotfile "$ucsddata/brain/label/lh.aparc.annot"
        $this configure -talfile "$ucsddata/brain/mri/transforms/MGHBrain+tlrc.HEAD"
        $this configure -arrow "$ucsddata/$regionspath/tcl/QueryA.html"
        $this configure -arrowout "$ucsddata/$regionspath/tcl/QueryAout.html"
        $this configure -umlsfile "$ucsddata/$regionspath/tcl/label2UMLS.txt"
        $this configure -javapath $ucsddata/$regionspath/java
        $this configure -dir $ucsddata
        $this configure -model lh-pial
    }
    $this apply
}
