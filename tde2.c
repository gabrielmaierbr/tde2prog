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
} Medico;

typedef struct {
    char login[20];
    char nome[50];
    int idade;
    char especialidade[20];
    char coren[15];
} Enfermeiro;

typedef struct {
    char login[20];
    char nome[50];
    int idade;
} Recepcionista;

typedef struct {
    char nome[50];
    int idade;
    char sexo[1];
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
void verUsuarios();

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
void menuRECEPCIONISTA();

//Variáveis globais
char titulo[50] = "SISTEMA HOSPITALAR";
int totalUsuarios = 0;
int totalMedicos = 0;
int totalEnfermeiros = 0;
int totalRecepcionistas = 0;
int totalPacientes = 0;
Usuario usuariosSistema[50];
Usuario usuarioLogado;
Medico medicosSistema[50];
Enfermeiro enfermeirosSistema[50];
Recepcionista recepcionistasSistema[50];
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
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" Login: ");
        scanf("%s", login);
        
        // Limpa o buffer do teclado antes de ler a senha
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        printf(" Senha: ");
        senha = lerSenhaComMascara();
        
        for (int i = 0; i < totalUsuarios; i++) {

            if (strcmp(login, usuariosSistema[i].login) == 0 && strcmp(senha, usuariosSistema[i].senha) == 0) {
                
                autenticado = 1;
                usuarioLogado = usuariosSistema[i];
                
                printf("\n Login realizado com sucesso. Bem-vindo, %s!\n\n", login);
                system("pause");
                
                if (strcmp(usuarioLogado.tipo, "admin") == 0) {
                    menuADM();
                } else if (strcmp(usuarioLogado.tipo, "medico") == 0) {
                    menuMEDICO();
                } else if (strcmp(usuarioLogado.tipo, "enfermeiro") == 0) {
                    menuENFERMEIRO();
                } else if (strcmp(usuarioLogado.tipo, "recepcionista") == 0) {
                    menuRECEPCIONISTA();
                }
                break;
            }
        }
        
        if (!autenticado) {
            printf("\n Login ou senha incorretos!\n\n");
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
        FILE *file = fopen("informacoes.json", "w");
        fputs(jsonAtualizado, file);
        fclose(file);
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

    // Procura o array "enfermeiros" dentro do cJSON
    cJSON *enfermeirosArray = cJSON_GetObjectItem(root, "enfermeiros");

    if (enfermeirosArray == NULL) {
        enfermeirosArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "enfermeiros", enfermeirosArray);

        char *jsonAtualizado = cJSON_Print(root);
        FILE *file = fopen("informacoes.json", "w");
        fputs(jsonAtualizado, file);
        fclose(file);
        free(jsonAtualizado);
    } else {
        totalEnfermeiros = 0;
        cJSON *enfermeiroItem;

        // Percorre cada item do array (isso é um for)
        cJSON_ArrayForEach(enfermeiroItem, enfermeirosArray) {
            if (totalEnfermeiros >= 50) break;
            
            cJSON *login = cJSON_GetObjectItem(enfermeiroItem, "login");
            cJSON *nome = cJSON_GetObjectItem(enfermeiroItem, "nome");
            cJSON *idade = cJSON_GetObjectItem(enfermeiroItem, "idade");
            cJSON *especialidade = cJSON_GetObjectItem(enfermeiroItem, "especialidade");
            cJSON *coren = cJSON_GetObjectItem(enfermeiroItem, "coren");
            
            if (cJSON_IsString(login) && cJSON_IsString(nome) && cJSON_IsNumber(idade) && cJSON_IsString(especialidade) && cJSON_IsString(coren)) {
                strcpy(enfermeirosSistema[totalEnfermeiros].login, login->valuestring);
                strcpy(enfermeirosSistema[totalEnfermeiros].nome, nome->valuestring);
                enfermeirosSistema[totalEnfermeiros].idade = idade->valueint;
                strcpy(enfermeirosSistema[totalEnfermeiros].especialidade, especialidade->valuestring);
                strcpy(enfermeirosSistema[totalEnfermeiros].coren, coren->valuestring);
                totalEnfermeiros++;
            }
        }
    }

    // Procura o array "recepcionistas" dentro do cJSON
    cJSON *recepcionistasArray = cJSON_GetObjectItem(root, "recepcionistas");

    if (recepcionistasArray == NULL) {
        recepcionistasArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "recepcionistas", recepcionistasArray);

        char *jsonAtualizado = cJSON_Print(root);
        FILE *file = fopen("informacoes.json", "w");
        fputs(jsonAtualizado, file);
        fclose(file);
        free(jsonAtualizado);
    } else {
        totalRecepcionistas = 0;
        cJSON *recepcionistasItem;

        // Percorre cada item do array (isso é um for)
        cJSON_ArrayForEach(recepcionistasItem, recepcionistasArray) {
            if (totalRecepcionistas >= 50) break;
            
            cJSON *login = cJSON_GetObjectItem(recepcionistasItem, "login");
            cJSON *nome = cJSON_GetObjectItem(recepcionistasItem, "nome");
            cJSON *idade = cJSON_GetObjectItem(recepcionistasItem, "idade");
            
            if (cJSON_IsString(login) && cJSON_IsString(nome) && cJSON_IsNumber(idade)) {
                strcpy(recepcionistasSistema[totalRecepcionistas].login, login->valuestring);
                strcpy(recepcionistasSistema[totalRecepcionistas].nome, nome->valuestring);
                recepcionistasSistema[totalRecepcionistas].idade = idade->valueint;
                totalRecepcionistas++;
            }
        }
    }

    // Procura o array "pacientes" dentro do cJSON
    cJSON *pacientesArray = cJSON_GetObjectItem(root, "pacientes");

    if (pacientesArray == NULL) {
        pacientesArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "pacientes", pacientesArray);

        char *jsonAtualizado = cJSON_Print(root);
        FILE *file = fopen("informacoes.json", "w");
        fputs(jsonAtualizado, file);
        fclose(file);
        free(jsonAtualizado);
    } else {
        totalPacientes = 0;
        cJSON *pacientesItem;

        // Percorre cada item do array (isso é um for)
        cJSON_ArrayForEach(pacientesItem, pacientesArray) {
            if (totalPacientes >= 100) break;
            
            cJSON *nome = cJSON_GetObjectItem(pacientesItem, "nome");
            cJSON *idade = cJSON_GetObjectItem(pacientesItem, "idade");
            cJSON *sexo = cJSON_GetObjectItem(pacientesItem, "sexo");
            cJSON *prioridade = cJSON_GetObjectItem(pacientesItem, "prioridade");
            cJSON *diagnostico = cJSON_GetObjectItem(pacientesItem, "diagnostico");
            
            if (cJSON_IsString(nome) && cJSON_IsNumber(idade) && cJSON_IsString(sexo) && cJSON_IsNumber(prioridade) && cJSON_IsString(diagnostico)) {
                strcpy(pacientesSistema[totalPacientes].nome, nome->valuestring);
                pacientesSistema[totalPacientes].idade = idade->valueint;
                strcpy(pacientesSistema[totalPacientes].sexo, sexo->valuestring);
                pacientesSistema[totalPacientes].prioridade = prioridade->valueint;
                strcpy(pacientesSistema[totalPacientes].diagnostico, diagnostico->valuestring);
                totalPacientes++;
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
        printf(" ---> GERENCIAR USUÁRIO <---\n\n");
        printf(" 1- Cadastrar Usuário\n");
        printf(" 2- Excluir Usuário\n");
        printf(" 3- Ver Usuários\n");
        printf(" 0- Voltar");
        printf("\n\n Digite a opção: ");
        scanf("%d",&opcao);
        
        if (opcao == 1) {
            cadastrarUsuario();
        } else if (opcao == 2) {
            excluirUsuario();
        } else if (opcao == 3) {
            verUsuarios();
        } else if (opcao == 0) {
            break;
        } else {
            printf("\n Opção inválida!\n\n");
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
        printf("Qual usuário deseja cadastrar?\n\n");
        printf(" 1- Médico\n");
        printf(" 2- Enfermeiro\n");
        printf(" 3- Recepcionista\n");
        printf(" 0- Voltar\n");
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
            printf("\n Opção inválida!\n\n");
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

    // Se o arquivo não for encontrado
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
        scanf(" %[^\n]", medicosSistema[totalMedicos].especialidade);
        printf("\nInsira o CRM do Médico(a): ");
        scanf(" %[^\n]", medicosSistema[totalMedicos].crm);

        while (1) {
            int repetido = false;
            printf("\nInsira o Login do Usuário do Médico(a): ");
            scanf(" %[^\n]", medicosSistema[totalMedicos].login);

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
        scanf(" %[^\n]", usuariosSistema[totalUsuarios].senha);
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
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> CADASTRAR ENFERMEIRO(A) <---\n");

    FILE *file = fopen("informacoes.json","r");
    cJSON *root;
    char *buffer;

    // Se o arquivo não for encontrado
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
    cJSON *enfermeirosArray = cJSON_GetObjectItem(root, "enfermeiros");

    int qtd_de_enfermeiros;
    printf("Quantos enfermeiros deseja cadastrar? ");
    scanf("%d",&qtd_de_enfermeiros);

    for (int i=0;i<qtd_de_enfermeiros;i++) {
        system("cls");

        cJSON *novoUsuario = cJSON_CreateObject();
        cJSON *novoEnfermeiro = cJSON_CreateObject();

        printf("\n --- Cadastrando Enfermeiro(a) %d de %d",i+1,qtd_de_enfermeiros);
        printf("\nInsira o Nome do Enfermeiro(a): ");
        scanf(" %[^\n]", enfermeirosSistema[totalEnfermeiros].nome);
        printf("\nInsira a Idade do Enfermeiro(a): ");
        scanf("%d", &enfermeirosSistema[totalEnfermeiros].idade);
        printf("\nInsira a Especialidade do Enfermeiro(a): ");
        scanf(" %[^\n]", enfermeirosSistema[totalEnfermeiros].especialidade);
        printf("\nInsira o Coren do Enfermeiro(a): ");
        scanf(" %[^\n]", enfermeirosSistema[totalEnfermeiros].coren);

        while (1) {
            int repetido = false;
            printf("\nInsira o Login do Usuário do Enfermeiro(a): ");
            scanf(" %[^\n]", enfermeirosSistema[totalEnfermeiros].login);

            for (int i=0;i<totalUsuarios;i++) {
                if (strcmp(enfermeirosSistema[totalEnfermeiros].login, usuariosSistema[i].login) == 0) {
                    repetido = true;
                    printf("Este login já existe!\n");
                    break;
                }
            }
            if (repetido == false) {
                break;
            }
        }

        strcpy(usuariosSistema[totalUsuarios].login, enfermeirosSistema[totalEnfermeiros].login);
        printf("\nInsira a Senha do Usuário do Médico(a): ");
        scanf(" %[^\n]", usuariosSistema[totalUsuarios].senha);
        strcpy(usuariosSistema[totalUsuarios].tipo, "enfermeiro");

        // Objeto do Enfermeiro
        cJSON_AddStringToObject(novoEnfermeiro, "login", enfermeirosSistema[totalEnfermeiros].login);
        cJSON_AddStringToObject(novoEnfermeiro, "nome", enfermeirosSistema[totalEnfermeiros].nome);
        cJSON_AddNumberToObject(novoEnfermeiro, "idade", enfermeirosSistema[totalEnfermeiros].idade);
        cJSON_AddStringToObject(novoEnfermeiro, "especialidade", enfermeirosSistema[totalEnfermeiros].especialidade);
        cJSON_AddStringToObject(novoEnfermeiro, "coren", enfermeirosSistema[totalEnfermeiros].coren);

        // Objeto do Usuário
        cJSON_AddStringToObject(novoUsuario, "login", usuariosSistema[totalUsuarios].login);
        cJSON_AddStringToObject(novoUsuario, "senha", usuariosSistema[totalUsuarios].senha);
        cJSON_AddStringToObject(novoUsuario, "tipo", usuariosSistema[totalUsuarios].tipo);

        // Adicionando os objetos ao JSON
        cJSON_AddItemToArray(usuariosArray, novoUsuario);
        cJSON_AddItemToArray(enfermeirosArray, novoEnfermeiro);

        printf("\nEnfermeiro %s cadastrado com sucesso!\n\n", enfermeirosSistema[totalEnfermeiros].nome);
        system("pause");

        totalUsuarios++;
        totalEnfermeiros++;

    }

    // Salvar tudo no json
    FILE *fileOut = fopen("informacoes.json","w");
    char *jsonAtualizado = cJSON_Print(root);
    fprintf(fileOut, "%s", jsonAtualizado);

    fclose(fileOut);
    free(jsonAtualizado);
    cJSON_Delete(root);

}

void cadastrarRecepcionista() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> CADASTRAR RECEPCIONISTA <---\n");

    FILE *file = fopen("informacoes.json","r");
    cJSON *root;
    char *buffer;

    // Se o arquivo não for encontrado
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
    cJSON *recepcionistasArray = cJSON_GetObjectItem(root, "recepcionistas");

    int qtd_de_recepcionistas;
    printf("Quantos recepcionistas deseja cadastrar? ");
    scanf("%d",&qtd_de_recepcionistas);

    for (int i=0;i<qtd_de_recepcionistas;i++) {
        system("cls");

        cJSON *novoUsuario = cJSON_CreateObject();
        cJSON *novoRecepcionista = cJSON_CreateObject();

        printf("\n --- Cadastrando Recepcionista %d de %d",i+1,qtd_de_recepcionistas);
        printf("\nInsira o Nome do Recepcionista: ");
        scanf(" %[^\n]", recepcionistasSistema[totalRecepcionistas].nome);
        printf("\nInsira a Idade do Recepcionista: ");
        scanf("%d", &recepcionistasSistema[totalRecepcionistas].idade);

        while (1) {
            int repetido = false;
            printf("\nInsira o Login do Usuário do Recepcionista: ");
            scanf(" %[^\n]", recepcionistasSistema[totalRecepcionistas].login);

            for (int i=0;i<totalUsuarios;i++) {
                if (strcmp(recepcionistasSistema[totalRecepcionistas].login, usuariosSistema[i].login) == 0) {
                    repetido = true;
                    printf("Este login já existe!\n");
                    break;
                }
            }
            if (repetido == false) {
                break;
            }
        }

        strcpy(usuariosSistema[totalUsuarios].login, recepcionistasSistema[totalRecepcionistas].login);
        printf("\nInsira a Senha do Usuário do Recepcionista: ");
        scanf(" %[^\n]", usuariosSistema[totalUsuarios].senha);
        strcpy(usuariosSistema[totalUsuarios].tipo, "recepcionista");

        // Objeto do Recepcionista
        cJSON_AddStringToObject(novoRecepcionista, "login", recepcionistasSistema[totalRecepcionistas].login);
        cJSON_AddStringToObject(novoRecepcionista, "nome", recepcionistasSistema[totalRecepcionistas].nome);
        cJSON_AddNumberToObject(novoRecepcionista, "idade", recepcionistasSistema[totalRecepcionistas].idade);

        // Objeto do Usuário
        cJSON_AddStringToObject(novoUsuario, "login", usuariosSistema[totalUsuarios].login);
        cJSON_AddStringToObject(novoUsuario, "senha", usuariosSistema[totalUsuarios].senha);
        cJSON_AddStringToObject(novoUsuario, "tipo", usuariosSistema[totalUsuarios].tipo);

        // Adicionando os objetos ao JSON
        cJSON_AddItemToArray(usuariosArray, novoUsuario);
        cJSON_AddItemToArray(recepcionistasArray, novoRecepcionista);

        printf("\nRecepcionista %s cadastrado com sucesso!\n\n", recepcionistasSistema[totalRecepcionistas].nome);
        system("pause");

        totalUsuarios++;
        totalRecepcionistas++;

    }

    // Salvar tudo no json
    FILE *fileOut = fopen("informacoes.json","w");
    char *jsonAtualizado = cJSON_Print(root);
    fprintf(fileOut, "%s", jsonAtualizado);

    fclose(fileOut);
    free(jsonAtualizado);
    cJSON_Delete(root);

}

void excluirUsuario() {
    int opcao;
    while(1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR USUÁRIO <---\n");
        printf("Qual usuário deseja excluir?\n\n");
        printf(" 1- Médico\n");
        printf(" 2- Enfermeiro\n");
        printf(" 3- Recepcionista\n");
        printf(" 0- Voltar\n");
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
            printf("\n Opção inválida!\n\n");
            system("pause");
        }
    }
}

void excluirMedico() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> EXCLUIR MÉDICO(A) <---\n");

    // Verifica se há médicos cadastrados
    if (totalMedicos == 0) {
        printf("\nNão há médicos cadastrados.\n\n");
        system("pause");
        return;
    }

    // Pergunta qual médico quer que exclua
    int indiceExcluir;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR MÉDICO(A) <---\n");
        printf("Qual médico(a) deseja excluir [0 para voltar]?\n\n");
        for (int i=0;i<totalMedicos;i++) {
            printf("%d. %s\n",i+1,medicosSistema[i].nome);
        }
        printf("Escolha: ");
        scanf("%d",&indiceExcluir);
        if (indiceExcluir>=1 && indiceExcluir<=totalMedicos) {
            break;
        }
        if (indiceExcluir == 0) {
            return;
        }
        printf("\nOpção inválida!\n\n");
        system("pause");
        system("cls");
    }
    char loginExcluir[20];
    strcpy(loginExcluir, medicosSistema[indiceExcluir-1].login);

    // Remove o médico da struct medicosSistema
    for (int i=indiceExcluir-1;i<totalMedicos-1;i++) {
        medicosSistema[i] = medicosSistema[i+1];
    }
    totalMedicos--;
    medicosSistema[totalMedicos] = (Medico){0};

    // Acha o índice do médico na struct
    int usuarioEncontrado;
    for (int i=0; i<totalUsuarios;i++) {
        if (strcmp(usuariosSistema[i].login,loginExcluir) == 0) {
            usuarioEncontrado = i;
        }
    }

    // Remove o médico da struct usuariosSistema
    for (int i=usuarioEncontrado;i<totalUsuarios-1;i++) {
        usuariosSistema[i] = usuariosSistema[i+1];
    }
    totalUsuarios--;
    usuariosSistema[totalUsuarios] = (Usuario){0};

    // Atualização o JSON
    FILE *file = fopen("informacoes.json","r");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON.\n\n");
        system("pause");
        return;
    }

    // Descobre o tamanho do arquivo e põe na variável "tamanho"
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Coloca no ponteiro "buffer"
    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';
    fclose(file);

    // transofrma no formato JSON
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        printf("Erro ao interpretar o arquivo JSON.\n\n");
        return;
    }

    // Procura os arrays "medicos" e "usuarios"
    cJSON *medicosArray = cJSON_GetObjectItem(root, "medicos");
    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");

    // Remove do array de médicos
    int tamanhoMedicos = cJSON_GetArraySize(medicosArray);
    for (int i = 0; i < tamanhoMedicos; i++) {
        cJSON *medicoItem = cJSON_GetArrayItem(medicosArray, i);
        cJSON *login = cJSON_GetObjectItem(medicoItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(medicosArray, i);
            break;
        }
    }

    // Remove do array de usuários
    int tamanhoUsuarios = cJSON_GetArraySize(usuariosArray);
    for (int i = 0; i < tamanhoUsuarios; i++) {
        cJSON *usuarioItem = cJSON_GetArrayItem(usuariosArray, i);
        cJSON *login = cJSON_GetObjectItem(usuarioItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(usuariosArray, i);
            break;
        }
    }

    // Salva no arquivo
    file = fopen("informacoes.json", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON para escrita.\n\n");
        cJSON_Delete(root);
        return;
    }

    char *jsonAtualizado = cJSON_Print(root);
    fprintf(file, "%s", jsonAtualizado);
    fclose(file);
    free(jsonAtualizado);
    cJSON_Delete(root);

    printf("\nMédico '%s' excluído com sucesso!\n\n", loginExcluir);
    system("pause");

}

