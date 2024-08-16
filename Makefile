CSOURCES = src/main.c src/fs/fs.c src/cpf/nx/end.c src/cpf/nxfs/mkfile.c src/cpf/nx/help.c src/cpf/nx/ver.c src/cpf/nxfs/fstat.c src/cpf/nxfs/rm.c

all:
	gcc $(CSOURCES) -o bin/nx.exe

run: all
	bin/nx.exe

debug:
	gcc $(CSOURCES) -g -o bin/nx_dbg.exe
	gdb bin/nx_dbg.exe