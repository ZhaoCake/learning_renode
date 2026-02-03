{
  description = "Learning Renode Development Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          name = "riscv32-renode-env";

          buildInputs = with pkgs; [
            # 1. 仿真器
            renode

            # 2. 交叉编译工具链 (针对 RISC-V Embedded/Bare-metal)
            # 使用 embedded 版本以避免 glibc/stubs-ilp32 依赖问题
            pkgsCross.riscv32-embedded.buildPackages.gcc
            gdb # multiarch gdb
            
            # 3. 构建系统
            cmake
            ninja
            gnumake

            # 4. 辅助工具
            dtc      # Device Tree Compiler (以后如果玩 Zephyr 或 Linux 会用到)
            openocd  # 虽然是仿真，但有时脚本会用到相关的配置逻辑
            vim      # 或你喜欢的编辑器
            
            # 5. Python 环境 (用于 Robot Framework 测试和 TFLite 模型转换)
            (python3.withPackages (ps: with ps; [
              pip
              robotframework
              numpy
              pillow # 图像处理，如果做视觉AI
            ]))
          ];

          shellHook = ''
            echo "=========================================="
            echo "   Learning Renode Development Environment   "
            echo "=========================================="
            echo "Tools Check:"
            if command -v renode >/dev/null 2>&1; then
              echo "Renode: $(renode --version)"
            else
              echo "Renode: Not found (might be installing...)"
            fi
            
            if command -v riscv32-none-elf-gcc >/dev/null 2>&1; then
               echo "GCC:    $(riscv32-none-elf-gcc --version | head -n 1)"
            elif command -v riscv32-unknown-elf-gcc >/dev/null 2>&1; then
               echo "GCC:    $(riscv32-unknown-elf-gcc --version | head -n 1)"
            else
               echo "GCC:    Not found (check pkgsCross configuration)"
            fi
            echo ""
            export RENODE_SHOW_LOG=true
            # 设置通用的别名，方便 Makefile 使用
            if command -v riscv32-none-elf-gcc >/dev/null 2>&1; then
                export CC_PREFIX=riscv32-none-elf-
            elif command -v riscv32-unknown-elf-gcc >/dev/null 2>&1; then
                 export CC_PREFIX=riscv32-unknown-elf-
            fi
          '';
        };
      }
    );
}
