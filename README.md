# XSYNC

XSYNC is a lightweight, high-speed time synchronization tool written entirely in C. It leverages the power of [ZeroMQ](https://zeromq.org/) for networking and achieves a time offset of <=1ms between client and server. Designed for applications that demand precise and fast time sync, XSYNC prioritizes performance and simplicity.

## Features

- **Ultra-low latency**: Ensures a time offset of <=1ms between client and server.
- **Efficient networking**: Built on ZeroMQ for high-performance communication.
- **Lightweight**: Minimal dependencies and resource usage.
- **Open-source**: Fully written in C for maximum portability and transparency.

## Demo
https://github.com/user-attachments/assets/4b6089e9-8d42-4521-bc91-5b5ee709169f

## Installation

### Prebuilt Executables

Prebuilt executables are available for Linux x86 and Linux ARM architectures in this repository's [Releases](https://github.com/yourusername/XSYNC/releases) section.

1. Download the appropriate archive for your architecture.
2. Unzip the archive:
   ```bash
   unzip xsync-linux-x86.zip # Replace with the appropriate file
   ```
3. Make the files executable:
   ```bash
   chmod +x XSyncServer XSyncClient
   ```
4. Run the executable with `sudo` for it to work:
   ```bash
   sudo ./XSyncServer
   sudo ./XSyncClient
   ```
   
## How It Works

1. **Client-Server Architecture**: XSYNC operates on a simple client-server model where the server continuously broadcasts the current time.
2. **Spamming for Accuracy**: The client repeatedly requests time updates, ensuring high accuracy by minimizing drift.
3. **ZeroMQ Integration**: ZeroMQ enables fast and reliable message passing between the client and server.

## Performance

- Tested to achieve an offset of <=1ms in typical LAN environments.
- Optimized for minimal CPU and memory usage.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

