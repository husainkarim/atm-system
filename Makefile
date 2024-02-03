objects = src/main.o src/system.o src/auth.o
CFLAGS = -I/usr/include/sqlite3
LDFLAGS = -lsqlite3
atm : $(objects)
	cc -o atm $(objects)

main.o : src/header.h
kbd.o : src/header.h
command.o : src/header.h
display.o : src/header.h
insert.o : src/header.h
search.o : src/header.h
files.o : src/header.h
utils.o : src/header.h

clean :
	rm -f $(objects)