void excluirEnfermeiro() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> EXCLUIR ENFERMEIRO(A) <---\n");

    // Verifica se há enfermeiros cadastrados
    if (totalEnfermeiros == 0) {
        printf("\nNão há enfermeiros cadastrados.\n\n");
        system("pause");
        return;
    }

    // Pergunta qual enfermeiro quer que exclua
    int indiceExcluir;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR ENFERMEIRO(A) <---\n");
        printf("Qual enfermeiro(a) deseja excluir [0 para voltar]?\n\n");
        for (int i=0;i<totalEnfermeiros;i++) {
            printf("%d. %s\n",i+1,enfermeirosSistema[i].nome);
        }
        printf("Escolha: ");
        scanf("%d",&indiceExcluir);
        if (indiceExcluir>=1 && indiceExcluir<=totalEnfermeiros) {
            break;
        }
        if (indiceExcluir == 0) {
            return;
        }
        printf("\nOpção inválida!\n\n");
        system("pause");
        system("cls");
    }
    char loginExcluir[20];
    strcpy(loginExcluir, enfermeirosSistema[indiceExcluir-1].login);

    // Remove o enfermeiro da struct enfermeirosSistema
    for (int i=indiceExcluir-1;i<totalEnfermeiros-1;i++) {
        enfermeirosSistema[i] = enfermeirosSistema[i+1];
    }
    totalEnfermeiros--;
    enfermeirosSistema[totalEnfermeiros] = (Enfermeiro){0};

    // Acha o índice do enfermeiro na struct
    int usuarioEncontrado;
    for (int i=0; i<totalUsuarios;i++) {
        if (strcmp(usuariosSistema[i].login,loginExcluir) == 0) {
            usuarioEncontrado = i;
        }
    }

    // Remove o enfermeiro da struct usuariosSistema
    for (int i=usuarioEncontrado;i<totalUsuarios-1;i++) {
        usuariosSistema[i] = usuariosSistema[i+1];
    }
    totalUsuarios--;
    usuariosSistema[totalUsuarios] = (Usuario){0};

    // Atualização o JSON
    FILE *file = fopen("informacoes.json","r");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON.\n\n");
        system("pause");
        return;
    }

    // Descobre o tamanho do arquivo e põe na variável "tamanho"
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Coloca no ponteiro "buffer"
    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';
    fclose(file);

    // transofrma no formato JSON
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        printf("Erro ao interpretar o arquivo JSON.\n\n");
        return;
    }

    // Procura os arrays "enfermeiros" e "usuarios"
    cJSON *enfermeirosArray = cJSON_GetObjectItem(root, "enfermeiros");
    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");

    // Remove do array de enfermeiros
    int tamanhoEnfermeiros = cJSON_GetArraySize(enfermeirosArray);
    for (int i = 0; i < tamanhoEnfermeiros; i++) {
        cJSON *enfermeirosItem = cJSON_GetArrayItem(enfermeirosArray, i);
        cJSON *login = cJSON_GetObjectItem(enfermeirosItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(enfermeirosArray, i);
            break;
        }
    }

    // Remove do array de usuários
    int tamanhoUsuarios = cJSON_GetArraySize(usuariosArray);
    for (int i = 0; i < tamanhoUsuarios; i++) {
        cJSON *usuarioItem = cJSON_GetArrayItem(usuariosArray, i);
        cJSON *login = cJSON_GetObjectItem(usuarioItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(usuariosArray, i);
            break;
        }
    }

    // Salva no arquivo
    file = fopen("informacoes.json", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON para escrita.\n\n");
        cJSON_Delete(root);
        return;
    }

    char *jsonAtualizado = cJSON_Print(root);
    fprintf(file, "%s", jsonAtualizado);
    fclose(file);
    free(jsonAtualizado);
    cJSON_Delete(root);

    printf("\nEnfermeiro(a) '%s' excluído(a) com sucesso!\n\n", loginExcluir);
    system("pause");

}

