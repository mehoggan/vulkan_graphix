# Git hooks

`.githooks/pre-commit` runs the same checks used throughout the
`vulkan_earth` C++ modernization pass, against any staged
`vulkan_earth/src` files, before allowing a commit:

1. build (incremental)
2. `-Wshadow` (no local/parameter may shadow a member)
3. self-assignment scan (`x = x;` / `a.b = a.b;`)
4. `.clang-tidy` naming/length rules

`.git/hooks/` is never tracked by git, so this repo's clone doesn't wire
itself up automatically. One-time setup after cloning:

```sh
ln -sf ../../.githooks/pre-commit .git/hooks/pre-commit
```

(This repo's own `.git/hooks/pre-commit` is already wired up this way
locally.) Skip a single commit's checks with `git commit --no-verify`.

Missing tooling (`g++`, `clang-tidy`/`run-clang-tidy`, `bear`) degrades
the corresponding check to a skipped warning rather than blocking the
commit.
