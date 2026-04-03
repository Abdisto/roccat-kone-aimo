{
  description = "roccat kone aimo virtual device";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "roccat-kone-aimo-virtual";
        version = "1.0.0";

        src = ./src;

        nativeBuildInputs = with pkgs; [
          pkg-config
          cmake
          meson
        ];

        buildInputs = with pkgs; [
          hidapi
        ];

        # Let cmake do its thing
        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
        ];
      };
    };
}
