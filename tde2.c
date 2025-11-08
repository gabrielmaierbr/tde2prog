#include <stdio.h>
#include <stdlib.h>
#include <string.h> //manipulação das strings
#include <locale.h> //escolher o idioma do console pelo setlocale
#include <conio.h> //usado na captura de caracteres da senha para censurar na tela de login
#include <ctype.h> //manipulação de caracteres
#include <windows.h> //Usado para deixar em ptbr o console e todas as funções que usam system("");

//Estruturas
typedef struct {
    char nome[50];
    int idade;
    char sexo;
    int prioridade;
    char diagnostico[100];
} Paciente;

typedef struct {
    char login[20];
    char senha[30];
    char tipo[20];
} Usuario;

//Declaração das Funções
void menuADM();
void credenciais();
void salvarUsuariosJSON();
void lerUsuariosJSON();
char* lerSenhaComMascara();

//Variáveis globais
char titulo[50] = "SISTEMA HOSPITALAR";
Usuario usuariosSistema[50];
Paciente pacientesSistema[100];
int totalUsuarios = 0;
Usuario usuarioLogado;

void credenciais() {
    char login[30];
    char* senha; //Ponteiro para usar a função
    int autenticado = 0; //Servir de booleano para saber se vai logar ou não
    
    lerUsuariosJSON();
    
    if (totalUsuarios == 0) { //Detecta se tem ou nao usuarios (arquivo json), caso nao houver, criar um novo com os usuários padrões
        printf("Criando usuários padrão...\n");
        
        strcpy(usuariosSistema[0].login, "admin");
        strcpy(usuariosSistema[0].senha, "adm123");
        strcpy(usuariosSistema[0].tipo, "admin");
        
        strcpy(usuariosSistema[1].login, "medico");
        strcpy(usuariosSistema[1].senha, "med123");
        strcpy(usuariosSistema[1].tipo, "medico");
        
        strcpy(usuariosSistema[2].login, "enfermeiro");
        strcpy(usuariosSistema[2].senha, "enf123");
        strcpy(usuariosSistema[2].tipo, "enfermeiro");
        
        totalUsuarios = 3;
        salvarUsuariosJSON();
        printf("Arquivo usuarios.json criado com usuários padrão.\n");
        system("pause");
    }

    while (!autenticado) {
        system("cls");
        printf(" [--------------------- %s ---------------------]\n\n", titulo);
        printf(" Login: ");
        scanf("%s", login);
        
        //Limpa o buffer do teclado antes de ler a senha, forma mais eficiente que o fflush(stdin)
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        printf(" Senha: ");
        senha = lerSenhaComMascara();

        //debug para mostrar o que está sendo comparado, remover antes da entrega do sistema
        printf("DEBUG: Login digitado: '%s'\n", login);
        printf("DEBUG: Senha digitada: '%s'\n", senha);
        
        for (int i = 0; i < totalUsuarios; i++) {
            printf("DEBUG: Comparando com usuario[%d]: login='%s' senha='%s'\n", i, usuariosSistema[i].login, usuariosSistema[i].senha);      
            if (strcmp(login, usuariosSistema[i].login) == 0 && 
                strcmp(senha, usuariosSistema[i].senha) == 0) {
                
                autenticado = 1;
                usuarioLogado = usuariosSistema[i];
                
                printf("\n\n Login realizado com sucesso. Bem-vindo, %s!\n", login);
                system("pause");
                
                if (strcmp(usuarioLogado.tipo, "admin") == 0) {
                    menuADM();
                } else if (strcmp(usuarioLogado.tipo, "medico") == 0) {
                    printf("menuMEDICO()\n");
                    system("pause");
                } else if (strcmp(usuarioLogado.tipo, "enfermeiro") == 0) {
                    printf("menuENFERMEIRO()\n");
                    system("pause");
                }
                break;
            }
        }
        
        if (!autenticado) {
            printf("\n\n Login ou senha incorretos\n");
            system("pause");
        }
    }
}

void salvarUsuariosJSON() {
    FILE *file = fopen("usuarios.json", "w"); //Abre o arquivo usuarios.json para alterações, w de write para escrever
    if (!file) {
        printf("Erro ao criar arquivo usuarios.json\n");
        return;
    }
    
    fprintf(file, "{\n");
    fprintf(file, "  \"usuarios\": [\n");
    
    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(file, "    {\n");
        fprintf(file, "      \"login\": \"%s\",\n", usuariosSistema[i].login);
        fprintf(file, "      \"senha\": \"%s\",\n", usuariosSistema[i].senha);
        fprintf(file, "      \"tipo\": \"%s\"\n", usuariosSistema[i].tipo);
        
        if (i < totalUsuarios - 1) {
            fprintf(file, "    },\n");
        } else {
            fprintf(file, "    }\n");
        }
    }
    
    fprintf(file, "  ]\n");
    fprintf(file, "}\n");
    
    fclose(file);
    printf("Arquivo usuarios.json salvo com %d usuários.\n", totalUsuarios);

    //O processo é encerrado e salva usuários padrões pré-definidos caso o arquivo seja apagado
}

