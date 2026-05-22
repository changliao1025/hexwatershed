# HexWatershed — Code Quality Improvement Plan

> Generated: 2026-05-19
> Analyst: Roo (Architect mode)
> Status: **Pending implementation**

---

## Files Examined

| File | Lines | Key observations |
|---|---|---|
| `src/main.cpp` | 97 | Entry point, deeply-nested `if` chain for error handling |
| `src/global.h` / `src/global.cpp` | 142 / 127 | Dozens of unit-conversion constants from an unrelated eco3d model |
| `src/hexagon.h` | 161 | 40+ public data members, all `int` flags, magic sentinel values |
| `src/parameter.h` | 86 | All flags stored as `int`, `using namespace std` in header |
| `src/domain/domain.h` | 147 | 30+ `std::string` filename members, `using namespace` in header |
| `src/domain/domain_read.cpp` | 345 | Repeated `HasMember`/`GetXxx` pattern ~20 times |
| `src/compset/compset.h` | 161 | `using namespace` in header, large flat member list |
| `src/compset/compset_direction.cpp` | 718 | Raw iterator loops, many local variable copies of flags |
| `src/compset/compset_depression.cpp` | 1236 | Largest file; duplicated switch-case blocks per mesh type |
| `src/compset/compset_run.cpp` | 649 | O(n²) flow-accumulation while-loop |
| `src/compset/compset_stream.cpp` | 442 | Raw iterator loops, unused `error_code` variables |

---

## Issues Found

### 1. Namespace pollution in headers
Every header (`global.h`, `hexagon.h`, `parameter.h`, `domain.h`, `compset.h`, `system.h`) contains
`using namespace std;` and/or `using namespace rapidjson;`. This leaks names into every translation
unit that includes them — a well-known C++ anti-pattern that can cause silent name collisions.

**Fix:** Remove all `using namespace` directives from headers; use explicit `std::`, `rapidjson::`,
`jsonmodel::` prefixes everywhere.

---

### 2. Wrong `@file` Doxygen tags
`compset_direction.cpp`, `compset_run.cpp`, `compset_stream.cpp`, `compset_export.cpp`, and
`compset_initialize.cpp` all have `@file domain.cpp`. They will generate incorrect Doxygen output.

**Fix:** Update each `@file` tag to match the actual filename.

---

### 3. Bloated `global.h` / `global.cpp`
Constants like `dTimestep_eco3d`, `meter_2_foot`, `joule_2_langley`, and dozens of others belong to
a different model (eco3d). They add noise and compilation overhead to every file that includes
`global.h`.

**Fix:** Remove or move eco3d-specific constants to a separate header (e.g., `eco3d_constants.h`)
that is not included by default. Keep only watershed-relevant constants in `global.h`.

---

### 4. `int` used for boolean flags
`hexagon.h` has ~20 members named `iFlag_*` that are semantically boolean but typed as `int`.
Same pattern in `parameter.h`. This wastes memory (4× per flag) and obscures intent.

**Fix:** Change all `iFlag_*` members to `bool`. Update all assignment sites (`= 0` → `= false`,
`= 1` → `= true`) and all comparisons (`== 1` → `== true` or just the bare variable).

---

### 5. `int` error-code convention is fragile
Every function returns `int error_code = 1` and the caller checks `!= 0`. There is no type safety —
a caller can silently ignore the return value.

**Fix:** Introduce `enum class ErrorCode { Success = 1, Failure = 0 };` (or simply use `bool`).
Add `[[nodiscard]]` to every function that returns an error code so the compiler warns when the
return is discarded.

---

### 6. Repeated JSON-parsing boilerplate
`domain_retrieve_user_input()` repeats the pattern:
```cpp
sKey = "someKey";
if (pConfigDoc.HasMember(sKey.c_str()))
    cCompset.cParameter.someField = pConfigDoc[sKey.c_str()].GetXxx();
```
approximately 20 times.

**Fix:** Extract a helper template:
```cpp
template <typename T>
T readJsonField(const rapidjson::Document& doc, const std::string& key, T defaultValue);
```
Specialise for `int`, `float`, `std::string`. This reduces `domain_retrieve_user_input()` from
~150 lines to ~30.

---

### 7. Raw iterator loops where range-`for` suffices
Files like `compset_direction.cpp`, `compset_depression.cpp`, and `compset_stream.cpp` use
`std::vector<T>::iterator` loops that can be replaced with range-based `for` or standard algorithms
(`std::find_if`, `std::min_element`, `std::copy_if`).

**Fix:** Replace all `for (iIterator = v.begin(); iIterator != v.end(); iIterator++)` patterns with
`for (const auto& item : v)` or the appropriate `<algorithm>` call.

---

### 8. Missing `[[nodiscard]]`
All functions returning `int error_code` lack `[[nodiscard]]`. Silent discard of error codes is a
common source of bugs.

