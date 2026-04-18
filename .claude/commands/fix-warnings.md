# Fix Compiler Warnings

Scan the Brakeza3D debug build for compiler warnings and fix as many as possible.

$ARGUMENTS

## Step 1 — Build and capture warnings

Run the debug build and collect all warning lines:

```bash
cmake --build cmake-build-debug-mingw-brakezabundle 2>&1 | grep -E "warning:" | sort -u
```

If the build directory does not exist yet, configure it first:

```bash
cmake -B cmake-build-debug-mingw-brakezabundle -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug .
cmake --build cmake-build-debug-mingw-brakezabundle 2>&1 | grep -E "warning:" | sort -u
```

## Step 2 — Triage

Group warnings by file. For each unique file+line, classify:

| Class | Examples | Action |
|-------|----------|--------|
| **Fix** | unused variable, unused parameter, implicit conversion, signed/unsigned mismatch, missing return, shadow, deprecated API | Fix in source |
| **Suppress** | third-party header warnings, generated code, false positives | Add targeted `#pragma GCC diagnostic` or skip |
| **Skip** | warnings inside `sol/`, `imgui/`, `bullet/`, `assimp/` vendor headers | Do not modify vendor code |

## Step 3 — Fix loop

For every **Fix-class** warning, in order of frequency (most-repeated file first):

1. Read the affected file around the reported line (±10 lines of context).
2. Apply the minimal correct fix:
   - **Unused variable / parameter** → prefix with `(void)var;`, use `[[maybe_unused]]`, or remove if truly dead code.
   - **Signed/unsigned mismatch** → cast to the correct type (`static_cast<size_t>` etc.).
   - **Implicit narrowing / conversion** → add explicit `static_cast<>`.
   - **Missing `override`** → add `override` to virtual method declaration.
   - **Shadow** → rename the inner variable.
   - **Missing return in non-void** → add the return statement.
   - **Deprecated** → replace with the suggested non-deprecated alternative.
3. Do NOT reformat unrelated code, add comments, or change logic.
4. After fixing each file, note the warning count delta.

## Step 4 — Verify

Rebuild after all fixes:

```bash
cmake --build cmake-build-debug-mingw-brakezabundle 2>&1 | grep -E "warning:" | sort -u
```

Report:
- Warnings **before** vs **after**
- List of files modified
- Any warnings left unfixed and why (vendor code, requires design change, ambiguous fix)
