# Rationale

* The classical RBMDS approach does not work well as it must with modern object
  programming languages
* There is the need for practical high-level OOP language which mixes
  transparent persistence and ease of use for everyday tasks.
* SNIA NVM Programming Model and PMDK/`libpmem` provide a usable layer of
  persistence, but it has no dynamic script language over it for experimenting
  and making little projects.
    * We require dynamic persistent language for ease, fast and fun programming
    * `FORTH` is the easiest language to write from scratch ever possible, but
    * `Smalltalk` is the easiest language and IDE for use by both the end-user
      and practical programmer.
* software development system must be self-inductive: it must provide tools for
  bootstrapping and extending itself
    * the only high-level source code generation is possible: there is no other
      way to provide real integration with existing language stacks, and
      source-level portability
