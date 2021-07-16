// By Xiangpan

    int nbins = Lee_test->bins_newworld;
    TMatrixD matrix_pred = Lee_test->matrix_pred_newworld;
    TMatrixD matrix_syst = Lee_test->matrix_absolute_cov_newworld;    
    TMatrixD matrix_shape(nbins, nbins);
    TMatrixD matrix_mixed(nbins, nbins);
    TMatrixD matrix_norm(nbins, nbins);
    ///
    double N_T = 0;
    for(int idx=0; idx<nbins; idx++) N_T += matrix_pred(0, idx);
    ///
    double M_kl = 0;
    for(int i=0; i<nbins; i++) {
      for(int j=0; j<nbins; j++) {
	M_kl += matrix_syst(i,j);
      }
    }
    ///
    for(int i=0; i<nbins; i++) {
      for(int j=0; j<nbins; j++) {      
	double N_i = matrix_pred(0, i);
	double N_j = matrix_pred(0, j);
	double M_ij = matrix_syst(i,j);      
	double M_ik = 0; for(int k=0; k<nbins; k++) M_ik += matrix_syst(i,k);
	double M_kj = 0; for(int k=0; k<nbins; k++) M_kj += matrix_syst(k,j);
	matrix_shape(i,j) = M_ij - N_j*M_ik/N_T - N_i*M_kj/N_T + N_i*N_j*M_kl/N_T/N_T;
	matrix_mixed(i,j) = N_j*M_ik/N_T + N_i*M_kj/N_T - 2*N_i*N_j*M_kl/N_T/N_T;	
	matrix_norm(i,j) = N_i*N_j*M_kl/N_T/N_T;
      }
    }
