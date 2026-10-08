# MP3 Tag Reader & Editor (C)

A command-line tool written in C that reads and edits ID3v2 metadata in MP3 files.

## Features

- **View tags**: shows the ID3 version and the main frames (Title, Artist, Album, Year, Genre, Comment).
- **Edit tags**: overwrites a chosen frame with a new value directly in the file.
- **Help menu**: lists all commands with `-help`.
- **Error handling**: reports a missing ID3 tag, invalid file type, bad arguments, unreadable files and values too long for a frame.

## Supported Options

| Option | Frame | Meaning |
|--------|-------|---------|
| `-t` | TIT2 | Title |
| `-a` | TPE1 | Artist |
| `-A` | TALB | Album |
| `-y` | TYER | Year |
| `-g` | TCON | Genre |
| `-c` | COMM | Comment |

## Build and Usage

```bash
gcc main.c view.c edit.c -o a.out

./a.out -help
./a.out -v song.mp3
./a.out -e -t song.mp3 "New Title"
```

## Project Structure

```
├── main.c      # Argument parsing and dispatch
├── view.c      # ID3 check, version, frame parsing, display
├── edit.c      # Argument validation, frame search, overwrite
├── header.h    # View structure and prototypes
├── edit.h      # Edit structure and prototypes
└── type.h      # Status and operation enums
```

## How It Works

An ID3v2 tag starts with a 10-byte header (`"ID3"`, version, flags, size), followed by frames. Each frame has a 10-byte header (ID, size, flags) and its content, whose first byte is the text encoding.

- **View**: verifies the signature, prints the version, then reads frames and prints the known ones, skipping frames too large for the buffer.
- **Edit**: finds the matching frame and overwrites it in place, padding the rest with zeros. The new value must fit within the existing frame size.

## Limitations and Future Work

- Supports ID3v2.3 text frames only; ID3v1, v2.2 and v2.4 are not handled yet.
- Only the first 6 frames are scanned.
- Planned: embedded image details, album art extraction, tag deletion and ID3v2.4 support.

## Reference

- [ID3v2.3 specification](http://id3.org/id3v2.3.0)
