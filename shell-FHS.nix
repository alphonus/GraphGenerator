{ pkgs ? import <nixpkgs> {} }:
(pkgs.buildFHSUserEnv {
  name = "graphgen";
  targetPkgs = pkgs: (with pkgs; [
    python37
    #python37Packages.pip
    #python37Packages.virtualenv
    cudaPackages.cudatoolkit
    #gcc
  #stdenv.cc.cc.lib
  #libgccjit
  #glibc glibc.dev
    #libcxx libcxx.dev
    octave
    unzip


  ]);
  multiPkgs = pkgs: (with pkgs; [
  #stdenv.cc.cc.lib
  #libgccjit
  #clang
  gcc
  ]
  );

  runScript = pkgs.writeScript "init.sh" ''
  export CUDA_HOME=${pkgs.cudaPackages.cudatoolkit}
  export TMPDIR=/tmp
  export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python
  exec bash
  '';
#export CCFLAGS="-I/usr/include"
#export LDFLAGS="-L/usr/lib -L${pkgs.linuxPackages.nvidia_x11}/lib"
}
).env

