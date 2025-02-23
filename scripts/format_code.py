#!/usr/bin/env python3
import os
import subprocess
import glob

def format_files(directory):
    # 支持的文件扩展名
    extensions = ['*.cpp', '*.h']
    files_to_format = []

    # 遍历目录，收集所有 .cpp 和 .h 文件
    for ext in extensions:
        files_to_format.extend(glob.glob(os.path.join(directory, '**', ext), recursive=True))

    if not files_to_format:
        print("No files to format.")
        return

    # 使用 clang-format 格式化文件
    for file in files_to_format:
        print(f"Formatting {file}...")
        subprocess.run(['clang-format-12', '-i', file], check=True)

if __name__ == "__main__":
    # 从项目根目录开始，格式化 servers/ 下的代码
    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    servers_dir = os.path.join(project_root, 'servers')
    format_files(servers_dir)