{ pkgs ? import <nixpkgs> {} }:

with pkgs;

stdenv.mkDerivation rec {
  name = "qcomicbook-${version}";
  version = "0.9.1";
  
  src = ./.;

  nativeBuildInputs = [
    cmake pkg-config
  ];

  buildInputs = with qt6; [
    qtbase qttools qt5compat qt6Packages.poppler
  ];

}
