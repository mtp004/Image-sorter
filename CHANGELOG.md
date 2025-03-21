# Changelog

## [1.0.0] - 2025-03-21

### Added
- New modular architecture with separate files:
  - `main.c`: Core program functionality
  - `main.h`: Header file with function prototypes
  - `config.c`: Configuration loading logic
  - `config.h`: Configuration declarations and constants
- Configuration file support for:
  - Customizable file types to be sorted
  - Configurable trash bin path

### Changed
- Renamed main source file from `image_sorter.c` to `main.c` in Makefile
- Restructured program flow with modular functions
- Enhanced user interface with clearer prompts
- Improved directory processing with better error handling

### Fixed
- Added validation for configuration file reading
- Improved error messaging throughout the application