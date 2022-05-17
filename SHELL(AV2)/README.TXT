Programa realizado por Joan Gil Rigo, Emanuel Hegedus y Bartomeu Capó Salas

Mejoras del programa:

	1- Se ha realizado la implementación del cd avanzado, permitiendo usar el comando cd para 		llegar a directorios que contienen espacios en su nombre.

	2- El reaper muestra más datos de lo normal, es decir, muestra como termina cualquier 		comando no interno de nuestro mini_shell, lo encontramos útil para mostrarle al usuario 	mayor información sobre los procesos, incluso en foreground.

	3- El prompt es colorido y contiene bastante información

	4- El código se encuentra comentado y es entendible en su mayor parte

Bugs y errores conocidos:

	1- De vez en cuando el programa decide pararse, según el debugger del shell ((cc -g 		(programa).c) y (gdb a.out)) el error es que el programa para al recibir una señal SIGINT 		o SIGTSTP que se salta el handler y para o pausa el shell

	2- Algunas veces puede ocurrir que el prompt no se muestre correctamente, este bug se ha 	corregido en su mayor parte, pero puede que aun queden algunos casos
