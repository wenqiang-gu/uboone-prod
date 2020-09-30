for smp in prodgenie_bnb_intrinsic_nue_overlay_run1-chkout \
prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_CV_run3b-chkout \
prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_WireModThetaYZ_run3b-chkout \
prodgenie_bnb_nu_overlay_run1-chkout \
prodgenie_bnb_nu_overlay_WCP_DetVar_CV_run3b-chkout \
prodgenie_bnb_nu_overlay_WCP_DetVar_WireModThetaYZ_run3b-chkout
do
    # prune_checkout_trees $smp.txt $smp.root

    suffix="-chkout"
    basename=${smp%"$suffix"}

    # prune_weight_trees $smp.txt $smp.root UBGenieFluxSmallUni
    # prune_weight_trees $smp.txt $smp.root expskin_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root horncurrent_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root kminus_PrimaryHadronNormalization
    # prune_weight_trees $smp.txt $smp.root kplus_PrimaryHadronFeynmanScaling
    # prune_weight_trees $smp.txt $smp.root kzero_PrimaryHadronSanfordWang
    # prune_weight_trees $smp.txt $smp.root nucleoninexsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root nucleonqexsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root nucleontotxsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root piminus_PrimaryHadronSWCentralSplineVariation
    # prune_weight_trees $smp.txt $smp.root pioninexsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root pionqexsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root piontotxsec_FluxUnisim
    # prune_weight_trees $smp.txt $smp.root piplus_PrimaryHadronSWCentralSplineVariation

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
      mkdir -p $basename
      cd $basename
        echo prune_weight_trees ../$smp.txt $knob.root $knob
        prune_weight_trees ../$smp.txt $knob.root $knob
      cd ..
    done

done

