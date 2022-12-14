# Contributing to this project

While the [README](./README.md) already contains much of the information needed to work in the project, this document contains a few additional notes, in particular recommendations on how to setup your Git environment in order to be able to make your contributions to the project as good as possible

## Setting up Git

This project uses a few git hooks in order to make it possible to automatically review all submitted code for formatting and linting errors, before it even hits the CI (which mostly serves as a backup in case you fail to properly set this up, or if your environment fails to find some errors the CI does (for example if you have an old version of `clang-tidy`/`clang-format`, or even just lack the programs))

The git hooks can be installed into the `.git` folder with the following command

```
pre-commit install --hook-type pre-commit --hook-type commit-msg --hook-type pre-push
```

(Note: this obviously requires that the pre-commit program be installed: under most environments, it can easily be installed using `pip install pre-commit` (although this itself requires pip))

## Guidelines for contributions and norm

The norm for code contributions to this project is checked by several tools such as clang-format, clang-tidy and cppcheck.

It is highly recommended that you run these tools on your code before submitting it (the git hook you can set up above is made to do this automatically when you try to commit code)

You can do this manually, but it should also be noted that, should you have `pre-commit` installed, you can instead run the pre-commit checks without actually committing anything, by just running

```
pre-commit run
```

It should be noted that alongside clang-format, clang-tidy and cppcheck, a few other minor checks (like trailing whitespace at the end of lines/files, or extremely large files) and a check on the commit message itself (mandating a format resembling `feat: some new thing` or `fix: an annoying bug` as the first line of the commit messages).

Of course, one should also always make sure their code compiles with the procedure specified in the [INSTALL.md](./INSTALL.md) file before contributing it.

### Branch management

When directly contributing to this repository, it is also recommended that you never directly commit to `main`, unless some sort of emergency requires it.

Instead, create separate branches for each new feature or fix you want to add to the project and create a pull request based on that branch before merging it, to allow other contributors to review the changes if needed.

Branches pushed onto the main repository should have names that correspond to the commit convention, i.e. with names like `feat/some-new-feature` or `fix/some-annoying-bug`.
