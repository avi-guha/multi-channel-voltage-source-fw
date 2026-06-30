{
  description = "ESP-IDF development shell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    esp-dev.url = "github:mirrexagon/nixpkgs-esp-dev";
# We let esp-dev use its own nixpkgs internally so it can find its older dependencies
  };

  outputs = {
    self,
    nixpkgs,
    esp-dev,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
      # We don't use the overlay globally here to avoid the Python conflict
      config.permittedInsecurePackages = [
        "python3.13-ecdsa-0.19.1"
        "python3.10-ecdsa-0.19.1" # Added 3.10 just in case
      ];
    };

    # Use the packages directly from the esp-dev flake's output
    # This keeps the ESP-IDF environment "sandboxed" with its own Python version
    esp-idf = esp-dev.packages.${system}.esp-idf-full;
  in {
    devShells.${system}.default = pkgs.mkShell {
      name = "esp-project";
      buildInputs = [
        esp-idf
      ];
      shellHook = ''
        alias esp32='idf.py set-target esp32'
        alias b='idf.py build'
        alias fm='idf.py -p /dev/ttyUSB0 flash && idf.py monitor'
        alias f='idf.py -p /dev/ttyUSB0 flash'
        alias m='idf.py monitor'
      '';
    };
  };
}
