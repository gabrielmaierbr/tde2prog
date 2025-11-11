#include <stdio.h>
#include <stdlib.h>
#include <string.h> //manipulação das strings
#include <locale.h> //escolher o idioma do console pelo setlocale
#include <conio.h> //usado na captura de caracteres da senha para censurar na tela de login
#include <ctype.h> //manipulação de caracteres
#include "cJSON.h" //Biblioteca para manipulação de JSON
#include <windows.h> //Usado para deixar em ptbr o console e todas as funções que usam system("");
#include <stdbool.h> //Para usar booleanos

//Estruturas
typedef struct {
    char login[20];
    char senha[30];
    char tipo[20];
} Usuario;

typedef struct {
    char login[20];
    char nome[50];
    int idade;
    char especialidade[20];
    char crm[15];
} Medicos;

typedef struct {
    char nome[50];
    int idade;
    char sexo;
    int prioridade;
    char diagnostico[100];
} Paciente;

// Declaração das Funções
void credenciais();
void criarUsuarioPadrao();
void jsonParaStructs();
char* lerSenhaComMascara();
void gerenciarUsuarios();

void cadastrarUsuario();
void cadastrarMedico();
void cadastrarEnfermeiro();
void cadastrarRecepcionista();
void excluirUsuario();
void excluirMedico();
void excluirEnfermeiro();
void excluirRecepcionista();

void gerenciarPacientes();
void cadastrarPaciente();
void excluirPaciente();
void verPacientes();

void gerenciarLeitos();
void criarLeito();
void excluirLeito();
void gerenciarPacientesNosLeitos();
void alocarPacienteAoLeito();
void tirarPacienteDoLeito();

void darAlta();

void menuADM();
void menuMEDICO();
void menuENFERMEIRO();
void menuRECEPCAO();

//Variáveis globais
char titulo[50] = "SISTEMA HOSPITALAR";
int totalUsuarios = 0;
int totalMedicos = 0;
Usuario usuariosSistema[50];
Usuario usuarioLogado;
Medicos medicosSistema[50];
Paciente pacientesSistema[100];

//----------------------------- Login e Senha -----------------------------

