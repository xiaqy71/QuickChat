#!/bin/bash
SESSION_NAME="QuickChat"
if tmux has-session -t "$SESSION_NAME" 2>/dev/null; then
    tmux send-keys -t "$SESSION_NAME:0.2" C-c  # GateServer
    tmux send-keys -t "$SESSION_NAME:0.1" C-c  # VerifyServer
    tmux send-keys -t "$SESSION_NAME:0.3" C-c  # VerifyServer
    tmux send-keys -t "$SESSION_NAME:0.4" C-c  # VerifyServer
    # tmux send-keys -t "$SESSION_NAME:0.0" "sudo systemctl stop redis" C-m
    tmux kill-session -t "$SESSION_NAME"
    echo "All services stopped."
else
    echo "No tmux session found, stopping Redis directly..."
    # sudo systemctl stop redis
fi