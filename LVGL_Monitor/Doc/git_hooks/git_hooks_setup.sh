# git_hooks_setup.sh
# 在git bash下执行此脚本 命令: ./git_hooks_setup.sh
# !/bin/bash
# 获取 git 仓库的根目录
git_root=$(git rev-parse --show-toplevel 2>/dev/null)

# 检查是否在 git 仓库内
if [ $? -ne 0 ]; then
	echo "当前目录不是 git 仓库 请在 git 仓库中运行此脚本
	exit 1
fi

# 设置 hookspath 为仓库中的 .githooks 目录
git config core.hooksPath "$git_root/.githooks"

# 确保所有的钩子脚本可执行
chmod +x "$git_root/,githooks/"*

echo "git hooks 已经成功配置!"