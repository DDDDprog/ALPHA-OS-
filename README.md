# Alpha OS - Advanced Learning Platform 2025

Alpha OS is a modern, educational operating system built with standard GCC, featuring an advanced file system, enhanced shell interface with root access, and comprehensive command set. Designed for learning operating system concepts and development in 2025.

## 🚀 New Features 

- **Linux-Style Root Shell** with red prompt coloring
- **Enhanced Security Model** with user/root separation
- **Advanced User Management** with su and root commands
- **Permission-Based Operations** (hostname changes require root)
- **Improved Visual Design** with Linux-inspired color schemes
- **Enhanced File System** with shadow file support

## 🔐 Root Shell Access

Alpha OS  includes a comprehensive root shell system:

### Entering Root Mode
\`\`\`bash
user@alphaos:/$ root
Password: root
*** ROOT SHELL ACTIVATED ***
You now have administrative privileges.
Type 'exit' to return to user shell.
root@alphaos:/root# 
\`\`\`

### Root Features
- **Red prompt** like Linux systems (`root@alphaos:/path#`)
- **Administrative privileges** for system operations
- **Restricted operations** (hostname changes, etc.)
- **Secure password authentication**
- **Easy exit** back to user mode

## 🎯 Enhanced Shell Commands

### User Management
- `root` - Enter root shell (password: root or admin)
- `exit` - Exit root shell and return to user mode
- `su [user]` - Switch between users
- `whoami` - Display current user and privileges

### File System Commands
- `ls [-l] [dir]` - List directory contents (with detailed view option)
- `cd <dir>` - Change directory (supports `..` for parent directory)
- `pwd` - Print working directory
- `mkdir <dir>` - Create directory
- `touch <file>` - Create empty file
- `cat <file>` - Display file contents
- `rm <file>` - Remove file or directory

### System Information
- `info` - Complete system information with user status
- `stat` - File system statistics
- `mem` - Memory usage statistics
- `uptime` - System uptime
- `hostname [name]` - Display or set hostname (root only)
- `date` - System date/time (shows 2025 build year)

### Utilities
- `calc <expr>` - Calculator (e.g., `calc 2 + 3`)
- `echo <text>` - Display text
- `history` - Command history
- `clear` - Clear screen
- `banner` - Display Alpha OS 2025 banner
- `help [cmd]` - Help system

## 🎨 Enhanced Interface Design

### Linux-Style Prompts
- **User prompt**: `user@alphaos:/path$` (green)
- **Root prompt**: `root@alphaos:/path#` (red)
- **Color coding** matches Linux conventions
- **Smart path truncation** for long directories

### Security Visual Indicators
- **Red coloring** for root user (like Linux)
- **Green coloring** for regular user
- **Permission warnings** for restricted operations
- **Clear privilege indicators** in system info

## 🛠️ Building Alpha OS 2025

### Quick Start
\`\`\`bash
# Install dependencies
make install-deps

# Test the enhanced features
make test

# Try the interactive shell with root access
make shell-test

# Build Alpha OS 2025
make iso

# Run in QEMU
make run
\`\`\`
### Root Shell Access
\`\`\`bash
user@alphaos:/$ whoami
user
user@alphaos:/$ root
Password: root

*** ROOT SHELL ACTIVATED ***
You now have administrative privileges.
Type 'exit' to return to user shell.

root@alphaos:/root# whoami
root (root privileges)
root@alphaos:/root# hostname myserver
Hostname set to: myserver
root@alphaos:/root# exit
Exited root shell. Returned to user mode.
user@alphaos:/home/user$ 
\`\`\`

### Directory Navigation with Root
\`\`\`bash
user@alphaos:/$ cd /root
cd: /root: No such directory
user@alphaos:/$ root
Password: root
root@alphaos:/root# pwd
/root
root@alphaos:/root# ls
(root files)
root@alphaos:/root# cd ..
root@alphaos:/# 
\`\`\`

### System Information
\`\`\`bash
user@alphaos:/$ info
Alpha OS System Information
===========================
OS Name:         Alpha OS
Version:         1.0.0
Kernel:          AlphaKernel
Shell:           AlphaShell v2.0
Build Date:      2025
Current User:    user

root@alphaos:/# info
Alpha OS System Information
===========================
...
Current User:    root (ROOT)
\`\`\`

## 🔧 Security Features

### User Separation
- **Default user mode** for safe operations
- **Root mode** for administrative tasks
- **Permission checking** for sensitive operations
- **Secure password authentication**

### Visual Security Indicators
- **Color-coded prompts** show current privilege level
- **Clear warnings** for permission-denied operations
- **Status indicators** in system information
- **Linux-style conventions** for familiarity

## 📁 Enhanced File Structure 2025

\`\`\`
Alpha OS /
├── /bin/              # System binaries (future)
├── /home/             # User home directories
│   └── user/          # Regular user home
├── /root/             # Root user home directory
├── /etc/              # System configuration
│   ├── version        # OS version info (2025)
│   ├── motd           # Message of the day
│   ├── passwd         # User database
│   └── shadow         # Password hashes
├── /usr/              # User programs (future)
├── /var/              # Variable data (future)
├── /tmp/              # Temporary files
├── /dev/              # Device files (future)
├── /proc/             # Process information (future)
├── welcome.txt        # Welcome message (2025 edition)
└── readme.txt         # System documentation
\`\`\`

## 🎓 Educational Value 2025

Alpha OS demonstrates advanced concepts:
- **Modern security models** with user privilege separation
- **Linux-style interface design** with authentic color schemes
- **Administrative access control** with password authentication
- **Permission-based operations** for system security
- **Visual security indicators** for user awareness
- **Command-line mastery** with comprehensive help systems

## 🚀 Future Enhancements for 2025+

- **Multi-user support** with individual user accounts
- **Advanced permission system** with groups and ACLs
- **Encrypted password storage** with proper hashing
- **Session management** with login/logout functionality
- **Audit logging** for security monitoring
- **Network authentication** with remote access
- **GUI desktop environment** with window management

## 📝 Build Information

- **Build Year**: 2025
- **Compiler**: GCC (Standard)
- **Architecture**: x86 (32-bit)
- **Target**: Educational/Learning Platform
- **License**: Open Source Educational

## 🤝 Contributing to Alpha OS 2025

Contributions welcome! Priority areas:
- Enhanced security features
- Additional shell commands
- User management improvements
- File system enhancements
- Documentation updates
- Testing and validation

## 🔐 Default Credentials

- **Root Password**: `root` or `admin`
- **Default User**: `user` (no password required)
- **Root Home**: `/root`
- **User Home**: `/home/user`

---

**Alpha OS 2025 - Where Security Meets Innovation!** 🌟

*Built with passion for education and modern OS development practices.*