void excluirRecepcionista() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> EXCLUIR RECEPCIONISTA <---\n");

    // Verifica se há recepcionistas cadastrados
    if (totalRecepcionistas == 0) {
        printf("\nNão há recepcionistas cadastrados.\n\n");
        system("pause");
        return;
    }

    // Pergunta qual recepcionista quer que exclua
    int indiceExcluir;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR RECEPCIONISTA <---\n");
        printf("Qual recepcionista deseja excluir [0 para voltar]?\n\n");
        for (int i=0;i<totalRecepcionistas;i++) {
            printf("%d. %s\n",i+1,recepcionistasSistema[i].nome);
        }
        printf("Escolha: ");
        scanf("%d",&indiceExcluir);
        if (indiceExcluir>=1 && indiceExcluir<=totalRecepcionistas) {
            break;
        }
        if (indiceExcluir == 0) {
            return;
        }
        printf("\nOpção inválida!\n\n");
        system("pause");
        system("cls");
    }
    char loginExcluir[20];
    strcpy(loginExcluir, recepcionistasSistema[indiceExcluir-1].login);

    // Remove o recepcionista da struct recepcionistasSistema
    for (int i=indiceExcluir-1;i<totalRecepcionistas-1;i++) {
        recepcionistasSistema[i] = recepcionistasSistema[i+1];
    }
    totalRecepcionistas--;
    recepcionistasSistema[totalRecepcionistas] = (Recepcionista){0};

    // Acha o índice da recepcionista na struct
    int usuarioEncontrado;
    for (int i=0; i<totalUsuarios;i++) {
        if (strcmp(usuariosSistema[i].login,loginExcluir) == 0) {
            usuarioEncontrado = i;
        }
    }

    // Remove o recepcionista da struct usuariosSistema
    for (int i=usuarioEncontrado;i<totalUsuarios-1;i++) {
        usuariosSistema[i] = usuariosSistema[i+1];
    }
    totalUsuarios--;
    usuariosSistema[totalUsuarios] = (Usuario){0};

    // Atualização o JSON
    FILE *file = fopen("informacoes.json","r");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON.\n\n");
        system("pause");
        return;
    }

    // Descobre o tamanho do arquivo e põe na variável "tamanho"
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Coloca no ponteiro "buffer"
    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';
    fclose(file);

    // transofrma no formato JSON
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        printf("Erro ao interpretar o arquivo JSON.\n\n");
        return;
    }

    // Procura os arrays "recepcionistas" e "usuarios"
    cJSON *recepcionistasArray = cJSON_GetObjectItem(root, "recepcionistas");
    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");

    // Remove do array de recepcionistas
    int tamanhoRecepcionistas = cJSON_GetArraySize(recepcionistasArray);
    for (int i = 0; i < tamanhoRecepcionistas; i++) {
        cJSON *recepcionistasItem = cJSON_GetArrayItem(recepcionistasArray, i);
        cJSON *login = cJSON_GetObjectItem(recepcionistasItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(recepcionistasArray, i);
            break;
        }
    }

    // Remove do array de usuários
    int tamanhoUsuarios = cJSON_GetArraySize(usuariosArray);
    for (int i = 0; i < tamanhoUsuarios; i++) {
        cJSON *usuarioItem = cJSON_GetArrayItem(usuariosArray, i);
        cJSON *login = cJSON_GetObjectItem(usuarioItem, "login");
        if (cJSON_IsString(login) && strcmp(login->valuestring, loginExcluir) == 0) {
            cJSON_DeleteItemFromArray(usuariosArray, i);
            break;
        }
    }

    // Salva no arquivo
    file = fopen("informacoes.json", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON para escrita.\n\n");
        cJSON_Delete(root);
        return;
    }

    char *jsonAtualizado = cJSON_Print(root);
    fprintf(file, "%s", jsonAtualizado);
    fclose(file);
    free(jsonAtualizado);
    cJSON_Delete(root);

    printf("\nRecepcionista '%s' excluído com sucesso!\n\n", loginExcluir);
    system("pause");
}