void lerUsuariosJSON() {
    FILE *file = fopen("usuarios.json", "r"); //Abre o arquivo para ser lido, r de read para apenas leitura
    if (!file) {
        printf("Arquivo usuarios.json nao encontrado\n");
        totalUsuarios = 0;
        return;
    }
    
    printf("---- Lendo o JSON ----\n");
    
    char linha[256];
    totalUsuarios = 0;
    int usuario_atual = -1;
    
    //Lê linha por linha
    while (fgets(linha, sizeof(linha), file) && totalUsuarios < 50) {
        //Remove quebras de linha na leitura
        linha[strcspn(linha, "\r\n")] = 0;
        
        printf("LINHA: %s\n", linha); //Retorno da informação para DEBUG
        
        //Verifica se condiz com o usuário digitado
        if (strstr(linha, "{") && usuario_atual == -1) {
            usuario_atual = totalUsuarios;
            printf("--- Iniciando usuario %d ---\n", usuario_atual);
        }
        //Verifica se é o fim de cada objeto do json, se for "{" ele considera um novo, como mostrado antes, se for "}" usado agora, mostra que é o fim de um usuário
        //e incrementa o total de usuários
        else if (strstr(linha, "}") && usuario_atual != -1) {
            printf("--- Finalizando usuario %d ---\n", usuario_atual);
            totalUsuarios++;
            usuario_atual = -1;
        }
        //Procura nos usuários do json pelo login digitado
        else if (strstr(linha, "\"login\"") && usuario_atual != -1) {
            char *start = strstr(linha, ":");
            if (start) {
                start = strchr(start, '\"');
                if (start) {
                    start++;
                    char *end = strchr(start, '\"');
                    if (end) {
                        *end = '\0';
                        strcpy(usuariosSistema[usuario_atual].login, start);
                        printf("LOGIN: %s\n", start);
                    }
                }
            }
        }
        //Procura a senha no json pela senha digitada
        else if (strstr(linha, "\"senha\"") && usuario_atual != -1) {
            char *start = strstr(linha, ":");
            if (start) {
                start = strchr(start, '\"');
                if (start) {
                    start++;
                    char *end = strchr(start, '\"');
                    if (end) {
                        *end = '\0';
                        strcpy(usuariosSistema[usuario_atual].senha, start);
                        printf("SENHA: %s\n", start);
                    }
                }
            }
        }
        //Procura o tipo de usuário de acordo com o usuário e a senha
        else if (strstr(linha, "\"tipo\"") && usuario_atual != -1) { //Detecta que linha contém o tipo e o strstr retornará o valor
            char *start = strstr(linha, ":"); //Separa o conteúdo do ":"
            if (start) {
                start = strchr(start, '\"'); //Após separar os ":", vai procurar o primeiro caractere de aspas duplas
                if (start) {
                    start++; //Avança uma posição para o primeiro caractere real da palavra da linha
                    char *end = strchr(start, '\"'); //Localiza a proxima aspa dupla que fecha o valor
                    if (end) {
                        *end = '\0'; //Substitui as aspas por \0 que é um valor nulo
                        strcpy(usuariosSistema[usuario_atual].tipo, start); //Copia o valor do tipo para a variável da struct
                        printf("TIPO: %s\n", start); //Debug para saber o que foi copiado
                    }
                }
            }
        }
    }
    
    fclose(file);
    printf("---- Leitura: %d usuarios carregados ----\n", totalUsuarios);
}

char* lerSenhaComMascara() {
    static char senha[30];
    int i = 0;
    char ch;
    
    senha[0] = '\0'; //remove todo o conteúdo da variavel com um valor nulo, prevenção de erros
    
    while (1) {
        ch = getch(); //lê o conteúdo do caractere sem precisar dar Enter
        
        if (ch == 13 || ch == 10) { //caso o usuário pressione Enter (numero 13) ou possua quebra de linha (numero 10) de acordo com ASCII
            senha[i] = '\0'; //adiciona \0 no final para colocar um valor nulo depois dos caracteres da senha, a fim de evitar erros na leitura
            printf("\n"); //pula a linha no final da leitura
            break;
        } 
        else if (ch == 8) { //8 é o numero do backspace, caso o usuario apague, decrementa o i para voltar o lugar do caractere apagado
            if (i > 0) {
                i--;
                printf("\b \b"); //esta linha serve para apagar o caractere "*" da tela de login após ter um caractere apagado na string
            }
        }
        else if (i < 29) { //verifica o tamanho do vetor de 0 a 29 e armazena cada caractere digitado na variavel senha enquanto ao mesmo tempo exibe um "*" para cada caractere digitado na tela de login
            senha[i] = ch;
            i++;
            printf("*");
        }
        else if (ch == 3 || ch == 27) { //3 é o código do CTRL + C e 27 é o do ESC, caso o usuário queira cancelar a digitação
            i = 0;
            senha[0] = '\0'; //vai limpar o conteúdo da senha ao cancelar
            printf("\n--- Senha cancelada ---\n");
            break;
        }
    }
    
    return senha;
}

void menuADM() {
    int opcao;
    
    do {
        system("cls");
        printf(" [---------- %s - MENU ADMINISTRADOR ----------]\n\n", titulo);
        printf(" 1- Gerenciar Usuários\n");
        printf(" 2- Gerenciar Pacientes\n");
        printf(" 3- Arquivos Locais\n");
        printf(" 4- Relatórios\n");
        printf(" 0- Sair\n");
        printf("\n Digite a opção: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 0:
                printf("\n Saindo do sistema...\n");
                system("pause");
                credenciais();
                break;
            case 1:
                printf("\n Gerenciar Usuários - Em desenvolvimento\n");
                system("pause");
                break;
            case 2:
                printf("\n Gerenciar Pacientes - Em desenvolvimento\n");
                system("pause");
                break;
            case 3:
                printf("\n Arquivos Locais - Em desenvolvimento\n");
                system("pause");
                break;
            case 4:
                printf("\n Relatórios - Em desenvolvimento\n");
                system("pause");
                break;
            default:
                printf("\n Opção inválida\n\n");
                system("pause");
        }
    } while (opcao != 0);
}

int main() {
    system("chcp 65001 > nul");
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "Portuguese_Brazil.65001");
    
    system("title SISTEMA HOSPITALAR");
    system("mode con: cols=120 lines=30");
    system("color 0F");
        
    credenciais();
}
// só testando se meu clone foi