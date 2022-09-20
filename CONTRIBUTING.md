# Contributing to this project

While the [README](./README.md) already contains much of the information needed to work in the project, this document will contain a few additional notes, in particular recommendations on how to setup your Git environment in order to be able to make your contributions to the project as good as possible

## Setting up Git

This project uses a few git hooks in order to make it possible to automatically review all submitted code for formatting and linting errors, before it even hits the CI (which mostly serves as a backup in case you fail to properly set this up, or if your environment fails to find some errors the CI does (for example if you have an old version of `clang-tidy`/`clang-format`, or even just lack the programs))

The git hooks can be installed into the `.git` folder with the following command, as executed from the top directory of this repository:

```
./scripts/setup_git_hooks.sh
```

(Note: this script requires a POSIX environment to execute)
