# Testing uds_over_ip.cpp with socat

To test the `uds_over_ip.cpp` implementation using `socat`, follow these steps:

## Prerequisites
1. Ensure `socat` is installed on your system. You can install it using your package manager:
    - On Debian/Ubuntu: `sudo apt-get install socat`
    - On macOS (with Homebrew): `brew install socat`

2. Compile `uds_over_ip.cpp`:
    ```bash
    g++ -std=c++11 uds_over_ip.cpp -o uds_over_ip -pthread
    ```

## Steps to Test

1. Start a UNIX domain socket server using `socat`:
    ```bash
    socat -d -d UNIX-LISTEN:/tmp/uds_socket STDOUT
    ```

2. In another terminal, run the compiled `uds_over_ip` program and ensure it connects to the `/tmp/uds_socket` UNIX domain socket.

3. Use `socat` to simulate a client sending/receiving data to the server:
    ```bash
    socat STDIO TCP:localhost:8080
    ```

4. Observe the output in both the `uds_over_ip` program and the `socat` server terminal to verify the communication.

## Cleanup
After testing, remove the UNIX domain socket file:
