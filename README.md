# jlogger
## Motivation
This is a simple tool to send multiline messages to journald. It's very much alike `logger --journald` or `systemd-cat`, but accepts multiline messages for transport.
This is useful, for example, when sending any kind of diagnostic reports to a centralized logging service.

## Building
`gcc jlogger.c -o jlogger -O3 -s -Wall -Wpedantic -Wextra -lsystemd`

## Usage
`some-program | jlogger <priority> <identifier>`