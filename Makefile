all:		cliente

cliente:   	icmp_descobrir.c
			gcc icmp_descobrir.c -o descobrir

clean:
			rm -f descobrir *~