void credenciais() {
    char login[30];
    char* senha; //Ponteiro para usar a função
    int autenticado = 0; //Servir de booleano para saber se vai logar ou não
    
    jsonParaStructs();
    
    //Detecta se tem ou nao usuarios (arquivo json), caso nao houver, criar um novo com os usuários padrões
    if (totalUsuarios == 0) {
        
        // Criando usuário padrão
        strcpy(usuariosSistema[0].login, "admin");
        strcpy(usuariosSistema[0].senha, "adm123");
        strcpy(usuariosSistema[0].tipo, "admin");
        totalUsuarios = 1;
        
        criarUsuarioPadrao();
    }

    while (!autenticado) {
        system("cls");
        printf(" [--------------------- %s ---------------------]\n\n", titulo);
        printf(" Login: ");
        scanf("%s", login);
        
        // Limpa o buffer do teclado antes de ler a senha
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        printf(" Senha: ");
        senha = lerSenhaComMascara();

        printf("DEBUG: Login digitado: '%s'\n", login); // DEBUG
        printf("DEBUG: Senha digitada: '%s'\n", senha);  // DEBUG
        
        for (int i = 0; i < totalUsuarios; i++) {

            printf("DEBUG: Comparando com usuario[%d]: login='%s' senha='%s'\n", i, usuariosSistema[i].login, usuariosSistema[i].senha); // DEBUG

            if (strcmp(login, usuariosSistema[i].login) == 0 && strcmp(senha, usuariosSistema[i].senha) == 0) {
                
                autenticado = 1;
                usuarioLogado = usuariosSistema[i];
                
                printf("\n\n Login realizado com sucesso. Bem-vindo, %s!\n", login);
                system("pause");
                
                if (strcmp(usuarioLogado.tipo, "admin") == 0) {
                    menuADM();
                } else if (strcmp(usuarioLogado.tipo, "medico") == 0) {
                    menuMEDICO();
                } else if (strcmp(usuarioLogado.tipo, "enfermeiro") == 0) {
                    menuENFERMEIRO();
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

void criarUsuarioPadrao() {

    //Abre o arquivo usuarios.json para alterações, "w" de write para escrever
    FILE *file = fopen("informacoes.json", "w");
    if (!file) {
        printf("Erro ao criar arquivo usuarios.json\n");
        return;
    }

    // Cria o objeto raiz do JSON: {}
    cJSON *root = cJSON_CreateObject();

    // Cria um array para os usuários: "usuarios": []
    cJSON *usuariosArray = cJSON_CreateArray();

    // Cria o objeto usuário individual: { "login": "...", "senha": "...", "tipo": "..." }
    cJSON *usuarioObj = cJSON_CreateObject();
    cJSON_AddStringToObject(usuarioObj, "login", usuariosSistema[0].login);
    cJSON_AddStringToObject(usuarioObj, "senha", usuariosSistema[0].senha);
    cJSON_AddStringToObject(usuarioObj, "tipo", usuariosSistema[0].tipo);

    // Adiciona o usuário ao array
    cJSON_AddItemToArray(usuariosArray, usuarioObj);

    // Adiciona o array ao objeto root
    cJSON_AddItemToObject(root, "usuarios", usuariosArray);

    // Converte a estrutura cJSON em string formatada
    char *jsonString = cJSON_Print(root);

    // Escreve no arquivo
    fputs(jsonString, file);

    // Libera memória
    fclose(file);
    cJSON_Delete(root);
    free(jsonString);

    jsonParaStructs();

    //O processo é encerrado e salva usuários padrões pré-definidos caso o arquivo seja apagado
}

void jsonParaStructs() {
    FILE *file = fopen("informacoes.json", "r");
    if (!file) {
        totalUsuarios = 0;
        return;
    }
    
    // Descobre o tamanho do arquivo usuarios.json e coloca na variável "tamanho"
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Aloca memória e lê o arquivo e atribui o conteúdo ao ponteiro "buffer"
    char *buffer = (char*)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';

    // Fecha o arquivo
    fclose(file);
    
    // Converte o texto .json na estrutura da biblioteca cJSON
    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        free(buffer);
        return;
    }
    
    // Procura o array "usuarios" dentro do cJSON
    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");

    // Verifica se é um array
    if (cJSON_IsArray(usuariosArray)) {
        totalUsuarios = 0;
        cJSON *usuarioItem;

        // Percorre cada item do array (isso é um for)
        cJSON_ArrayForEach(usuarioItem, usuariosArray) {
            if (totalUsuarios >= 50) break;
            
            cJSON *login = cJSON_GetObjectItem(usuarioItem, "login");
            cJSON *senha = cJSON_GetObjectItem(usuarioItem, "senha");
            cJSON *tipo = cJSON_GetObjectItem(usuarioItem, "tipo");
            
            if (cJSON_IsString(login) && cJSON_IsString(senha) && cJSON_IsString(tipo)) {
                strcpy(usuariosSistema[totalUsuarios].login, login->valuestring);
                strcpy(usuariosSistema[totalUsuarios].senha, senha->valuestring);
                strcpy(usuariosSistema[totalUsuarios].tipo, tipo->valuestring);
                totalUsuarios++;
            }
        }
    }

    // Procura o array "medicos" dentro do cJSON
    cJSON *medicosArray = cJSON_GetObjectItem(root, "medicos");
    
    if (medicosArray == NULL) {
        medicosArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "medicos", medicosArray);

        char *jsonAtualizado = cJSON_Print(root);
        FILE *file2 = fopen("informacoes.json", "w");
        fputs(jsonAtualizado, file2);
        fclose(file2);
        free(jsonAtualizado);
    } else {
        totalMedicos = 0;
        cJSON *medicoItem;

        // Percorre cada item do array (isso é um for)
        cJSON_ArrayForEach(medicoItem, medicosArray) {
            if (totalMedicos >= 50) break;
            
            cJSON *login = cJSON_GetObjectItem(medicoItem, "login");
            cJSON *nome = cJSON_GetObjectItem(medicoItem, "nome");
            cJSON *idade = cJSON_GetObjectItem(medicoItem, "idade");
            cJSON *especialidade = cJSON_GetObjectItem(medicoItem, "especialidade");
            cJSON *crm = cJSON_GetObjectItem(medicoItem, "crm");
            
            if (cJSON_IsString(login) && cJSON_IsString(nome) && cJSON_IsNumber(idade) && cJSON_IsString(especialidade) && cJSON_IsString(crm)) {
                strcpy(medicosSistema[totalMedicos].login, login->valuestring);
                strcpy(medicosSistema[totalMedicos].nome, nome->valuestring);
                medicosSistema[totalMedicos].idade = idade->valueint;
                strcpy(medicosSistema[totalMedicos].especialidade, especialidade->valuestring);
                strcpy(medicosSistema[totalMedicos].crm, crm->valuestring);
                totalMedicos++;
            }
        }
    }
    
    
    cJSON_Delete(root);
    free(buffer);
}

char* lerSenhaComMascara() {
    static char senha[30];
    int i = 0;
    char ch;
    
    // Remove todo o conteúdo da variavel com um valor nulo, prevenção de erros
    senha[0] = '\0';
    
    while (1) {
        // Lê o conteúdo do caractere sem precisar dar Enter
        ch = getch();
        
        // Caso o usuário pressione Enter (numero 13) ou possua quebra de linha (numero 10) de acordo com ASCII
        if (ch == 13 || ch == 10) {
            // Adiciona \0 no final para colocar um valor nulo depois dos caracteres da senha, a fim de evitar erros na leitura
            senha[i] = '\0';
            printf("\n"); // Pula a linha no final da leitura
            break;
        } 

        // 8 é o numero do backspace, caso o usuario apague, decrementa o i para voltar o lugar do caractere apagado
        else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b"); // Esta linha serve para apagar o caractere "*" da tela de login após ter um caractere apagado na string
            }
        }

        // Verifica o tamanho do vetor de 0 a 29 e armazena cada caractere digitado na variavel senha enquanto ao mesmo tempo exibe um "*" para cada caractere digitado na tela de login
        else if (i < 29) {
            senha[i] = ch;
            i++;
            printf("*");
        }

        // 3 é o código do CTRL + C e 27 é o do ESC, caso o usuário queira cancelar a digitação
        else if (ch == 3 || ch == 27) {
            i = 0;
            senha[0] = '\0'; // Vai limpar o conteúdo da senha ao cancelar
            printf("\n--- Senha cancelada ---\n");
            break;
        }
    }
    
    return senha;
}

//--------------------------- Gerenciar Usuários ---------------------------

void gerenciarUsuarios() {
    int opcao;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> GERENCIAR USUÁRIO <---\n");
        printf("Opções [0 para voltar]:\n\n");
        printf(" 1- Cadastrar Usuário\n");
        printf(" 2- Excluir Usuário\n");
        printf("\n Digite a opção: ");
        scanf("%d",&opcao);
        
        if (opcao == 1) {
            cadastrarUsuario();
        } else if (opcao == 2) {
            excluirUsuario();
        } else if (opcao == 0) {
            break;
        } else {
            printf("\n Opção inválida\n\n");
            system("pause");
        }
    }
}

