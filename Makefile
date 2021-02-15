# \ <section:var>
MODULE       = $(notdir $(CURDIR))
OS           = $(shell uname -s)
MACHINE      = $(shell uname -m)
NOW          = $(shell date +%d%m%y)
REL          = $(shell git rev-parse --short=4 HEAD)
CORES        = $(shell grep processor /proc/cpuinfo| wc -l)
# / <section:var>
# \ <section:dir>
CWD          = $(CURDIR)
DOC          = $(CWD)/doc
BIN          = $(CWD)/bin
TMP          = $(CWD)/tmp
SRC          = $(CWD)/src
INC          = $(CWD)/include
# / <section:dir>
# \ <section:tool>
WGET         = wget -c
CURL         = curl
CXX         ?= g++
LEX          = flex
YACC         = bison
FORMAT       = clang-format -style=LLVM
# / <section:tool>
# \ <section:src>
C += src/vm.cpp
P += tmp/lexer.cpp tmp/parser.cpp
H += include/vm.hpp
S += $(C) $(H)
# / <section:src>
# \ <section:all>
.PHONY: all
all: ./bin/$(MODULE)

.PHONY: repl
repl: ./bin/$(MODULE) src/empty.p
	./$^

CFLAGS += -I$(TMP) -I$(INC) -I$(SRC)

./bin/$(MODULE): $(C) $(P) $(H)
	$(FORMAT) -i $(C) $(H)
	$(CXX) $(CFLAGS) -o $@ $(C) $(P) $(L)

tmp/lexer.cpp: src/lexer.lex
	$(LEX) -o $@ $<

tmp/parser.cpp: src/parser.yacc
	$(YACC) -o $@ $<
# / <section:all>
# \ <section:doc>
.PHONY: doc
doc: \
	doc/SNIA_NVMProgrammingModel_v1.2.pdf \
	doc/2020_Book_ProgrammingPersistentMemory.pdf \
	doc/ALittleSmalltalk.pdf doc/SmallTalk-80.pdf \
	doc/Threaded_interpretive_languages.pdf
doc/2020_Book_ProgrammingPersistentMemory.pdf:
	$(WGET) -O $@ https://github.com/ponyatov/plang/releases/download/150221-077d/2020_Book_ProgrammingPersistentMemory.pdf
doc/SNIA_NVMProgrammingModel_v1.2.pdf:
	$(WGET) -O $@ https://www.snia.org/sites/default/files/technical_work/final/NVMProgrammingModel_v1.2.pdf
doc/ALittleSmalltalk.pdf:
	$(WGET) -O $@ http://rmod-files.lille.inria.fr/FreeBooks/LittleSmalltalk/ALittleSmalltalk.pdf
doc/SmallTalk-80.pdf:
	$(WGET) -O $@ http://rmod-files.lille.inria.fr/FreeBooks/BlueBook/Bluebook.pdf
doc/Threaded_interpretive_languages.pdf:
	$(WGET) -O $@ http://sinclairql.speccy.org/archivo/docs/books/Threaded_interpretive_languages.pdf

.PHONY: doxy
doxy:
	rm -rf docs ; doxygen doxy.gen 1>/dev/null
# / <section:doc>
# \ <section:install>
.PHONY: install
install: $(OS)_install
	$(MAKE) doc
.PHONY: update
update: $(OS)_update
.PHONY: Linux_install Linux_update
Linux_install Linux_update:
	sudo apt update
	sudo apt install -u `cat apt.txt`
# / <section:install>
# \ <section:merge>
MERGE  = Makefile README.md .vscode $(S) apt.txt
MERGE += doc doxy.gen
.PHONY: main
main:
	git push -v
	git checkout $@
	git pull -v
	git checkout shadow -- $(MERGE)
	$(MAKE) doxy
.PHONY: shadow
shadow:
	git push -v
	git checkout $@
	git pull -v
	$(MAKE) doxy
.PHONY: release
release:
	git tag $(NOW)-$(REL)
	git push -v && git push -v --tags
	$(MAKE) shadow
.PHONY: zip
zip:
	git archive \
		--format zip \
		--output $(TMP)/$(MODULE)_$(NOW)_$(REL).src.zip \
	HEAD
# / <section:merge>