void verUsuarios() {
    int escolhaLogin;
    while(1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> VER DADOS DE UM USUÁRIO <---\n");
        printf("Escolha um usuário para ver suas informações [0 para voltar]: \n\n");
        for (int i=1;i<totalUsuarios;i++) {
            printf("%d. %s\n", i, usuariosSistema[i].login);
        }
        printf("\nEscolha: ");
        scanf("%d",&escolhaLogin);

        if (escolhaLogin>=1 && escolhaLogin<=totalUsuarios-1) {
            break;
        }
        if (escolhaLogin == 0) {
            return;
        }

        printf("\nOpção inválida!\n\n");
        system("pause");
    }

    char visualizarLogin[20];
    char visualizarTipo[20];
    strcpy(visualizarLogin, usuariosSistema[escolhaLogin].login);
    strcpy(visualizarTipo, usuariosSistema[escolhaLogin].tipo);

    int indiceVisualizar;
    if (strcmp(visualizarTipo, "medico") == 0) {
        for (int i=0;i<totalMedicos;i++) {
            if (strcmp(visualizarLogin, medicosSistema[i].login) == 0)  {
                indiceVisualizar = i;
                break;
            }
        }

        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> VER DADOS DE UM USUÁRIO <---\n");
        printf(" - Dados do usuário %s:\n", medicosSistema[indiceVisualizar].login);
        printf("Nome         : %s\n",medicosSistema[indiceVisualizar].nome);
        printf("Idade        : %d\n",medicosSistema[indiceVisualizar].idade);
        printf("Especialidade: %s\n",medicosSistema[indiceVisualizar].especialidade);
        printf("CRM          : %s\n\n",medicosSistema[indiceVisualizar].crm);
    } else if (strcmp(visualizarTipo, "enfermeiro") == 0) {
        for (int i=0;i<totalEnfermeiros;i++) {
            if (strcmp(visualizarLogin, enfermeirosSistema[i].login) == 0) {
                indiceVisualizar = i;
                break;
            }
        }

        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> VER DADOS DE UM USUÁRIO <---\n");
        printf(" - Dados do usuário %s:\n", enfermeirosSistema[indiceVisualizar].login);
        printf("Nome         : %s\n",enfermeirosSistema[indiceVisualizar].nome);
        printf("Idade        : %d\n",enfermeirosSistema[indiceVisualizar].idade);
        printf("Especialidade: %s\n",enfermeirosSistema[indiceVisualizar].especialidade);
        printf("CRM          : %s\n\n",enfermeirosSistema[indiceVisualizar].coren);
    } else if (strcmp(visualizarTipo, "recepcionista") == 0) {
        for (int i=0;i<totalRecepcionistas;i++) {
            if (strcmp(visualizarLogin, recepcionistasSistema[i].login) == 0) {
                indiceVisualizar = i;
                break;
            }
        }
        
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> VER DADOS DE UM USUÁRIO <---\n");
        printf(" - Dados do usuário %s:\n", recepcionistasSistema[indiceVisualizar].login);
        printf("Nome         : %s\n",recepcionistasSistema[indiceVisualizar].nome);
        printf("Idade        : %d\n\n",recepcionistasSistema[indiceVisualizar].idade);
    }
    
    system("pause");

}
//--------------------------- Gerenciar Pacientes ---------------------------

