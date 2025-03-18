Changelog
=========

## [Tue Mar 18, 2025]
- Updated `config.txt` to include `trashPath` configuration.
- Recompiled `image-sorter` binary.
- Refactored `LoadFileTypeConfig` to process a single line instead of reading from a file.
- Introduced `LoadConfigData` to handle reading multiple configuration lines.
- Implemented `LoadTrashPathConfig` to extract the trash path from the config file.
- Modified `MoveDirectoryToTrash` to use the dynamically loaded trash path instead of a hardcoded value.
- Changed timestamp format in `MoveDirectoryToTrash` to use semicolons instead of colons for compatibility.
- Removed redundant `LoadFileTypeConfig` file-based function.

