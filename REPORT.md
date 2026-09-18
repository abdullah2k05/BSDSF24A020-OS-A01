# REPORT.md - Programming Assignment 01

## Feature 2: Multi-file Build

### Q1: Explain the linking rule in this part's Makefile: `$(TARGET): $(OBJECTS)`. How does it differ from a Makefile rule that links against a library?

The linking rule `$(TARGET): $(OBJECTS)` tells make that the target executable depends on all object files. When any object file is newer than the target, make rebuilds the target by linking all object files together using gcc. The rule simply passes all `.o` files to the linker:
```
$(CC) $(CFLAGS) -o $@ $^
```

This differs from linking against a library because:
- **Direct linking**: All object files are explicitly listed and linked together. The linker resolves all symbols by looking through every provided `.o` file.
- **Library linking**: Instead of listing individual `.o` files, we reference a library archive (`.a`) or shared object (`.so`). The linker only extracts the object files it needs from the library to resolve undefined symbols, which can be faster and results in smaller executables.

### Q2: What is a git tag and why is it useful in a project? What is the difference between a simple tag and an annotated tag?

A **git tag** is a reference that points to a specific commit in the repository's history. It acts as a permanent bookmark, commonly used to mark release points (e.g., v1.0, v2.1).

**Why tags are useful:**
- They provide a human-readable name for important commits
- They allow easy checkout of specific project versions
- They serve as milestones for releases and deployments

**Simple tag vs Annotated tag:**
| Feature | Simple Tag | Annotated Tag |
|---------|-----------|---------------|
| Command | `git tag v1.0` | `git tag -a v1.0 -m "message"` |
| Storage | Just a pointer to commit | Full Git object with metadata |
| Metadata | None | Stores tagger name, email, date, and message |
| Verification | No GPG signing | Can be GPG signed |
| Recommendation | For temporary/local use | **Preferred for releases** |

### Q3: What is the purpose of creating a "Release" on GitHub? What is the significance of attaching binaries (like your client executable) to it?

A **GitHub Release** is a packaged version of your software at a specific tag. It provides:
- A formal distribution point for users to download your software
- Release notes describing changes, fixes, and features
- A stable URL for sharing specific versions

**Significance of attaching binaries:**
- Users can download and run the program without compiling from source
- Distributes pre-compiled executables for specific platforms
- Archives and libraries (`.a`, `.so`) can be included for developers
- Creates a complete, self-contained package for each version

---

## Feature 3: Static Library

### Q1: Compare the Makefile from Part 2 and Part 3. What are the key differences in the variables and rules that enable the creation of a static library?

| Aspect | Part 2 (Multi-file) | Part 3 (Static Library) |
|--------|---------------------|------------------------|
| **Variables** | `OBJECTS` (all `.o` files) | `LIB_OBJECTS` (library `.o`) + `MAIN_OBJECT` |
| **Library** | None | `LIBRARY = lib/libmyutils.a` |
| **AR tool** | Not used | `AR = ar`, `ARFLAGS = rcs` |
| **Build rule** | Links all `.o` directly | Two-step: archive `.o` into `.a`, then link main with `-l` |
| **Link command** | `gcc -o target *.o` | `gcc -o target main.o -Llib -lmyutils` |
| **Flags** | None | `-L` (library path), `-l` (link library) |

### Q2: What is the purpose of the `ar` command? Why is `ranlib` often used immediately after it?

**`ar` (archiver)** creates, modifies, and extracts from archive files. It bundles multiple object files into a single static library (`.a` file):
```bash
ar rcs libmyutils.a mystrfunctions.o myfilefunctions.o
```
- `r` - insert files into the archive
- `c` - create the archive if it doesn't exist
- `s` - write an object-file index into the archive

**`ranlib`** generates an index of the symbols in the archive and stores it inside the `.a` file. This index helps the linker quickly find which object file contains a needed symbol, speeding up the linking process. Without `ranlib`, the linker would need to search through every object file in the archive.

