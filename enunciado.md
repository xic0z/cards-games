# Guião 1

## Editores de texto

Eis os editores que se encontram instalados na máquina virtual:

- nano
- vim

## Vídeos a ver antes da aula

- https://www.youtube.com/watch?v=qgszy9GquRs
- https://www.youtube.com/watch?v=xSnetY3eoIk


## Objetivos
- Aprender a compilação separada
- Fazer uma ```makefile``` simples
- Correr um programa dentro do ```gdb```
- Ser capaz de imprimir o conteúdo de variáveis
- Ser capaz de colocar um ```breakpoint``` numa linha e numa função
- Ser capaz de colocar um ```breakpoint``` condicional

## Passos a executar

1. Vá buscar os dois ficheiros e coloque-os numa pasta;
1. Crie uma ```makefile``` para compilar o programa;
1. Use a opção ```-ggdb``` para compilar com informação para o ```debugger```;
1. Use a flag ```-lm``` para compilar com a biblioteca de matemática;
1. A sua ```makefile``` deve suportar a compilação separada;
1. Corra o programa dentro do ```gdb```;
1. Use o comando ```help```;
1. Veja o que fazem os comandos ```run``` e ```cont``` no ```gdb```;
1. Use o comando ```help``` para cada um dos comandos do ```gdb``` que encontrar abaixo;
1. Liste o conteúdo da função ```main``` e da função ```raizes``` usando o comando ```list```;
1. Coloque um ```breakpoint``` na função ```raizes``` usando o comando b;
1. Corra o programa e escreva 6 6 -12;
1. Imprima o valor dos argumentos da função ```raizes``` usando o comando ```p```;
1. Saia do ```gdb``` (em alternativa veja a ajuda com ```help breakpoints``` para ver como apagar um ```breakpoint```);
1. Volte a correr o programa dentro do ```gdb``` mas coloque um breapoint na linha que imprime os valores com a função ```printf```;
1. Imprima os valores das variáveis ```r1``` e ```r2```;
1. Coloque um ```breakpoint``` na linha que atribui o valor à variável ```r1```;
1. Corra o programa dentro ```gdb``` e introduza os valores 7 4 8;
1. Imprima o valor da variável ```delta```, o que se passa?
1. Consulte a ajuda dos comandos ```display``` e ```watch```;
1. Usando o comando cond, faça com que o programa pare no ```breakpoint``` só nos casos que identificou acima;q. Volte o correr o programa dentro do ```gdb``` com os dois casos que testou acima e verifique que ele só pára quando é necessário.