void gerenciarPacientes() {
    int opcao;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> GERENCIAR PACIENTES <---\n\n");
        printf(" 1- Cadastrar Paciente\n");
        printf(" 2- Excluir Paciente\n");
        printf(" 3- Ver Pacientes\n");
        printf(" 0- Voltar\n");
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
            printf("\n Opção Inválida!\n\n");
            system("pause");
        }
    }
}

void cadastrarPaciente() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> CADASTRAR PACIENTE <---\n");

    FILE *file = fopen("informacoes.json","r");
    cJSON *root;
    char *buffer;

    // Se o arquivo não for encontrado
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

    cJSON *pacientesArray = cJSON_GetObjectItem(root, "pacientes");

    int qtd_de_pacientes;
    printf("Quantos pacientes deseja cadastrar? ");
    scanf("%d",&qtd_de_pacientes);

    for (int i=0;i<qtd_de_pacientes;i++) {
        system("cls");

        cJSON *novoPaciente = cJSON_CreateObject();

        printf("\n --- Cadastrando Paciente %d de %d",i+1,qtd_de_pacientes);
        printf("\nInsira o Nome do Paciente: ");
        scanf(" %[^\n]", pacientesSistema[totalPacientes].nome);
        printf("\nInsira a Idade do Paciente: ");
        scanf("%d", &pacientesSistema[totalPacientes].idade);

        char temp[20];
        while (1) {
            printf("\nInsira o sexo do Paciente: ");
            scanf(" %s", temp);
            if (strcmp(temp,"m") == 0 || strcmp(temp,"M") == 0 || strcmp(temp,"f") == 0 || strcmp(temp,"F") == 0) {
                strcpy(pacientesSistema[totalPacientes].sexo, temp);
                break;
            }
            printf("\nOpção inválida! Só é possível \'m\' ou \'f\'.\n\n");
            system("pause");
        }

        int x;
        while (1) {
            printf("\nInsira o grau de prioridade do Paciente [1 a 5]: ");
            scanf("%d", &x);
            if (x>=1 && x<=5) {
                pacientesSistema[totalPacientes].prioridade = x;
                break;
            }
            printf("\nOpção inválida! Só é possível de 1 a 5\n\n");
            system("pause");
        }
        
        printf("\nEscreva o diagnóstico do Paciente: ");
        scanf(" %[^\n]", pacientesSistema[totalPacientes].diagnostico);

        // Objeto do Paciente
        cJSON_AddStringToObject(novoPaciente, "nome", pacientesSistema[totalPacientes].nome);
        cJSON_AddNumberToObject(novoPaciente, "idade", pacientesSistema[totalPacientes].idade);
        cJSON_AddStringToObject(novoPaciente, "sexo", pacientesSistema[totalPacientes].sexo);
        cJSON_AddNumberToObject(novoPaciente, "prioridade", pacientesSistema[totalPacientes].prioridade);
        cJSON_AddStringToObject(novoPaciente, "diagnostico", pacientesSistema[totalPacientes].diagnostico);

        // Adicionando o objeto ao JSON
        cJSON_AddItemToArray(pacientesArray, novoPaciente);

        printf("\nPaciente %s cadastrado com sucesso!\n\n", pacientesSistema[totalPacientes].nome);
        system("pause");

        totalPacientes++;

    }

    // Salvar tudo no json
    FILE *fileOut = fopen("informacoes.json","w");
    char *jsonAtualizado = cJSON_Print(root);
    fprintf(fileOut, "%s", jsonAtualizado);

    fclose(fileOut);
    free(jsonAtualizado);
    cJSON_Delete(root);

}

