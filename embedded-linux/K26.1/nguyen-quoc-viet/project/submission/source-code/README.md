# Chat Server & Client — P3 Project

Multi-user chat server using epoll for concurrent connection handling, with CLI client.

## Quick Start

```bash
make                      # Build both server and client
./build/chat-server       # Terminal 1: Run server (listens on port 5000)
./build/chat-client       # Terminal 2+: Run client(s)
```

## Project Structure

```
.
├── src/
│   ├── server/            # Server implementation
│   │   ├── main.c         # epoll event loop, client management
│   │   ├── auth.c         # User authentication, password hashing
│   │   ├── broadcast.c    # Message broadcast
│   │   └── history.c      # Message persistence & history
│   └── client/            # Client CLI
│       └── main.c         # Interactive chat client
├── inc/                   # Header files
│   └── chat.h             # Shared definitions
├── build/                 # Build artifacts (git ignored)
│   ├── chat-server        # Server executable
│   ├── chat-client        # Client executable
│   └── obj/               # Object files
├── test/                  # Test directory
├── doc/                   # Documentation directory
├── Makefile               # Build system
└── README.md              # This file
```

## Features

### Server
- **Single-threaded**: Epoll-based event loop (NOT thread-per-client)
- **Non-blocking I/O**: Proper EAGAIN/EWOULDBLOCK handling
- **Authentication**: SHA256 password hashing with salt
- **Persistence**: Message history logging with file locking
- **Concurrent**: Supports 100+ simultaneous clients
- **Protocol**: TEXT-based (LOGIN, MSG, USERLIST, LOGOUT, HISTORY, REGISTER)

### Client
- **Interactive CLI**: Real-time message input/output
- **Login/Register**: User account management
- **Commands**: `/who`, `/help`, `/quit`
- **Message History**: Displays previous messages on join
- **Non-blocking**: Handles server messages while typing

## Building

```bash
make              # Build
make clean        # Remove build artifacts
make rebuild      # Clean and build
make help         # Show targets
```

## Client Usage

```
╔═══════════════════════════════════════╗
║     DevLinux Chat Client v1.0         ║
╚═══════════════════════════════════════╝
Server: 127.0.0.1:5000

Login or Register? [login/register]: login
Username: alice
Password: mypass123
[✓] Login successful!

[Connected as alice]
┌─ Available Commands ─────────────────────────────┐
│ /who              - List online users            │
│ /help             - Show this help               │
│ /quit             - Exit chat                    │
│ (other text)      - Send message to all users    │
└──────────────────────────────────────────────────┘

> hello everyone
[alice] alice: hello everyone
[bob] bob: hi there!
> /who
[users] Online: alice,bob,charlie
> /quit
[!] Logged out.

Goodbye!
```

## Testing

Test results documented in parent directory's `test_report.md`.

All 14 tests pass:
- Server startup ✅
- Authentication ✅
- Message broadcast ✅
- Concurrent clients ✅
- Graceful disconnect ✅
- No memory leaks ✅

## Architecture

### Message Flow

```
Client → TCP socket → epoll_wait()
                ↓
         route_message()
         ├── LOGIN → authenticate_user()
         ├── MSG → broadcast_message()
         ├── USERLIST → handle_userlist()
         ├── LOGOUT → close_client()
         └── HISTORY → send_message_history()
                ↓
         output_buffer → write() → Client
```

### File Locking

- **accounts.db**: LOCK_SH (read), LOCK_EX (write)
- **messages.log**: LOCK_EX (append)

## Requirements

- GCC compiler
- OpenSSL library (libssl-dev)
- Linux with epoll support

## Compilation Flags

```bash
-Wall -Wextra -std=c99 -lcrypto
```

## Performance

- Binary size: ~27KB
- Code: 551 lines
- Supports 100 concurrent connections
- Non-blocking operation