### Q3: When you run `nm` on your `client_static` executable, are the symbols for functions like `mystrlen` present? What does this tell you about how static linking works?

Yes, `nm` shows that symbols like `mystrlen`, `mystrcpy`, `mystrncpy`, `mystrcat`, `wordCount`, and `mygrep` are all present in the `client_static` executable.

This confirms that **static linking copies the actual code** from the library's object files directly into the final executable. The executable becomes self-contained — it does not need any external library files at runtime. The trade-off is a larger binary size, but the program can run on any system without requiring the library to be installed.

---

## Feature 4: Dynamic Library

### Q1: What is Position-Independent Code (`-fPIC`) and why is it a fundamental requirement for creating shared libraries?

**Position-Independent Code (PIC)** is machine code that can execute correctly regardless of its absolute memory address. The `-fPIC` flag tells the compiler to generate code that uses relative addressing instead of absolute addresses.

**Why it's required for shared libraries:**
- Shared libraries (`.so` files) are loaded into memory at **different addresses** each time a program uses them
- The OS can place the library anywhere in the virtual address space
- Without PIC, the code would only work at the specific address it was compiled for
- PIC ensures the code uses a **Global Offset Table (GOT)** for references, making it relocatable at load time

### Q2: Explain the difference in file size between your static and dynamic clients. Why does this difference exist?

| Build Type | File Size |
|-----------|-----------|
| Static (`client_static`) | Larger |
| Dynamic (`client_dynamic`) | Smaller |

**Why the difference exists:**
- **Static linking**: Copies the **entire code** of the library functions into the executable. Every function from `libmyutils.a` is embedded in `client_static`.
- **Dynamic linking**: Only stores a **reference** to the shared library. The actual code remains in `libmyutils.so` and is loaded on-demand at runtime. The executable only contains stubs and the dynamic linker metadata.

In our case, both showed similar sizes because our library is small and glibc was also linked. In real-world applications with large libraries, the difference is dramatic.

### Q3: What is the `LD_LIBRARY_PATH` environment variable? Why was it necessary to set it for your program to run, and what does this tell you about the responsibilities of the operating system's dynamic loader?

**`LD_LIBRARY_PATH`** is an environment variable that tells the dynamic linker where to search for shared libraries at runtime (separate from the default system paths like `/usr/lib`).

**Why it was necessary:**
- Our custom `libmyutils.so` is in a non-standard location (`lib/`)
- The OS dynamic loader only searches default paths (`/lib`, `/usr/lib`, etc.) by default
- Without `LD_LIBRARY_PATH`, the loader reports: `cannot open shared object file: No such file or directory`

**Responsibilities of the dynamic loader:**
1. Reads the executable's dynamic section to find required libraries
2. Searches for libraries in standard paths and `LD_LIBRARY_PATH`
3. Maps the library into the process's virtual address space
4. Resolves all symbol references between the executable and libraries
5. Performs any necessary relocations for PIC code

---

## Feature 5: Man Pages & Installation

### Man Page Structure

Each man page follows the standard groff format with these sections:

| Macro | Purpose |
|-------|---------|
| `.TH` | Title header — sets the command name, section, date, and manual group |
| `.SH NAME` | Brief one-line description of the function |
| `.SH SYNOPSIS` | Shows the function signature/how to call it |
| `.SH DESCRIPTION` | Detailed explanation of what the function does, parameters, and return values |
| `.SH AUTHOR` | Contact information for the author |

### Install Target

The `install` target in the Makefile:
```makefile
install: $(STATIC_TARGET)
    install -d $(DESTDIR)$(PREFIX)/bin
    install -d $(DESTDIR)$(PREFIX)/share/man/man3
    install -m 755 $(STATIC_TARGET) $(DESTDIR)$(PREFIX)/bin/client
    install -m 644 man/man3/*.3 $(DESTDIR)$(PREFIX)/share/man/man3/
```

This copies the executable to `/usr/local/bin/` and man pages to `/usr/local/share/man/man3/`, making the program accessible system-wide via `client` command and `man mystrlen`.
