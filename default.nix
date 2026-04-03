{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  pname = "roccat-kone-aimo-virtual";
  version = "1.0.0";

  src = ./src;

  nativeBuildInputs = with pkgs; [
    pkg-config
    gcc
  ];

  buildInputs = with pkgs; [
    hidapi
  ];

  # Nix expects a 'make install' target. 
  # If your Makefile doesn't have one, we can define the install phase here:
  installPhase = ''
    mkdir -p $out/bin
    cp roccat_virtual $out/bin/
  '';
}
