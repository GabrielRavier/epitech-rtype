# The release workflow

Creating a release for this project is relatively simple:

- First, tag a given commit you want to make a release correspond for, using `git tag`, and push the tag to GitHub
- Second, go on GitHub's page for this project, and create a new release for it. This feature should normally be available to the right of the GitHub UI, associate it with the tag you have previously created, and give it a proper title and description
- Finally, package up the project (i.e. run `cmake --build build --target package` with CMake) and add the binary artifacts to the GitHub release (these should be `rtype-[version]-Linux.sh`, `rtype-[version]-Linux.tar.gz` and `rtype-[version]-Linux.tar.Z` files for Linux, and a `rtype-version-win64.exe` file for Windows, all found in the `build` folder)