void excluirPaciente() {
    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> EXCLUIR PACIENTE <---\n");

    // Verifica se há pacientes cadastrados
    if (totalPacientes == 0) {
        printf("\nNão há pacientes cadastrados.\n\n");
        system("pause");
        return;
    }

    // Pergunta qual paciente quer que exclua
    int indiceExcluir;
    while (1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> EXCLUIR PACIENTE <---\n");
        printf("Qual paciente deseja excluir [0 para voltar]?\n\n");
        for (int i=0;i<totalPacientes;i++) {
            printf("%d. %s\n",i+1,pacientesSistema[i].nome);
        }
        printf("Escolha: ");
        scanf("%d",&indiceExcluir);
        if (indiceExcluir>=1 && indiceExcluir<=totalPacientes) {
            break;
        }
        if (indiceExcluir == 0) {
            return;
        }
        printf("\nOpção inválida!\n\n");
        system("pause");
        system("cls");
    }
    indiceExcluir -= 1;
    char pacienteExcluir[50];
    strcpy(pacienteExcluir,pacientesSistema[indiceExcluir].nome);

    // Remove o paciente da struct pacientesSistema
    for (int i=indiceExcluir;i<totalPacientes-1;i++) {
        pacientesSistema[i] = pacientesSistema[i+1];
    }
    totalPacientes--;
    pacientesSistema[totalPacientes] = (Paciente){0};

    // Atualização o JSON
    FILE *file = fopen("informacoes.json","r");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON.\n\n");
        system("pause");
        return;
    }

    // Descobre o tamanho do arquivo e põe na variável "tamanho"
    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Coloca no ponteiro "buffer"
    char *buffer = malloc(tamanho + 1);
    fread(buffer, 1, tamanho, file);
    buffer[tamanho] = '\0';
    fclose(file);

    // transofrma no formato JSON
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        printf("Erro ao interpretar o arquivo JSON.\n\n");
        return;
    }

    // Procura o array "pacientes"
    cJSON *pacientesArray = cJSON_GetObjectItem(root, "pacientes");
    cJSON_DeleteItemFromArray(pacientesArray, indiceExcluir);

    // Salva no arquivo
    file = fopen("informacoes.json", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo JSON para escrita.\n\n");
        cJSON_Delete(root);
        return;
    }

    char *jsonAtualizado = cJSON_Print(root);
    fprintf(file, "%s", jsonAtualizado);
    fclose(file);
    free(jsonAtualizado);
    cJSON_Delete(root);

    printf("\nPaciente '%s' excluído com sucesso!\n\n", pacienteExcluir);
    system("pause");
}

