proc BIRNprocessXMLTemplateFile {infile resultsdir atlasdir subjname} {

    ######## read template 

    if { [ catch { set fid [ open $infile r ] } errmsg ] } {
    error "can't read xml file: $infile: $errmsg"
    }

    if { [ catch { set template [read $fid]; close $fid } ] } {
    error "error reading xml file: $infile: $errmsg"
    }

    ######## check for variables in template

    foreach svar {@SUBJECT@ @RESULT-END@ @RESULT-START@ @ATLASROOT@ @RESULTDIR@} {
    if { ! [ regexp $svar $template ] } { 
        error "no $svar in template: $infile"
    }
    }

    ######## substitute variables in template

    regsub -all "@SUBJECT@" $template $subjname tmptemplate 

    regsub -all "<@RESULT-START@.*@RESULT-END@>" [ \
        regsub -all "@ATLASROOT@" $tmptemplate $atlasdir \
    ] {} atlastemplate 


    regsub -all "<@RESULT-START@.*@RESULT-END@>" [ \
        regsub -all "@ATLASROOT@" $tmptemplate $resultsdir \
    ] {} worktemplate

    regsub -all "@RESULT-END@" [ \
    regsub -all  "@RESULT-START@" [ \
        regsub -all "@RESULTDIR@|@ATLASROOT@" $tmptemplate $resultsdir \
    ] {Volume} \
    ] {></Volume} resultstemplate


    ######## write output 

    set outfile1 [ file join $resultsdir $subjname "${subjname}.xml" ]
    set outfile2 [ file join $resultsdir $subjname "atlases.xml" ]
    set outfile3 [ file join $resultsdir $subjname "EMResult.xml" ]

    foreach {filename template} [ list $outfile1 $worktemplate $outfile2 \
        $atlastemplate $outfile3 $resultstemplate ] {

    if { [ catch { set fid [ open $filename  w ] } errmsg ] } { 
        error "can't open output xml file: $filename: $errmsg"
    }

    if { [ catch { puts -nonewline $fid $template; close $fid } errmsg ] } {
        error "failed writing output xml file: $filename: $errmsg"
    }
    }


    return [ list $outfile1 $outfile2 $outfile3 ] 
}