void cadastrarUsuario() {
    int opcao;
    while(1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> CADASTRAR USUÁRIO <---\n");
        printf("Qual usuário deseja cadastrar [0 para voltar]?\n\n");
        printf(" 1- Médico\n");
        printf(" 2- Enfermeiro\n");
        printf(" 3- Recepcionista\n");
        printf("\n Digite a opção: ");

        scanf("%d",&opcao);

        if (opcao==1) {
            cadastrarMedico();
        } else if (opcao==2) {
            cadastrarEnfermeiro();
        } else if (opcao==3) {
            cadastrarRecepcionista();
        } else if (opcao==0) {
            break;
        } else {
            printf("\n Opção inválida\n\n");
            system("pause");
        }
    }
}

void cadastrarMedico() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> CADASTRAR MÉDICO(A) <---\n");

    FILE *file = fopen("informacoes.json","r");
    cJSON *root;
    char *buffer;

    // Se o arquivo nãp for encontrado
    if(!file) {
        return;
    }

    // Vê o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Lê o arquivo e põe no ponteiro 'buffer'
    buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';

    // Fecha o arquivo
    fclose(file);

    // Transforma em formato JSON
    root = cJSON_Parse(buffer);
    free(buffer);

    // Caso o arquivo não possa ser transformado
    if(!root) {
        return;
    }

    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");
    cJSON *medicosArray = cJSON_GetObjectItem(root, "medicos");

    int qtd_de_medicos;
    printf("Quantos médicos deseja cadastrar? ");
    scanf("%d",&qtd_de_medicos);

    for (int i=0;i<qtd_de_medicos;i++) {
        system("cls");

        cJSON *novoUsuario = cJSON_CreateObject();
        cJSON *novoMedico = cJSON_CreateObject();

        printf("\n --- Cadastrando Médico(a) %d de %d",i+1,qtd_de_medicos);
        printf("\nInsira o Nome do Médico(a): ");
        scanf(" %[^\n]", medicosSistema[totalMedicos].nome);
        printf("\nInsira a Idade do Médico(a): ");
        scanf("%d", &medicosSistema[totalMedicos].idade);
        printf("\nInsira a Especialidade do Médico(a): ");
        scanf(" %s", medicosSistema[totalMedicos].especialidade);
        printf("\nInsira o CRM do Médico(a): ");
        scanf(" %s", medicosSistema[totalMedicos].crm);

        while (1) {
            int repetido = false;
            printf("\nInsira o Login do Usuário do Médico(a): ");
            scanf(" %s", medicosSistema[totalMedicos].login);

            for (int i=0;i<totalUsuarios;i++) {
                if (strcmp(medicosSistema[totalMedicos].login, usuariosSistema[i].login) == 0) {
                    repetido = true;
                    printf("Este login já existe!\n");
                    break;
                }
            }
            if (repetido == false) {
                break;
            }
        }

        strcpy(usuariosSistema[totalUsuarios].login, medicosSistema[totalMedicos].login);
        printf("\nInsira a Senha do Usuário do Médico(a): ");
        scanf(" %s", usuariosSistema[totalUsuarios].senha);
        strcpy(usuariosSistema[totalUsuarios].tipo, "medico");

        // Objeto do Medico
        cJSON_AddStringToObject(novoMedico, "login", medicosSistema[totalMedicos].login);
        cJSON_AddStringToObject(novoMedico, "nome", medicosSistema[totalMedicos].nome);
        cJSON_AddNumberToObject(novoMedico, "idade", medicosSistema[totalMedicos].idade);
        cJSON_AddStringToObject(novoMedico, "especialidade", medicosSistema[totalMedicos].especialidade);
        cJSON_AddStringToObject(novoMedico, "crm", medicosSistema[totalMedicos].crm);

        // Objeto do Usuário
        cJSON_AddStringToObject(novoUsuario, "login", usuariosSistema[totalUsuarios].login);
        cJSON_AddStringToObject(novoUsuario, "senha", usuariosSistema[totalUsuarios].senha);
        cJSON_AddStringToObject(novoUsuario, "tipo", usuariosSistema[totalUsuarios].tipo);

        // Adicionando os objetos ao JSON
        cJSON_AddItemToArray(usuariosArray, novoUsuario);
        cJSON_AddItemToArray(medicosArray, novoMedico);

        printf("\nMédico %s cadastrado com sucesso!\n\n", medicosSistema[totalMedicos].nome);
        system("pause");

        totalUsuarios++;
        totalMedicos++;

    }

    // Salvar tudo no json
    FILE *fileOut = fopen("informacoes.json","w");
    char *jsonAtualizado = cJSON_Print(root);
    fprintf(fileOut, "%s", jsonAtualizado);

    fclose(fileOut);
    free(jsonAtualizado);
    cJSON_Delete(root);

}

