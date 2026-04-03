{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    pkg-config
    cmake
    meson
  ];
  buildInputs = with pkgs; [
    hidapi
  ];
}
