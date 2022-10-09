# The CI

This project is set-up so that every push or PR to the main repository results in running various checks using GitHub Actions.

These checks can be summarized to:

- Building the project using cmake and checking it works properly
- Building and running the unit tests associated with the project
- Linting the project with clang-format, clang-tidy, cppcheck along with many other minor checks (automatic json syntax checking, trailing whitespace checks and many others)
- Checking if the project can be packaged up using `--target package` with cmake

On all of:
- Windows
- MacOS
- Linux

(Note that while the MacOS CI probably mean making a port to it would be quite easy, this does not mean our project will automatically work, as it hasn't actually ever been tested in person on MacOS)
