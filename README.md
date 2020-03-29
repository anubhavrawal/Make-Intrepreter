# Assignment Three: Fakefile Interpreter

For this assignment, we will write an interpreter for a `Fakefile` file, which is much like a `Makefile`.

```make
target: dependency.c other_dep.txt
    cat other_dep.txt | grep content
    cc -g -o target dependency.c
    ...

# this is a comment

other_dep.txt:
    echo "other dep file content" > other_dep.txt
```

Recall [the way Makefiles work](https://www.gnu.org/software/make/manual/html_node/index.html). We want to implement a simpler subset of this functionality and, instead of passing commands to the shell to be ran, create our own pipeline utilizing `fork()`, `exec()`, `pipe()` and `dup2()` in order to practice process control.

### Implementation

#### First Steps

1. Automatically open a file named `Fakefile` when the program (named `fake`) is executed.
1. Parse this file line by line looking for:
    * Comments to ignore (any line that starts with a `#`)
    * Rule targets and prerequisites (any line that doesn't start with a `#` or `\t`)
    * "Recipe" actions (lines under rule targets and prerequisites that begin with `\t`)
1. Complete "recipes" are separated by a blank line.
1. Annoyingly throw errors if the file is formatted incorrectly, **do not be lenient**!
1. There are **no** "implicit rules", everything must be included in the `Fakefile`.

#### [Parsing Recipes](https://www.gnu.org/software/make/manual/html_node/Rule-Example.html#Rule-Example)

1. For each recipe, check if the **target** is missing in the current directory or if any dependencies are more recent than the file named **target**
    * If this is true, perform each action under the target and prerequisites
    * If false, the target is up to date, nothing needs to be done
1. Because some targets can depend on other targets that come later in the file, the entire `Fakefile` must be read first and a _dependency tree_ created. This data structure is up to you (but a stack _should_ be sufficient ...)

#### Final Steps

1. Execute each command in the recipe using `fork()` and `exec()` family of functions.
1. You must parse the command line, look for `|` (pipe) characters to use the `pipe()` function, or `<`/`>` (input/output redirection) to redirect a file as input or output, respectively.
1. If a recipe fails (exit status was non-zero), then stop executing recipes. Show `stderr`!
1. When executed, if the user provides a target name, run that target instead of the first (default) target.

#### Tips

1. Compare your behavior with the `make` tool by using the `-f` flag: `make -f Fakefile`
1. There are two approaches to parsing command lines:
    * a loop that looks for `|`, `<` and `>` characters
    * a recursive parser that continues to fork until it reaches the end (or beginning?) of the line
1. **Do not use code from _that one_ shell tutorial**. I will know if you do this. Just don't look at it, you will lose points.


### Reading

Stephen's **Chapters 3, 7, 8:**

* **3.12 `dup2()`**
* **8.3 `fork()`**
* **8.6 `wait()` and `waitpid()`**
* **8.10 `exec()`**
* **15.2 `pipe()`**

### Makefile

Remember, you _must_ write your own `Makefile` (separate from the `Fakefile`), such that when your repository is cloned, one only has to run `make` in the folder to produce a `fake` application. **Also write a `clean` target** that removes binaries and `.o` object files or other compiler side-effects, so that we may make your program from scratch easily.

### Bonus

Can `fake` compile itself by reading the `Makefile` instead? You will have to set up your program to parse the command line switch `-f` to open a different file than `Fakefile`.