**Fix:** Add `[[nodiscard]]` to every function declaration in `domain.h`, `compset.h`, and all
other headers where the return value signals success/failure.

---

### 9. Flat, oversized class interfaces
`domain.h` has 30+ `std::string` filename members as direct public fields. `compset.h` is similar.

**Fix:** Group related members into nested structs:
```cpp
struct FilePaths {
    std::string mesh_info;
    std::string flowline_info;
    std::string elevation_polygon;
    // ...
};
struct RunFlags {
    bool global;
    bool multiple_outlet;
    bool flowline;
    // ...
};
```
This makes the class interface self-documenting and reduces cognitive load.

---

### 10. Magic numbers
Values like `0.1` and `0.001` in `compset_stream.cpp:97`, `-9999.0` in `hexagon.cpp:64`, and
`1 != file_test(...)` in `domain.cpp:41` should be named constants.

**Fix:** Define constants in an appropriate header:
```cpp
constexpr float kMissingElevation    = -9999.0f;
constexpr float kBreachElevationBump = 0.1f;
constexpr float kBreachRelativeBump  = 0.001f;
constexpr int   kFileExists          = 1;
```

---

### 11. Missing `const`-correctness
Query/read-only methods such as `compset_find_index_by_cell_id`, `compset_obtain_stream`, and
`compset_obtain_boundary` are not marked `const`.

**Fix:** Add `const` qualifier to all member functions that do not modify `*this`.

---

### 12. O(n²) flow accumulation algorithm
`compset_calculate_flow_accumulation()` uses a `while (lFlag_total != size)` outer loop that
rescans the entire cell vector each pass. For large meshes this is O(n²).

**Fix:** Replace with Kahn's BFS topological sort:
1. Build an in-degree map (number of upstream cells per cell).
2. Seed a queue with all cells that have zero upstream cells.
3. Process each cell: accumulate area, decrement downstream cell's in-degree, enqueue when it
   reaches zero.
This runs in O(n) time and O(n) space.

---

### 13. No automated tests
There is no `tests/` directory. Core algorithms (depression filling, flow direction, flow
accumulation) have no regression coverage.

**Fix:** Add a `tests/` directory with a CMake target. Use **Catch2** (header-only, easy to
integrate) or **GoogleTest**. Minimum test cases:
- Depression filling: flat DEM, single pit, nested pits.
- Flow direction: known slope, flat tie-breaking.
- Flow accumulation: linear chain, branching network, known outlet accumulation.

---

### 14. No code formatter config
No `.clang-format` file exists; indentation and spacing are inconsistent across files.

**Fix:** Add a `.clang-format` at the repo root (e.g., based on `Google` or `LLVM` style with
`ColumnLimit: 100`) and run `clang-format -i` on all `src/**/*.{cpp,h}` files as a one-time
cleanup commit.

---

## Recommended Execution Order

```
1.  Add .clang-format + format all files          (cosmetic, safe first commit)
2.  Fix @file Doxygen tags                         (documentation only)
3.  Remove using namespace from headers            (compile-time safety)
4.  Replace int flags with bool in hexagon/param   (type correctness)
5.  Add enum class ErrorCode + [[nodiscard]]       (API safety)
6.  Extract JSON helper template in domain_read    (DRY refactor)
7.  Replace raw iterator loops with range-for      (readability)
8.  Add const-correctness to read-only methods     (correctness)
9.  Replace magic numbers with named constants     (maintainability)
10. Prune global.h of eco3d constants              (compile-time cleanliness)
11. Group domain.h + compset.h members into structs (encapsulation)
12. Replace O(n²) flow accumulation with Kahn sort (performance)
13. Add unit test scaffold with Catch2             (regression safety)
```

---

## Checklist

- [ ] 1. Add `.clang-format` config and apply formatting
- [ ] 2. Fix `@file` Doxygen tags in all `compset_*.cpp` files
- [ ] 3. Remove `using namespace std/rapidjson/jsonmodel` from all headers
- [ ] 4. Replace `int iFlag_*` with `bool` in `hexagon.h` and `parameter.h`
- [ ] 5. Introduce `enum class ErrorCode` and `[[nodiscard]]` on all error-returning functions
- [ ] 6. Extract `readJsonField<T>()` helper; refactor `domain_retrieve_user_input()`
- [ ] 7. Replace raw iterator loops with range-based `for` in `compset_direction.cpp`, `compset_depression.cpp`, `compset_stream.cpp`
- [ ] 8. Add `const` qualifier to all non-mutating member functions
- [ ] 9. Replace magic numbers with named `constexpr` constants
- [ ] 10. Remove eco3d-unrelated constants from `global.h` / `global.cpp`
- [ ] 11. Group `domain.h` and `compset.h` public members into nested structs
- [ ] 12. Replace O(n²) while-loop in `compset_calculate_flow_accumulation()` with Kahn's BFS
- [ ] 13. Add `tests/` directory with Catch2 scaffold and tests for core algorithms
