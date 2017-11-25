all:		invadir

invadir:   	homem-do-meio.c
			gcc homem-do-meio.c -o invadir

clean:
			rm -f invadir *~