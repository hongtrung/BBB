
var :="var"
var3 :="var3"

var1 = $(var)
var2 := $(var)
var3 ?= $(var)

var := "changed"

rule3: 
	@echo "$(var1)"
	@echo "$(var2)"
	@echo "$(var3)"
all:
	gcc -o examb hello.c main.c -I .
rule1:
	echo "hi rule1"
rule2:
	@echo "hi rule2"
clear:
	rm -rf examb

