# jlogger
## What it is and why it was created?
This is a simple tool to send multiline messages to journald.

## Building
`gcc jlogger.c -o jlogger -O3 -s -Wall -Wpedantic -Wextra -lsystemd`

## Usage
`some-program | jlogger <priority> <identifier>`