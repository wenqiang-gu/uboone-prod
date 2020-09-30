for file in prodgenie_bnb_intrinsic_nue_overlay_run1 \
prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_CV_run3b \
prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_WireModThetaYZ_run3b \
prodgenie_bnb_nu_overlay_run1 \
prodgenie_bnb_nu_overlay_WCP_DetVar_CV_run3b \
prodgenie_bnb_nu_overlay_WCP_DetVar_WireModThetaYZ_run3b
do

    root -l -b -q check.C'("'"$file-chkout.root"'", "'wcpselection/T_eval'")'

    for knob in UBGenieFluxSmallUni \
    expskin_FluxUnisim \
    horncurrent_FluxUnisim \
    kminus_PrimaryHadronNormalization \
    kplus_PrimaryHadronFeynmanScaling \
    kzero_PrimaryHadronSanfordWang \
    nucleoninexsec_FluxUnisim \
    nucleonqexsec_FluxUnisim \
    nucleontotxsec_FluxUnisim \
    piminus_PrimaryHadronSWCentralSplineVariation \
    pioninexsec_FluxUnisim \
    pionqexsec_FluxUnisim \
    piontotxsec_FluxUnisim \
    piplus_PrimaryHadronSWCentralSplineVariation
    do
      root -l -b -q check.C'("'"$file/$knob.root"'", "'"$knob"'")'
    done

    

done


