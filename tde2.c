#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void menu();
void usuarios();
void pacientes();
void arquivosLocais();

char titulo[50] = "TDE2";

int main() {
	setlocale(LC_ALL, "Portuguese");
    system("chcp 65001");
    system("title TDE2");
    system("mode con: cols=62 lines=30");
    system("color 0E");
	menu();
}

void menu() {
	int opcao;
	system("cls");
	do {
		printf(" [--------------------- %s ---------------------]", titulo);
		printf("\n\n");
		printf(" 1- Usuários");
		printf(" 2- Pacientes");
		printf(" 3- Arquivos Locais");
		printf(" 0- Sair");
		printf("\n\n");
		printf(" Digite a opção: ");
		scanf("%d", &opcao);
		switch(opcao) {
			case 0:
				printf("\n Saindo do programa...");
				break;
			case 1:
				usuarios();
				break;
			case 2:
				pacientes();
				break;
		}
	} while (opcao != 0);
}

void usuarios() {
    int opcao;
	system("cls");
	do {
		printf(" [--------------------- %s ---------------------]", titulo);
		printf("\n\n");
		printf(" 1- Usuários");
		printf(" 2- Pacientes");
		printf(" 3- Arquivos Locais");
		printf(" 0- Sair");
		printf("\n\n");
		printf(" Digite a opção: ");
		scanf("%d", &opcao);
		switch(opcao) {
			case 0:
				printf("\n Saindo do programa...");
				break;
			case 1:
				usuarios();
				break;
			case 2:
				pacientes();
				break;
		}
	} while (opcao != 0);
	system("pause");
	menu();
}

void pacientes() {
    int opcao;
	system("cls");
	do {
		printf(" [--------------------- %s ---------------------]", titulo);
		printf("\n\n");
		printf(" 1- Usuários");
		printf(" 2- Pacientes");
		printf(" 3- Arquivos Locais");
		printf(" 0- Sair");
		printf("\n\n");
		printf(" Digite a opção: ");
		scanf("%d", &opcao);
		switch(opcao) {
			case 0:
				printf("\n Saindo do programa...");
				break;
			case 1:
				usuarios();
				break;
			case 2:
				pacientes();
				break;
		}
	} while (opcao != 0);
	system("pause");
	menu();
}

void arquivosLocais() {
    int opcao;
	system("cls");
	do {
		printf(" [--------------------- %s ---------------------]", titulo);
		printf("\n\n");
		printf(" 1- Usuários");
		printf(" 2- Pacientes");
		printf(" 3- Arquivos Locais");
		printf(" 0- Sair");
		printf("\n\n");
		printf(" Digite a opção: ");
		scanf("%d", &opcao);
		switch(opcao) {
			case 0:
				printf("\n Saindo do programa...");
				break;
			case 1:
				usuarios();
				break;
			case 2:
				pacientes();
				break;
		}
	} while (opcao != 0);
	system("pause");
	menu();
}