void cadastrarEnfermeiro() {

}

void cadastrarRecepcionista() {

}

void excluirUsuario() {
    int opcao;
    while(1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR USUÁRIO <---\n");
        printf("Qual usuário deseja excluir [0 para voltar]?\n\n");
        printf(" 1- Médico\n");
        printf(" 2- Enfermeiro\n");
        printf(" 3- Recepcionista\n");
        printf("\n Digite a opção: ");
        scanf("%d",&opcao);

        if (opcao==1) {
            excluirMedico();
        } else if (opcao==2) {
            excluirEnfermeiro();
        } else if (opcao==3) {
            excluirRecepcionista();
        } else if (opcao==0) {
            break;
        } else {
            printf("\n Opção inválida\n\n");
            system("pause");
        }
    }
}

void excluirMedico() {

}

void excluirEnfermeiro() {

}

void excluirRecepcionista() {

}

//--------------------------- Gerenciar Pacientes ---------------------------

void gerenciarPacientes() {
    int opcao;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> GERENCIAR PACIENTES <---\n");
        printf("Opções [0 para voltar]:\n\n");
        printf(" 1- Cadastrar Paciente\n");
        printf(" 2- Excluir Paciente\n");
        printf(" 3- Ver Pacientes\n");
        printf("\n Digite a opção: ");
        scanf("%d",&opcao);

        if (opcao==1) {
            cadastrarPaciente();
        } else if (opcao==2) {
            excluirPaciente();
        } else if (opcao==3) {
            verPacientes();
        } else if (opcao==0) {
            break;
        } else {
            printf("\n Opção Inválida\n\n");
            system("pause");
        }
    }
}

