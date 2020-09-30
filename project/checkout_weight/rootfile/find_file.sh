
for dir in prodgenie_bnb_intrinsic_nue_overlay_run3-chkout \
prodgenie_bnb_intrinsic_nue_overlay_run1-chkout \
prodgenie_bnb_nu_overlay_run1-chkout \
prodgenie_bnb_intrinsic_nue_overlay_run1-chkout
do
  find /pnfs/uboone/scratch/users/wgu/WCP-001404/$dir -name "WCP*root"  > $dir.txt
done


for dir in prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_CV_run3b-chkout \
prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_WireModThetaYZ_run3b-chkout \
prodgenie_bnb_nu_overlay_WCP_DetVar_CV_run3b-chkout \
prodgenie_bnb_nu_overlay_WCP_DetVar_WireModThetaYZ_run3b-chkout-new
do
  find /pnfs/uboone/scratch/users/wgu/WCP-001407/$dir -name "WCP*root"  > $dir.txt
done

