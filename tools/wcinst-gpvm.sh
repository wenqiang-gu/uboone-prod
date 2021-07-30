# ./wcb configure --with-jsoncpp="$JSONCPP_FQ_DIR" --with-jsonnet-lib=/icarus/app/users/sergeym/wct/jsonnet-0.17.0 --with-jsonnet-include=/icarus/app/users/sergeym/wct/jsonnet-0.17.0/include --with-eigen-include="$EIGEN_DIR/include/eigen3/" --with-root="$ROOTSYS" --with-fftw="$FFTW_FQ_DIR" --with-fftw-include="$FFTW_INC" --with-fftw-lib="$FFTW_LIBRARY" --with-fftwthreads="$FFTW_FQ_DIR" --boost-includes="$BOOST_INC" --boost-libs="$BOOST_LIB" --boost-mt --with-hdf5="$HDF5_FQ_DIR" --with-h5cpp="$H5CPP_DIR" --with-spdlog-include="$SPDLOG_INC" --with-spdlog-lib="$SPDLOG_LIB" --with-tbb-include=/icarus/app/users/sergeym/wct/oneTBB/install/include --with-tbb-lib=/icarus/app/users/sergeym/wct/oneTBB/install/lib64  --prefix=/icarus/app/users/wgu/wire-cell-toolkit/bin

# ./wcb -p --notests install

# git submodule init; git submodule update

export PATH=/icarus/app/users/wgu/wire-cell-toolkit/bin/bin:$PATH
export LD_LIBRARY_PATH=/icarus/app/users/wgu/wire-cell-toolkit/bin/lib64:$LD_LIBRARY_PATH
export WIRECELL_PATH=/nashome/w/wgu/wire-cell-cfg:/icarus/app/users/wgu/wire-cell-toolkit/data:$WIRECELL_PATH
