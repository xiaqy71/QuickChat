#!/usr/bin/env python3

import os
import subprocess

def format_code(directory):
    """
    格式化指定目录下的所有 .cpp 和 .h 文件
    """
    # 查找所有 .cpp 和 .h 文件
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith((".cpp", ".h")):
                file_path = os.path.join(root, file)
                print(f"正在格式化: {file_path}")
                # 调用 clang-format 格式化文件
                subprocess.run(["clang-format", "-i", file_path], check=True)

    print("格式化完成！")

if __name__ == "__main__":
    # 设置要格式化的目录
    code_directory = "./server/GateServer"
    format_code(code_directory)