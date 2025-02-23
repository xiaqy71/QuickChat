#!/bin/bash

ROOT_DIR="$(dirname "$(realpath "$0")")/.."
cd "$ROOT_DIR"

# 检查 tmux 是否已安装
if ! command -v tmux >/dev/null 2>&1; then
    echo "Error: tmux is not installed. Please install it with 'sudo apt install tmux'."
    exit 1
fi

# 创建新的 tmux 会话
SESSION_NAME="QuickChat"
tmux new-session -d -s "$SESSION_NAME"

# 窗格 0: 启动 Redis 并显示日志
echo "Starting Redis..."
tmux send-keys -t "$SESSION_NAME:0" "sudo service redis-server start" C-m
tmux send-keys -t "$SESSION_NAME:0" "until redis-cli ping | grep -q PONG; do echo 'Waiting for Redis...'; sleep 1; done" C-m
tmux send-keys -t "$SESSION_NAME:0" "echo 'Redis is ready.'; redis-cli" C-m

# 窗格 1: 启动 Mysql 并显示日志
tmux split-window -h -t "$SESSION_NAME:0"
echo "Starting VerifyServer..."
tmux send-keys -t "$SESSION_NAME:0.1" "sudo service mysql start" C-m


# 窗格 1: 启动 VerifyServer
tmux split-window -h -t "$SESSION_NAME:0.1"
echo "Starting VerifyServer..."
tmux send-keys -t "$SESSION_NAME:0.2" "source $ROOT_DIR/servers/VerifyServer/.venv/bin/activate" C-m
tmux send-keys -t "$SESSION_NAME:0.2" "cd $ROOT_DIR/servers/VerifyServer" C-m
tmux send-keys -t "$SESSION_NAME:0.2" "python3 verify_server.py" C-m

# 窗格 2: 启动 StatusServer
tmux split-window -v -t "$SESSION_NAME:0.2"
echo "Starting StatusServer..."
tmux send-keys -t "$SESSION_NAME:0.3" "cd $ROOT_DIR/build/servers/StatusServer" C-m
tmux send-keys -t "$SESSION_NAME:0.3" "./StatusServer" C-m

# 窗格 3: 启动 GateServer
tmux split-window -v -t "$SESSION_NAME:0.3"
echo "Starting GateServer..."
tmux send-keys -t "$SESSION_NAME:0.4" "cd $ROOT_DIR/build/servers/GateServer" C-m
tmux send-keys -t "$SESSION_NAME:0.4" "./GateServer" C-m

# 设置窗格布局
tmux select-layout -t "$SESSION_NAME:0" tiled

# 附加到 tmux 会话
tmux attach-session -t "$SESSION_NAME"