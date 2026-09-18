#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");
	int naipe = atoi(argv[1]);
	int valor = atoi(argv[2]);
	
	wprintf(L"%lc\n", 0x1f0a0 + valor + 16 * naipe);
	return 0;
}
