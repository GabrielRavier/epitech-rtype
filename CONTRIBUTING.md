# Contributing to this project

While the [README](./README.md) already contains much of the information needed to work in the project, this document contains a few additional notes, in particular recommendations on how to setup your Git environment in order to be able to make your contributions to the project as good as possible

## Setting up Git

This project uses a few git hooks in order to make it possible to automatically review all submitted code for formatting and linting errors, before it even hits the CI (which mostly serves as a backup in case you fail to properly set this up, or if your environment fails to find some errors the CI does (for example if you have an old version of `clang-tidy`/`clang-format`, or even just lack the programs))

The git hooks can be installed into the `.git` folder with the following command

```
pre-commit install --hook-type pre-commit --hook-type commit-msg --hook-type pre-push
```

(Note: this obviously requires that the pre-commit program be installed: under most environments, it can easily be installed using `pip install pre-commit` (although this itself requires pip...))

## Guidelines for contributions

It is highly recommended that you run git clang-format, clang-tidy and cppcheck, on your code before submitting it (the git hook you can set up above is made to do this automatically when you try to commit code)

You can do this manually, but it should also be noted that, should you have `pre-commit` installed, you can instead run the pre-commit checks without actually committing anything, by just running

```
pre-commit run
```
