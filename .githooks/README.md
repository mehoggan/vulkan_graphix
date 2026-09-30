# Git hooks

`.githooks/pre-commit` runs the same checks used throughout the
`vulkan_earth` C++ modernization pass, against any staged files in
`vulkan_earth/src` (plus its headers in
`vulkan_earth/include/vulkan_earth`), `lib/`, or `bin/` (the tutorials),
before allowing
a commit - each area checked independently, with that area's own real
compiler flags:

1. build (incremental, from the repo root - catches cross-area
   breakage too, e.g. a `lib/` change that breaks `bin/`)
2. `-Wshadow` (no local/parameter may shadow a member)
3. self-assignment scan (`x = x;` / `a.b = a.b;`)
4. `.clang-tidy` naming/length rules, restricted to files actually
   staged in the commit (a pre-existing violation in some unrelated
   header you happen to include doesn't block you)
5. `clang-format` - every staged `.cpp`/`.h`/`.hpp` in `bin/`,
   `include/`, `lib/`, `tests/`, or `vulkan_earth/` (vendored
   `STBImage.h`/`STBTrueType.h` excepted) must already match
   `.clang-format`. It checks the staged content itself, so re-stage
   after fixing; the failure message prints the exact
   `clang-format -i ... && git add ...` command to run

The naming/length check needs a `compile_commands.json` to know the
real include paths. For an out-of-tree build (CLAUDE.md's
`mkdir build && cd build && bear -- ../configure && bear -- make -j8`),
the single `build/compile_commands.json` covers every area and is all
you need. The hook uses an area's own `compile_commands.json`
(`lib/`, `bin/`, `vulkan_earth/src/`) instead when one exists, and
skips the check with a message if it finds neither. For an in-tree
build, generate one per area with:

```sh
touch lib/*.cpp && cd lib && bear -- make -j4 && cd ..
touch bin/*/*.cpp && cd bin && bear -- make -j4 && cd ..
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

Missing tooling (`g++`, `clang-tidy`/`run-clang-tidy`, `bear`,
`clang-format`) degrades
the corresponding check to a skipped warning rather than blocking the
commit.
