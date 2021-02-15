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
