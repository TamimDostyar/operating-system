# Light Project 4: File System in Userspace (FUSE)

## Project Goal
Build a custom userspace file system with FUSE that can be mounted and used with normal Linux commands.

## What I Will Build

- create
- read
- write
- delete
- mkdir
- ls

## Required Features

1. FUSE integration
- Mount and unmount file system.
- Implement required callback handlers.

2. Storage model
- Back data with one virtual disk file.
- Manage free space with a block bitmap.

3. Metadata model
- Implement inode-like structures.
- Implement directory entries.
- Track file sizes and block locations.

4. File and directory operations
- create/read/write/delete files.
- create directories and list contents.

5. Error handling
- Return correct Linux-style errors for invalid paths and operations.

## Suggested Structure

- fs/fuse_main.c
- fs/inode.c
- fs/bitmap.c
- fs/dir.c
- fs/block_io.c

## Testing Checklist

- Mount succeeds.
- mkdir and ls behave correctly.
- create/write/read/delete works for multiple files.
- Data remains correct across multiple operations.
- Free block bitmap updates correctly.

## OSTEP Connection

This project maps to inodes, directories, allocation bitmaps, and file-system layout.

## Deliverable

A mounted FUSE file system usable with common Linux tools.
