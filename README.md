# Fakefile Interpreter

An interpreter for a `Fakefile` file, which is much like a simple version of `Makefile`.

```make
target: dependency.c other_dep.txt
    cat other_dep.txt | grep content
    cc -g -o target dependency.c
    ...

# this is a comment

other_dep.txt:
    echo "other dep file content" > other_dep.txt
```

Adapted from [the way Makefiles work](https://www.gnu.org/software/make/manual/html_node/index.html). Implemented a simpler subset of this functionality and, instead of passing commands to the shell to be ran, created an pipeline utilizing `fork()`, `exec()`, `pipe()` and `dup2()` in order to practice process control.

### Execution Process

For running the application follow the following steps

1. Run `make` for creating the `fake` executable file
1. `cd` into the test directory
1. For running the contents within Fakefile use command `../fake`
1. (Optional) If you wish to run any other than Fakefile use the `-f` flag: `../fake -f Newfile`


### Implementation

#### First Steps

1. Automatically open a file named `Fakefile` when the program (named `fake`) is executed.
1. Parse this file line by line looking for:
    * Comments to ignore (any line that starts with a `#`)
    * Rule targets and prerequisites (any line that doesn't start with a `#` or `\t`)
    * "Recipe" actions (lines under rule targets and prerequisites that begin with `\t`)
1. Complete "recipes" are separated by a blank line.
1. Annoyingly throws errors if the file is formatted incorrectly.
1. There are **no** "implicit rules", everything must be included in the `Fakefile`.

#### [Parsing Recipes](https://www.gnu.org/software/make/manual/html_node/Rule-Example.html#Rule-Example)

1. For each recipe, check if the **target** is missing in the current directory or if any dependencies are more recent than the file named **target**
    * If this is true, perform each action under the target and prerequisites
    * If false, the target is up to date, nothing needs to be done
1. Because some targets can depend on other targets that come later in the file, the entire `Fakefile` must be read first and a _dependency tree_ created. This data structure is up to you (but a stack _should_ be sufficient ...)

#### Final Steps

1. Execute each command in the recipe using `fork()` and `exec()` family of functions.
1. Parse the command line, looking for `|` (pipe) characters to use the `pipe()` function, or `<`/`>` (input/output redirection) to redirect a file as input or output, respectively.
1. If a recipe fails (exit status was non-zero), then stops executing recipes. 
1. When executed, if the user provides a target name, runs that target instead of the first (default) target.

#### Testing

1. Compared the behavior with `make` tool by using the `-f` flag: `make -f Fakefile`





