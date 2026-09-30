#!/usr/bin/env python3
"""Servidor local, sem dependencias externas, para a interface da MiniRede."""

from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json
import subprocess
import threading
import webbrowser

ROOT = Path(__file__).resolve().parent.parent
WEB = ROOT / "gui"
BINARY = ROOT / "minirede"

DEMO = [
    "ADD_USER 1 ana Ana Costa",
    "ADD_USER 2 bruno Bruno Lima",
    "ADD_USER 3 carla Carla Souza",
    "ADD_USER 4 diego Diego Martins",
    "FOLLOW 1 2", "FOLLOW 1 3", "FOLLOW 2 3",
    "ADD_POST 101 2 1727557200 Finalmente entendi ponteiros. A chave foi desenhar cada ligação antes de escrever o código.",
    "ADD_POST 102 3 1727643600 Estruturas de dados ficam muito mais interessantes quando viram uma rede social de verdade.",
    "ADD_POST 103 1 1727730000 MiniRede ganhou uma interface! O motor continua sendo C++, agora com uma experiência visual completa.",
    "LIKE 1 102", "LIKE 2 102", "LIKE 3 103", "LIKE 4 103",
    "COMMENT 1 102 501 Projeto muito legal, Carla!",
]

history = list(DEMO)
lock = threading.Lock()

MUTATIONS = {"ADD_USER", "FOLLOW", "UNFOLLOW", "ADD_POST", "LIKE", "COMMENT", "REMOVE_POST", "REMOVE_RECENT_POST"}


def compile_binary():
    if not BINARY.exists() or BINARY.stat().st_mtime < max((ROOT / "minirede.cpp").stat().st_mtime, (ROOT / "minirede.h").stat().st_mtime):
        subprocess.run(["g++", "-std=c++11", "-O2", "minirede.cpp", "-o", "minirede"], cwd=ROOT, check=True)


def run(commands):
    payload = "\n".join(commands + ["END", ""])
    result = subprocess.run([str(BINARY)], input=payload, text=True, capture_output=True, cwd=ROOT, timeout=5)
    if result.returncode:
        raise RuntimeError(result.stderr or "Falha ao executar o motor C++")
    return result.stdout.splitlines()


def block(lines, start, end):
    try:
        begin = len(lines) - 1 - lines[::-1].index(start)
        finish = next(i for i in range(begin + 1, len(lines)) if lines[i] == end)
        return lines[begin + 1:finish]
    except (ValueError, StopIteration):
        return []


def parse_users(lines):
    users = []
    for line in lines:
        if line.startswith("USER "):
            parts = line.split(" ", 3)
            users.append({"id": int(parts[1]), "username": parts[2], "name": parts[3] if len(parts) > 3 else parts[2]})
    return users


def parse_posts(lines):
    posts = []
    for line in lines:
        if line.startswith("POST "):
            parts = line.split(" ", 5)
            posts.append({"id": int(parts[1]), "authorId": int(parts[2]), "timestamp": int(parts[3]), "likes": int(parts[4]), "text": parts[5] if len(parts) > 5 else ""})
    return posts


def snapshot(selected=None):
    users_lines = run(history + ["LIST_USERS"])
    posts_lines = run(history + ["LIST_RECENT_POSTS 100"])
    top_lines = run(history + ["TOP_POSTS 5"])
    users = parse_users(block(users_lines, "USERS_BEGIN", "USERS_END"))
    posts = parse_posts(block(posts_lines, "POSTS_BEGIN", "POSTS_END"))
    top = parse_posts(block(top_lines, "TOP_POSTS_BEGIN", "TOP_POSTS_END"))
    following = []
    if selected is not None:
        follow_lines = run(history + [f"LIST_FOLLOWING {int(selected)}"])
        following = [u["id"] for u in parse_users(block(follow_lines, "FOLLOWING_BEGIN", "FOLLOWING_END"))]
    return {"users": users, "posts": posts, "top": top, "following": following, "operations": len(history)}


def response_for(command):
    lines = run(history + [command])
    kind = command.split()[0].upper()
    pairs = {
        "LIST_USERS": ("USERS_BEGIN", "USERS_END"), "LIST_FOLLOWING": ("FOLLOWING_BEGIN", "FOLLOWING_END"),
        "LIST_RECENT_POSTS": ("POSTS_BEGIN", "POSTS_END"), "TOP_POSTS": ("TOP_POSTS_BEGIN", "TOP_POSTS_END"),
        "FEED": ("FEED_BEGIN", "FEED_END"), "LIST_COMMENTS": ("COMMENTS_BEGIN", "COMMENTS_END"),
        "SEARCH_POSTS": ("SEARCH_BEGIN", "SEARCH_END"), "GET_NOTIFICATIONS": ("NOTIFICATIONS_BEGIN", "NOTIFICATIONS_END"),
    }
    if kind in pairs:
        start, end = pairs[kind]
        content = block(lines, start, end)
        return [start] + content + [end]
    return lines[-1:] if lines else []


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB), **kwargs)

    def log_message(self, fmt, *args):
        pass

    def json(self, data, status=200):
        body = json.dumps(data, ensure_ascii=False).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path.startswith("/api/state"):
            try:
                query = self.path.partition("?")[2]
                selected = next((x.split("=", 1)[1] for x in query.split("&") if x.startswith("user=")), None)
                with lock:
                    self.json(snapshot(selected))
            except Exception as exc:
                self.json({"error": str(exc)}, 500)
            return
        super().do_GET()

    def do_POST(self):
        try:
            size = int(self.headers.get("Content-Length", 0))
            data = json.loads(self.rfile.read(size) or b"{}")
            if self.path == "/api/reset":
                with lock:
                    history[:] = list(DEMO)
                    state = snapshot(data.get("user"))
                self.json({"ok": True, "state": state})
                return
            if self.path != "/api/command":
                self.json({"error": "Rota não encontrada"}, 404)
                return
            command = " ".join(str(data.get("command", "")).strip().splitlines())
            if not command or len(command) > 600:
                self.json({"error": "Comando inválido"}, 400)
                return
            kind = command.split()[0].upper()
            with lock:
                output = response_for(command)
                ok = not any(line.startswith("ERROR") for line in output)
                if ok and (kind in MUTATIONS or kind == "GET_NOTIFICATIONS"):
                    history.append(command)
                state = snapshot(data.get("user"))
            self.json({"ok": ok, "output": output, "state": state})
        except Exception as exc:
            self.json({"error": str(exc)}, 500)


if __name__ == "__main__":
    compile_binary()
    address = ("127.0.0.1", 8080)
    print("MiniRede disponível em http://127.0.0.1:8080")
    threading.Timer(0.8, lambda: webbrowser.open("http://127.0.0.1:8080")).start()
    ThreadingHTTPServer(address, Handler).serve_forever()