void cadastrarPaciente() {

}

void excluirPaciente() {

}

void verPacientes() {

}

//--------------------------- Gerenciar Leitos ---------------------------

void gerenciarLeitos() {
    int opcao;
    while (1) {
        system("cls");
        printf(" ---> GERENCIAR LEITOS <---\n");
        printf("Opções [0 para voltar]:\n\n");
        printf(" 1- Criar Leito\n");
        printf(" 2- Excluir Leito\n");
        printf(" 3- Gerenciar Pecientes nos Leitos\n");
        printf("\n Digite a opção: ");
        scanf("%d",&opcao);

        if (opcao==1) {
            criarLeito();
        } else if (opcao==2) {
            excluirLeito();
        } else if (opcao==3) {
            gerenciarPacientesNosLeitos();
        } else if (opcao==0) {
            break;
        } else {
            printf("\n Opção Inválida\n\n");
            system("pause");
        }
    }
}

void criarLeito() {

}

void excluirLeito() {

}

void gerenciarPacientesNosLeitos() {
    int opcao;
    while(1) {
        system("cls");
        printf(" [--------- GERENCIAR PACIENTES NOS LEITOS ---------]\n\n");
        printf("O que deseja fazer [0 para voltar]?\n\n");
        printf(" 1- Alocar Paciente ao Leito\n");
        printf(" 2- Tirar Paciente do leito\n");
        printf("\n Digite a opção: ");
        scanf("%d",&opcao);

        if (opcao==1) {
            alocarPacienteAoLeito();
        } else if (opcao==2) {
            tirarPacienteDoLeito();
        } else if (opcao==0) {
            break;
        } else {
            printf(" \nOpção Inválida\n\n");
            system("pause");
        }
    }

}

void alocarPacienteAoLeito() {

}

void tirarPacienteDoLeito() {

}

//--------------------------- Exclusivo do Médico ---------------------------

void darAlta() {

}

//---------------------------------- MENUS ----------------------------------

void menuADM() {
    int opcao;
    
    do {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> MENU ADMINISTRADOR <---\n");
        printf(" 1- Gerenciar Usuários\n");
        printf(" 2- Gerenciar Pacientes\n");
        printf(" 3- Gerenciar Leitos\n");
        printf(" 0- Sair\n");
        printf("\n Digite a opção: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 0:
                printf("\n Saindo do sistema...\n\n");
                system("pause");
                credenciais();
                break;
            case 1:
                gerenciarUsuarios();
                break;
            case 2:
                gerenciarPacientes();
                break;
            case 3:
                gerenciarLeitos();
                break;
            default:
                printf("\n Opção inválida\n\n");
                system("pause");
        }
    } while (opcao != 0);
}

void menuMEDICO() {
    int opcao;

    do {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> MENU MÉDICO(A) <---\n");
        printf(" 1- Ver Pacientes\n");
        printf(" 2- Dar Alta\n");
        printf(" 0- Sair\n");
        printf("\n Digite a opção: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 0:
                printf("\n Saindo do sistema...\n\n");
                system("pause");
                credenciais();
                break;
            case 1:
                verPacientes();
                break;
            case 2:
                darAlta();
                break;
            default:
                printf("\n Opção inválida\n\n");
                system("pause");
        }
    } while (opcao != 0);

}

void menuENFERMEIRO() {
        int opcao;

    do {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> MENU ENFERMEIRO(A) <---\n");
        printf(" 1- Ver Pacientes\n");
        printf(" 2- Gerenciar pacientes nos Leitos\n");
        printf(" 0- Sair\n");
        printf("\n Digite a opção: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 0:
                printf("\n Saindo do sistema...\n\n");
                system("pause");
                credenciais();
                break;
            case 1:
                verPacientes();
                break;
            case 2:
                gerenciarPacientesNosLeitos();
                break;
            default:
                printf("\n Opção inválida\n\n");
                system("pause");
        }
    } while (opcao != 0);

}

void menuRECEPCAO() {
    int opcao;

    do {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> MENU RECEPÇÃO <---\n");
        printf(" 1- Cadastrar Paciente\n");
        printf(" 2- Excluir Paciente\n");
        printf(" 3- Ver Pacientes\n");
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
                cadastrarPaciente();
                break;
            case 2:
                excluirPaciente();
                break;
            case 3:
                verPacientes();
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
