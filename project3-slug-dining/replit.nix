{ pkgs }: {
  deps = [
    pkgs.lsb-release
    pkgs.llvmPackages_14.clangUseLLVM
    pkgs.gnumake
    pkgs.valgrind
  ];
}