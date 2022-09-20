#!/usr/bin/env bash

set -euo pipefail

trap_exit () {
    echo "A command run from this script failed !"
}

trap trap_exit ERR



# Redirect output to stderr.
exec 1>&2

if [ -e ./.git/hooks/pre-commit ] && [ ! -e ./.git/hooks/pre-commit-rtype-sha512sum-version ]
then
    echo "Oops ! Looks like you already have a pre-commit hook set up on this repository ! Sadly, we won't be able to make this work in that case unless you are willing to forego your existing pre-commit hook..."
    exit 1
fi

if [ -e ./.git/hooks/pre-commit-rtype-sha512sum-version ] && [ "$(<./.git/hooks/pre-commit sha512sum)" != "$(cat ./.git/hooks/pre-commit-rtype-sha512sum-version)" ]
then
    echo "Looks like ./.git/hooks/pre-commit-rtype-sha512sum-version contains a wrong value, did you modify ./.git/hooks/pre-commit youself ?"
    echo "If you really want to update ./.git/hooks/pre-commit, please rm both ./.git/hooks/pre-commit and ./.git/hooks/pre-commit-rtype-sha512sum-version before running this script again"
    exit 1
fi

if [ ! -e ./.git/hooks/pre-commit-rtype-sha512sum-version ] || [ "$(<./scripts/script_data/git-clang-format-pre-commit-hook sha512sum)" != "$(cat ./.git/hooks/pre-commit-rtype-sha512sum-version)" ]
then
    echo "Updating ./.git/hooks/pre-commit to contain the current contents of ./scripts/script_data/git-clang-format-pre-commit-hook"
    cp -i ./scripts/script_data/git-clang-format-pre-commit-hook ./.git/hooks/pre-commit
    <./.git/hooks/pre-commit sha512sum >./.git/hooks/pre-commit-rtype-sha512sum-version
fi
