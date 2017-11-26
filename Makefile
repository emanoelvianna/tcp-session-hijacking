all:		invadir derrubar

invadir:   	homem-do-meio.c
			gcc homem-do-meio.c -o invadir

derrubar:   	derrubar-conexao-tcp.c
			gcc derrubar-conexao-tcp.c -o derrubar

clean:
			rm -f invadir derrubar *~