void verPacientes() {

    if (totalPacientes == 0) {
    printf("\nNão há pacientes cadastrados.\n\n");
    system("pause");
    return;
    }
    
    int escolhaPaciente;
    while(1) {
        system("cls");
        printf(" [---------- %s -----------]\n\n", titulo);
        printf(" ---> VER DADOS DE UM PACIENTE <---\n");
        printf("Escolha um paciente para ver suas informações [0 para voltar]: \n\n");
        for (int i=0;i<totalPacientes;i++) {
            printf("%d. %s\n", i+1, pacientesSistema[i].nome);
        }
        printf("\nEscolha: ");
        scanf("%d",&escolhaPaciente);

        if (escolhaPaciente>=1 && escolhaPaciente<=totalPacientes) {
            break;
        }
        if (escolhaPaciente == 0) {
            return;
        }

        printf("\nOpção inválida!\n\n");
        system("pause");
    }

    escolhaPaciente -= 1;

    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> VER DADOS DE UM PACIENTE <---\n");
    printf(" - Dados do paciente %s:\n", pacientesSistema[escolhaPaciente].nome);
    printf("Idade        : %d\n",pacientesSistema[escolhaPaciente].idade);
    printf("Sexo         : %s\n",pacientesSistema[escolhaPaciente].sexo);
    printf("Prioridade   : %d\n",pacientesSistema[escolhaPaciente].prioridade);
    printf("Diagnóstico  : %s\n\n",pacientesSistema[escolhaPaciente].diagnostico);

    system("pause");
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
            printf("\n Opção Inválida!\n\n");
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
            printf(" \nOpção Inválida!\n\n");
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
        printf(" ---> MENU ADMINISTRADOR <---\n\n");
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
                printf("\n Opção inválida!\n\n");
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
                printf("\n Opção inválida!\n\n");
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
                printf("\n Opção inválida!\n\n");
                system("pause");
        }
    } while (opcao != 0);

}

void menuRECEPCIONISTA() {
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
                printf("\n Opção inválida!\n\n");
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
