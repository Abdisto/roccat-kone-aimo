# roccat-kone-aimo
Backengineering done by me by monitoring /dev/hidraw. Using hidapi's test.c as starting block and letting AI complete the uinput stuff for linux. Still in progress.

This was done for a personal use-case. Be aware that despite this being a small code-base, that this project might impose a serious security risk. Use with discretion.

## Todo 
- add scroll wheel left and right flick buttons
- Do we need udev rules? Still don't know, works without

## Usage
roccat.nix
```
{ pkgs, inputs, ... }:

let
  roccatPkg = inputs.roccat.packages.${pkgs.stdenv.hostPlatform.system}.default;
in
{
  environment.systemPackages = [
    roccatPkg
  ];

  systemd.services.roccat-virtual = {
    description = "Roccat Virtual Device";
    wantedBy = [ "multi-user.target" ];

    serviceConfig = {
      ExecStart = "${roccatPkg}/bin/roccat_kone_aimo_hidraw";
      Restart = "always";
    };
  };
}

```
flake.nix
```
inputs.roccat.url = "github:Abdisto/roccat-kone-aimo"
```
