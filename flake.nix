{
  description = "LearnC-FastCGI";
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixpkgs-unstable;

  };

  outputs = { self, nixpkgs }: let
      forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" ];
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            buildInputs = with pkgs; [
              fcgi
              json_c
              nginx
              ccls
              gcc
            ];
            shellHook = ''
              # Nginx only want to read conf file with full path (idk why)
              alias serve='nginx -c "$(readlink -f ./nginx.conf)"'
            '';
          };
        });
    };
}
