# Git hooks

`.githooks/pre-commit` runs the same checks used throughout the
`vulkan_earth` C++ modernization pass, against any staged files in
`vulkan_earth/src`, `lib/`, or `bin/` (the tutorials), before allowing
a commit - each area checked independently, with that area's own real
compiler flags:

1. build (incremental, from the repo root - catches cross-area
   breakage too, e.g. a `lib/` change that breaks `bin/`)
2. `-Wshadow` (no local/parameter may shadow a member)
3. self-assignment scan (`x = x;` / `a.b = a.b;`)
4. `.clang-tidy` naming/length rules, restricted to files actually
   staged in the commit (a pre-existing violation in some unrelated
   header you happen to include doesn't block you)

The naming/length check needs a `compile_commands.json` in the area's
own directory (`lib/compile_commands.json`, `bin/compile_commands.json`,
`vulkan_earth/src/compile_commands.json`) to know the real include
paths; it's skipped with a message if that's missing. Generate one
per area with:

```sh
touch lib/*.cpp && cd lib && bear -- make -j4 && cd ..
touch bin/*.cpp && cd bin && bear -- make -j4 && cd ..
touch vulkan_earth/src/*.cpp && cd vulkan_earth/src && bear -- make -j4 && cd ../..
```

(`touch` first so `make` actually recompiles everything for `bear` to
capture - an up-to-date build has nothing left to intercept. Don't use
`make -B`/`--always-make` here instead: it also forces the
`configure`/`Makefile` regeneration rules, which can fail under bear's
process interception in some environments.)

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
