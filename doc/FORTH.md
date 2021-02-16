# FORTH

* `FORTH` is the easiest language to write from scratch ever possible, so it is
  only the single reason to use it.

* From the other practical points of view this language is ugly: low-level,
  no-syntax, write-only, no memory-access protection, etc.

* Anyway, FORTH is still the best for CLI especially for very small systems
  (embedded, IoT) -- command shell not only enough but also very usable.  

* Here, we use FORTH-like assembly as a portable machine language and use source
  code files as the input for the system startup.

* For the prototype, portability is many times more important than speed or
  compactness.
  
* We can select bytecode format and command set to be easy to disassemble and
  generate, but it still able to run fast enough for tiny software systems.

## Principles

* make programming language as simple as possible / **no-syntax** /
  * very simple no-syntax: any group of non-space chars means **word**
    (function) name
    * the only single exception is integer numbers, there is no floating-point
  * every next word from the source code stream (command line input) parsed will
    be immediately executed before the next word
  * the system works in two modes: interpretation + compilation
    * in compilation mode, every parsed word will be compiled as its call at the
      end of the used memory
      * some **immediate** words must be anyway executed in compilation mode
        because these words implement the compiler: control structures,
        strings,..
* there is no syntax parser: every word can be elementary selected till the next
  delimiter (tab/space, end-of-line, string paren)
* compiled words are stored with their headers (names) in a dumb single linked
  list from the beginning of memory
  * parsed word name goes thru the lookup via this words **vocabulary**, and 
    * founded compiled code can be executed immediately, or 
    * the `call` command to this code can be compiled
* the **compilation** is just only adding byte by byte to the end of the used
  memory
  * any user-defined word is able to be *immediate*, and can modify memory --
    so, user is able to extend the compiler as it wants to do
* due to the lack of a syntax parser which able to look forward, you are forced
  to use **postfix notation**: parameters must be first pushed into the
  operational data stack before the word name which will process them    
  * any user-defined word is able to look ahead at the source stream by fetching
    the next words -- you can use the simplest case of prefix notation in some
    cases
* all processing must be done via the using of the stack machine concept
  * all operations must be done on the **data stack**
  * there are only a few words to fetch/store data between stack and memory
  * word (commands, functions) has no parameters because any word has only one
    global parameter -- the data stack, and the only few top stack elements
    which can be manipulated
* postfix notation and stack processing makes the FORTH language ugly and
  unreadable
  * you must track the state of data stack in every machine command step, and
    shake it if you need some deeper data element, or two top elements in
    reverse order
  * there are no any parens, operators and their precedence
  * there are no floating-point, dynamic memory allocation, libraries,..
  * **it's the price of the extremal simplicity, which lets you write your own
    FORTH in a few days from scratch** (including bare machine code)
  * if you prefer esoteric coding and the hacking of very light systems, it's
    yours
