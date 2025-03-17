Changelog:

Commit: cd55d6d8df04ec47c83ab40dce621d9e38eb1666
Date: Mon Mar 17, 2025
Author: Tri Pham <tri.pham121@icloud.com>

Changes:
- Modified the `PromptDirectoryDeletion` function to move the directory to trash instead of permanently deleting it.
- Added a new function `MoveDirectoryToTrash` to handle moving directories to the trash.
- Implemented timestamp-based folder renaming to avoid collisions when moving to trash.
- Removed the old `DeleteAllInDirectory` function, as the deletion now triggers a move to trash.
