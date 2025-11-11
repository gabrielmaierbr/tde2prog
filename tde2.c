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
void credenciais();
void criarUsuarioPadrao();
void lerUsuariosJSON();
char* lerSenhaComMascara();
void menuADM(), menuMEDICO(), menuENFERMEIRO(), menuRECEPCAO();
void gerenciarUsuarios();
void cadastrarUsuario(), cadastrarMedico(), cadastrarEnfermeiro(), cadastrarRecepcionista();
void excluirUsuario(), excluirMedico(), excluirEnfermeiro(), excluirRecepcionista();
void gerenciarPacientes(), cadastrarPaciente(), excluirPaciente(), verPacientes();
void gerenciarLeitos(), criarLeito(), excluirLeito();
void gerenciarPacientesNosLeitos(), alocarPacienteAoLeito(), tirarPacienteDoLeito();
void darAlta();
cJSON* lerArquivoJson(const char* nomeArquivo);
bool cadastrarLogineSenha(Usuario usuarios, char tipo[20]);
bool usuarioExiste(const char* login);


//Variáveis globais
char titulo[50] = "SISTEMA HOSPITALAR";
int totalUsuarios = 0;
Usuario usuariosSistema[50];
Usuario usuarios;
Usuario usuarioLogado;
Paciente pacientesSistema[100];

//----------------------------- Login e Senha -----------------------------

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
        
        totalUsuarios = 1;
        criarUsuarioPadrao();
        printf("Arquivo usuarios.json criado com usuários padrão.\n");
        system("pause");
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

            if (strcmp(login, usuariosSistema[i].login) == 0 && 
                strcmp(senha, usuariosSistema[i].senha) == 0) {
                
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
    FILE *file = fopen("usuarios.json", "r");
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
    fclose(file);
    
    // Converte o texto .json na estrutura da biblioteca cJSON
    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        free(buffer);
        return;
    }
    
    cJSON *usuariosArray = cJSON_GetObjectItem(root, "usuarios");
    if (cJSON_IsArray(usuariosArray)) {
        totalUsuarios = 0;
        cJSON *usuarioItem;
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
    
    cJSON_Delete(root);
    free(buffer);
}

bool usuarioExiste(const char* login) {
    for (int i = 0; i < totalUsuarios; i++) {
        // strcmp retorna 0 se as strings forem idênticas
        if (strcmp(usuariosSistema[i].login, login) == 0) {
            return true; // Encontrou um usuário com este login
        }
    }
    return false; // Não encontrou
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
    char nomeMedico[30], especialidade[20], crm[15], *buffer = NULL, loginMedico[20], senhaMedico[20];
    int idadeMedico, totalMedicos;

    system("cls");
    printf(" [---------- %s -----------]\n\n", titulo);
    printf(" ---> CADASTRAR MÉDICO(A) <---\n");

    // Tenta abrir o arquivo para leitura para ver se ele já existe
    FILE *file = fopen("medicos.json", "rb"); // "rb" para ler em modo binário
    cJSON *root_json = NULL;
    cJSON *medicos_array = NULL;

    if (file == NULL) {
        // Arquivo não existe, vamos criar uma estrutura JSON do zero
        printf("Arquivo 'medicos.json' não encontrado. Criando um novo.\n");
        root_json = cJSON_CreateObject();
        medicos_array = cJSON_CreateArray();
        cJSON_AddItemToObject(root_json, "medicos", medicos_array);
    } else {
        // Arquivo existe, vamos ler e analisar (parse) seu conteúdo
        long tamanho_arquivo;
        fseek(file, 0, SEEK_END);
        tamanho_arquivo = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = (char *)malloc(tamanho_arquivo + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Erro ao alocar memória para ler o arquivo.\n");
            fclose(file);
            return;
        }

        fread(buffer, 1, tamanho_arquivo, file);
        fclose(file);
        buffer[tamanho_arquivo] = '\0';

        root_json = cJSON_Parse(buffer);
        free(buffer);

        if (root_json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            fprintf(stderr, "Erro ao analisar o JSON: %s\n", error_ptr);
            return;
        }

        // Pega o array "medicos" do objeto JSON principal
        medicos_array = cJSON_GetObjectItemCaseSensitive(root_json, "medicos");
        if (!cJSON_IsArray(medicos_array)) {
            fprintf(stderr, "Erro: A chave 'medicos' não é um array no JSON.\n");
            cJSON_Delete(root_json);
            return;
        }
    }

    printf("Quantos Médicos deseja cadastrar? ");
    scanf("%d", &totalMedicos);

    for (int i = 0; i < totalMedicos; i++) {
        printf("\n--- Cadastrando Médico(a) %d de %d ---\n", i + 1, totalMedicos);
        printf("Insira o Nome do Médico(a): ");
        scanf(" %[^\n]", nomeMedico);
        printf("Insira a Idade do Médico(a): ");
        scanf("%d", &idadeMedico);
        printf("Insira a Especialidade do Médico(a): ");
        scanf(" %s", especialidade);
        printf("Insira o CRM do Médico(a): ");
        scanf(" %s", crm);
        printf("Insira o Login do Usuário do Médico(a): ");
        scanf(" %s", usuarios.login);
        printf("Insira a Senha do Usuário do Médico(a): ");
        scanf(" %s", usuarios.senha);

        if (!cadastrarLogineSenha(usuarios, "medico")) {
            printf("\nCadastro de médico cancelado, Usuario duplicado.\n");
            system("pause");
            return;
        } else {
            // Cria um novo objeto JSON para o médico
            cJSON *medico_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(medico_obj, "nome", nomeMedico);
            cJSON_AddNumberToObject(medico_obj, "idade", idadeMedico);
            cJSON_AddStringToObject(medico_obj, "especialidade", especialidade);
            cJSON_AddStringToObject(medico_obj, "crm", crm);

            // Adiciona o objeto do novo médico ao array de médicos
            cJSON_AddItemToArray(medicos_array, medico_obj);

            printf("\nMédico %s cadastrado com sucesso!\n", nomeMedico);
            system("pause");
            system("cls");
            printf(" [-------------- CADASTRAR MÉDICO(A) --------------]\n\n");
        }

        // Converte o objeto cJSON modificado de volta para uma string formatada
        char *json_string_modificado = cJSON_Print(root_json);
        if (json_string_modificado == NULL) {
            fprintf(stderr, "Erro ao gerar a string JSON.\n");
            cJSON_Delete(root_json);
            return;
        }

        // Abre o arquivo em modo de escrita ("w") para sobrescrever com o conteúdo atualizado
        file = fopen("medicos.json", "w");
        if (file == NULL) {
            perror("Erro ao abrir o arquivo para escrita");
            cJSON_Delete(root_json);
            free(json_string_modificado);
            return;
        }

        fputs(json_string_modificado, file);
        fclose(file);

        // Libera a memória alocada pela cJSON
        free(json_string_modificado);
        cJSON_Delete(root_json);

        printf("Todos os médicos foram salvos com sucesso em 'medicos.json'!\n");
        system("pause");
        return;
    }
}

void cadastrarEnfermeiro() {
char nomeEnfermeiro[30], coren[15];
    int idadeEnfermeiro, totalEnfermeiros;

    system("cls");
    printf(" [-------------- CADASTRAR ENFERMEIRO(A) --------------]\n\n");

    // Tenta abrir o arquivo para leitura para ver se ele já existe
    FILE *file = fopen("enfermeiros.json", "rb"); // "rb" para ler em modo binário
    char *buffer = NULL;
    cJSON *root_json = NULL;
    cJSON *enfermeiro_array = NULL;

    if (file == NULL) {
        // Arquivo não existe, vamos criar uma estrutura JSON do zero
        printf("Arquivo 'enfermeiros.json' não encontrado. Criando um novo.\n");
        root_json = cJSON_CreateObject();
        enfermeiro_array = cJSON_CreateArray();
        cJSON_AddItemToObject(root_json, "enfermeiros", enfermeiro_array);
    } else {
        // Arquivo existe, vamos ler e analisar (parse) seu conteúdo
        long tamanho_arquivo;
        fseek(file, 0, SEEK_END);
        tamanho_arquivo = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = (char *)malloc(tamanho_arquivo + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Erro ao alocar memória para ler o arquivo.\n");
            fclose(file);
            return;
        }

        fread(buffer, 1, tamanho_arquivo, file);
        fclose(file);
        buffer[tamanho_arquivo] = '\0';

        root_json = cJSON_Parse(buffer);
        free(buffer);

        if (root_json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            fprintf(stderr, "Erro ao analisar o JSON: %s\n", error_ptr);
            return;
        }

        // Pega o array "medicos" do objeto JSON principal
        enfermeiro_array = cJSON_GetObjectItemCaseSensitive(root_json, "enfermeiros");
        if (!cJSON_IsArray(enfermeiro_array)) {
            fprintf(stderr, "Erro: A chave 'enfermeiros' não é um array no JSON.\n");
            cJSON_Delete(root_json);
            return;
        }
    }

    printf("Quantos Enfermeiros deseja cadastrar? ");
    scanf("%d", &totalEnfermeiros);

    for (int i = 0; i < totalEnfermeiros; i++) {
        printf("\n--- Cadastrando Enfermeiro(a) %d de %d ---\n", i + 1, totalEnfermeiros);
        printf("Insira o Nome do Enfermeiro(a): ");
        scanf(" %[^\n]", nomeEnfermeiro);
        printf("Insira a Idade do Enfermeiro(a): ");
        scanf("%d", idadeEnfermeiro);
        printf("Insira o COREN do Enfermeiro: ");
        scanf(" %s", coren);
        printf("Insira o Login do Usuário do Enfermeiro(a): ");
        scanf(" %s", usuarios.login);
        printf("Insira a Senha do Usuário do Enfermeiro(a): ");
        scanf(" %s", usuarios.senha);
        if (!cadastrarLogineSenha(usuarios, "enfermeiro")){
            printf("\nCadastro de enfermeiro cancelado, Usuario duplicado.\n");
            system("pause");
            return;
        } else {
             // Cria um novo objeto JSON para o enfermeiro
        cJSON *enfermeiro_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(enfermeiro_obj, "nome", nomeEnfermeiro);
        cJSON_AddNumberToObject(enfermeiro_obj, "idade", idadeEnfermeiro);
        cJSON_AddStringToObject(enfermeiro_obj, "coren", coren);

        // Adiciona o objeto do novo enfermeiro ao array de enfermeiro
        cJSON_AddItemToArray(enfermeiro_array, enfermeiro_obj);

        printf("\nEnfermeiro %s cadastrado com sucesso!\n", nomeEnfermeiro);
        system("pause");
        system("cls");
        printf(" [-------------- CADASTRAR ENFERMEIRO(A) --------------]\n\n");
        }
    }

    // Converte o objeto cJSON modificado de volta para uma string formatada
    char *json_string_modificado = cJSON_Print(root_json);
    if (json_string_modificado == NULL) {
        fprintf(stderr, "Erro ao gerar a string JSON.\n");
        cJSON_Delete(root_json);
        return;
    }

    // Abre o arquivo em modo de escrita ("w") para sobrescrever com o conteúdo atualizado
    file = fopen("enfermeiros.json", "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        cJSON_Delete(root_json);
        free(json_string_modificado);
        return;
    }

    fputs(json_string_modificado, file);
    fclose(file);

    // Libera a memória alocada pela cJSON
    free(json_string_modificado);
    cJSON_Delete(root_json);

    printf("Todos os enfermeiros foram salvos com sucesso em 'enfermeiros.json'!\n");
    system("pause");
    return;
}

void cadastrarRecepcionista() {
    char nomeRecepcionista[30], cpf_recepcionista[10];
    int idadeRecepcionista, totalRecepcionistas;

    system("cls");
    printf(" [-------------- CADASTRAR RECEPCIONISTA(A) --------------]\n\n");

    // Tenta abrir o arquivo para leitura para ver se ele já existe
    FILE *file = fopen("recepcionistas.json", "rb"); // "rb" para ler em modo binário
    char *buffer = NULL;
    cJSON *root_json = NULL;
    cJSON *recepcionista_array = NULL;

    if (file == NULL) {
        // Arquivo não existe, vamos criar uma estrutura JSON do zero
        printf("Arquivo 'recepcionistas.json' não encontrado. Criando um novo.\n");
        root_json = cJSON_CreateObject();
        recepcionista_array = cJSON_CreateArray();
        cJSON_AddItemToObject(root_json, "recepcionistas", recepcionista_array);
    } else {
        // Arquivo existe, vamos ler e analisar (parse) seu conteúdo
        long tamanho_arquivo;
        fseek(file, 0, SEEK_END);
        tamanho_arquivo = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = (char *)malloc(tamanho_arquivo + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Erro ao alocar memória para ler o arquivo.\n");
            fclose(file);
            return;
        }

        fread(buffer, 1, tamanho_arquivo, file);
        fclose(file);
        buffer[tamanho_arquivo] = '\0';

        root_json = cJSON_Parse(buffer);
        free(buffer);

        if (root_json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            fprintf(stderr, "Erro ao analisar o JSON: %s\n", error_ptr);
            return;
        }

        // Pega o array "medicos" do objeto JSON principal
        recepcionista_array = cJSON_GetObjectItemCaseSensitive(root_json, "recepcionistas");
        if (!cJSON_IsArray(recepcionista_array)) {
            fprintf(stderr, "Erro: A chave 'recepcionistas' não é um array no JSON.\n");
            cJSON_Delete(root_json);
            return;
        }
    }

    printf("Quantos recepcionistas deseja cadastrar? ");
    scanf("%d", &totalRecepcionistas);

    for (int i = 0; i < totalRecepcionistas; i++) {
        printf("\n--- Cadastrando o (a) Recepcionista %d de %d ---\n", i + 1, totalRecepcionistas);
        printf("Insira o Nome do(a) Recepcionista: ");
        scanf(" %[^\n]", nomeRecepcionista);
        printf("Insira a Idade do(a) Recepcionista: ");
        scanf("%d", idadeRecepcionista);
        printf("Insira o CPF do(a) Recepcionista: ");
        scanf(" %s", cpf_recepcionista);
        printf("Insira o Login do Usuário do(a) Recepcionista: ");
        scanf(" %s", usuarios.login);
        printf("Insira a Senha do Usuário do(a) Recepcionista: ");
        scanf(" %s", usuarios.senha);
        if (!cadastrarLogineSenha(usuarios, "recepcionista")){
            printf("\nCadastro de recepcionista cancelado, Usuario duplicado.\n");
            system("pause");
            return;
        } else {
             // Cria um novo objeto JSON para o Recepcionista
        cJSON *recepcionista_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(recepcionista_obj, "nome", nomeRecepcionista);
        cJSON_AddNumberToObject(recepcionista_obj, "idade", idadeRecepcionista);
        cJSON_AddStringToObject(recepcionista_obj, "CPF", cpf_recepcionista);

        // Adiciona o objeto do novo Recepcionista ao array de Recepcionista
        cJSON_AddItemToArray(recepcionista_array, recepcionista_array);

        printf("\nRecepcionista %s cadastrado com sucesso!\n", nomeRecepcionista);
        system("pause");
        system("cls");
        printf(" [-------------- CADASTRAR RECEPCIONISTA(A) --------------]\n\n");
        }
    }

    // Converte o objeto cJSON modificado de volta para uma string formatada
    char *json_string_modificado = cJSON_Print(root_json);
    if (json_string_modificado == NULL) {
        fprintf(stderr, "Erro ao gerar a string JSON.\n");
        cJSON_Delete(root_json);
        return;
    }

    // Abre o arquivo em modo de escrita ("w") para sobrescrever com o conteúdo atualizado
    file = fopen("recepcionistas.json", "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        cJSON_Delete(root_json);
        free(json_string_modificado);
        return;
    }

    fputs(json_string_modificado, file);
    fclose(file);

    // Libera a memória alocada pela cJSON
    free(json_string_modificado);
    cJSON_Delete(root_json);

    printf("Todos os recepcionistas foram salvos com sucesso em 'enfermeiros.json'!\n");
    system("pause");
    return;

}

bool cadastrarLogineSenha(Usuario usuarios, char *tipo) {

    cJSON *usuario_json =  lerArquivoJson("usuarios.json"); // Carrega os usuários existentes
    cJSON *usuarios_array = cJSON_GetObjectItemCaseSensitive(usuario_json, "usuarios");
    if (!cJSON_IsArray(usuarios_array)) {
        fprintf(stderr, "Erro: A chave 'usuarios' não é um array no JSON.\n");
        cJSON_Delete(usuario_json);
        return FALSE;
    }{   
    if (usuarioExiste(usuarios.login)) {
        printf("Erro: O login '%s' já existe. Escolha outro login.\n", usuarios.login);
        cJSON_Delete(usuario_json);
        return FALSE;
    } else {
        // Continua o processo de cadastro
        // Cria um novo objeto JSON para o usuário
        cJSON *usuario_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(usuario_obj, "login", usuarios.login);
        cJSON_AddStringToObject(usuario_obj, "senha", usuarios.senha);
        cJSON_AddStringToObject(usuario_obj, "tipo", tipo);
        // Adiciona o objeto do novo usuário ao array de usuários
        cJSON_AddItemToArray(usuarios_array, usuario_obj);
        // Converte o objeto cJSON modificado de volta para uma string formatada
        char *json_string_modificado = cJSON_Print(usuario_json);
        if (json_string_modificado == NULL) {
            fprintf(stderr, "Erro ao gerar a string JSON.\n");
            cJSON_Delete(usuario_json);
            return FALSE;
        }
        // Abre o arquivo em modo de escrita ("w") para sobrescrever com o conteúdo atualizado
        FILE *file = fopen("usuarios.json", "w");
        if (file == NULL) {
            perror("Erro ao abrir o arquivo para escrita");
            cJSON_Delete(usuario_json);
            free(json_string_modificado);
            return FALSE;

            fputs(json_string_modificado, file);
            fclose(file);
            // Libera a memória alocada pela cJSON
            free(json_string_modificado);
            cJSON_Delete(usuario_json);
            return TRUE;
            }
        }
    }
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
    // Implementação da função de exclusão de médico, buscando pelo CRM
    char crmBusca[15];
    char *buffer = NULL;

    system("cls");
    printf(" [-------------- EXCLUIR MÉDICO(A) --------------]\n\n");

    printf("Digite o CRM do médico que deseja excluir: ");
    scanf(" %s", crmBusca);

    FILE *file = fopen("medicos.json", "rb");
    if (file == NULL) {
        printf("Nenhum médico cadastrado ainda.\n");
        system("pause");
        return;
    }

    long tamanho_arquivo;
    fseek(file, 0, SEEK_END);
    tamanho_arquivo = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(tamanho_arquivo + 1);
    fread(buffer, 1, tamanho_arquivo, file);
    fclose(file);
    buffer[tamanho_arquivo] = '\0';

    cJSON *root_json = cJSON_Parse(buffer);
    free(buffer);

    if (root_json == NULL) {
        printf("Erro ao ler JSON.\n");
        return;
    }
    // Pega o array "medicos" do objeto JSON principal
    cJSON *medicos_array = cJSON_GetObjectItemCaseSensitive(root_json, "medicos");
    int tamanhoArray = cJSON_GetArraySize(medicos_array);
    int encontrou = 0;
    // Percorre o array para encontrar o médico com o CRM correspondente
    for (int i = 0; i < tamanhoArray; i++) {
        cJSON *medico = cJSON_GetArrayItem(medicos_array, i);
        cJSON *crm = cJSON_GetObjectItemCaseSensitive(medico, "crm");

        if (strcmp(crm->valuestring, crmBusca) == 0) {
            cJSON_DeleteItemFromArray(medicos_array, i);
            encontrou = 1;
            break;
        }
    }

    if (!encontrou) {
        printf("Nenhum médico encontrado com o CRM informado.\n");
        cJSON_Delete(root_json);
        system("pause");
        return;
    }
    // Salva o JSON atualizado de volta no arquivo
    char *jsonAtualizado = cJSON_Print(root_json);
    file = fopen("medicos.json", "w");
    fputs(jsonAtualizado, file);
    fclose(file);

    free(jsonAtualizado);
    cJSON_Delete(root_json);

    printf("Médico removido com sucesso!\n");
    system("pause");
}

void excluirEnfermeiro() {
    char corenBusca[15];
    char *buffer = NULL;

    system("cls");
    printf(" [-------------- EXCLUIR ENFERMEIRO(A) --------------]\n\n");

    printf("Digite o COREN do enfermeiro(a) que deseja excluir: ");
    scanf(" %s", corenBusca);

    FILE *file = fopen("enfermeiros.json", "rb");
    if (file == NULL) {
        printf("Nenhum enfermeiro cadastrado ainda.\n");
        system("pause");
        return;
    }

    long tamanho_arquivo;
    fseek(file, 0, SEEK_END);
    tamanho_arquivo = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(tamanho_arquivo + 1);
    fread(buffer, 1, tamanho_arquivo, file);
    fclose(file);
    buffer[tamanho_arquivo] = '\0';

    cJSON *root_json = cJSON_Parse(buffer);
    free(buffer);

    if (root_json == NULL) {
        printf("Erro ao ler JSON.\n");
        return;
    }

    cJSON *enfermeiros_array = cJSON_GetObjectItemCaseSensitive(root_json, "enfermeiros");
    int tamanhoArray = cJSON_GetArraySize(enfermeiros_array);
    int encontrou = 0;

    for (int i = 0; i < tamanhoArray; i++) {
        cJSON *enfermeiro = cJSON_GetArrayItem(enfermeiros_array, i);
        cJSON *coren = cJSON_GetObjectItemCaseSensitive(enfermeiro, "coren");

        if (strcmp(coren->valuestring, corenBusca) == 0) {
            cJSON_DeleteItemFromArray(enfermeiros_array, i);
            encontrou = 1;
            break;
        }
    }

    if (!encontrou) {
        printf("Nenhum enfermeiro(a) encontrado com o COREN informado.\n");
        cJSON_Delete(root_json);
        system("pause");
        return;
    }

    char *jsonAtualizado = cJSON_Print(root_json);
    file = fopen("enfermeiros.json", "w");
    fputs(jsonAtualizado, file);
    fclose(file);

    free(jsonAtualizado);
    cJSON_Delete(root_json);

    printf("Enfermeiro(a) removido com sucesso!\n");
    system("pause");
}

void excluirRecepcionista() {
    char cpfBusca[20];
    char *buffer = NULL;

    system("cls");
    printf(" [-------------- EXCLUIR RECEPCIONISTA --------------]\n\n");

    printf("Digite o CPF do recepcionista que deseja excluir: ");
    scanf(" %s", cpfBusca);

    FILE *file = fopen("recepcionistas.json", "rb");
    if (file == NULL) {
        printf("Nenhum recepcionista cadastrado ainda.\n");
        system("pause");
        return;
    }

    long tamanho_arquivo;
    fseek(file, 0, SEEK_END);
    tamanho_arquivo = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(tamanho_arquivo + 1);
    fread(buffer, 1, tamanho_arquivo, file);
    fclose(file);
    buffer[tamanho_arquivo] = '\0';

    cJSON *root_json = cJSON_Parse(buffer);
    free(buffer);

    if (root_json == NULL) {
        printf("Erro ao ler JSON.\n");
        return;
    }

    cJSON *recepcionistas_array = cJSON_GetObjectItemCaseSensitive(root_json, "recepcionistas");
    int tamanhoArray = cJSON_GetArraySize(recepcionistas_array);
    int encontrou = 0;

    for (int i = 0; i < tamanhoArray; i++) {
        cJSON *recep = cJSON_GetArrayItem(recepcionistas_array, i);
        cJSON *cpf = cJSON_GetObjectItemCaseSensitive(recep, "cpf");

        if (strcmp(cpf->valuestring, cpfBusca) == 0) {
            cJSON_DeleteItemFromArray(recepcionistas_array, i);
            encontrou = 1;
            break;
        }
    }

    if (!encontrou) {
        printf("Nenhum recepcionista encontrado com o CPF informado.\n");
        cJSON_Delete(root_json);
        system("pause");
        return;
    }

    char *jsonAtualizado = cJSON_Print(root_json);
    file = fopen("recepcionistas.json", "w");
    fputs(jsonAtualizado, file);
    fclose(file);

    free(jsonAtualizado);
    cJSON_Delete(root_json);

    printf("Recepcionista removido(a) com sucesso!\n");
    system("pause");
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

// Função auxiliar para ler um arquivo JSON completo
// Retorna o objeto cJSON raiz ou NULL em caso de erro.
cJSON* lerArquivoJson(const char* nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "rb");
    if (file == NULL) {
        // Retorna NULL se o arquivo não existir, será tratado na função principal
        return NULL; 
    }

    fseek(file, 0, SEEK_END);
    long tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(tamanho + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Erro de alocação de memória para ler %s\n", nomeArquivo);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, tamanho, file);
    fclose(file);
    buffer[tamanho] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        fprintf(stderr, "Erro ao analisar JSON de %s: %s\n", nomeArquivo, error_ptr);
    }
    
    return json;
}


void criarLeito() {
    // --- ETAPA 1: CARREGAR OS MÉDICOS DE medicos.json ---
    cJSON *medicos_json = lerArquivoJson("medicos.json");
    if (medicos_json == NULL) {
        printf("Erro: Arquivo 'medicos.json' não encontrado ou inválido.\n");
        printf("Por favor, cadastre médicos antes de criar leitos.\n");
        system("pause");
        return;
    }

    cJSON *medicos_array_json = cJSON_GetObjectItemCaseSensitive(medicos_json, "medicos");
    if (!cJSON_IsArray(medicos_array_json) || cJSON_GetArraySize(medicos_array_json) == 0) {
        printf("Nenhum médico encontrado em 'medicos.json'.\n");
        printf("Por favor, cadastre médicos antes de criar leitos.\n");
        cJSON_Delete(medicos_json);
        system("pause");
        return;
    }

    int num_medicos = cJSON_GetArraySize(medicos_array_json);
    // Aloca um array de strings para guardar os nomes dos médicos
    char **lista_nomes_medicos = (char **)malloc(num_medicos * sizeof(char *));

    cJSON *medico_item;
    int medico_idx = 0;
    cJSON_ArrayForEach(medico_item, medicos_array_json) {
        cJSON *nome_obj = cJSON_GetObjectItemCaseSensitive(medico_item, "nome");
        if (cJSON_IsString(nome_obj) && (nome_obj->valuestring != NULL)) {
            lista_nomes_medicos[medico_idx] = strdup(nome_obj->valuestring); // strdup aloca e copia
            medico_idx++;
        }
    }

    // --- ETAPA 2: LER/CRIAR O ARQUIVO leitos.json ---
    system("cls");
    printf(" [-------------- CRIAR LEITO --------------]\n\n");

    cJSON *leitos_json = lerArquivoJson("leitos.json");
    cJSON *leitos_array_json;

    if (leitos_json == NULL) {
        printf("Arquivo 'leitos.json' não encontrado. Criando um novo.\n");
        leitos_json = cJSON_CreateObject();
        leitos_array_json = cJSON_CreateArray();
        cJSON_AddItemToObject(leitos_json, "leitos", leitos_array_json);
    } else {
        leitos_array_json = cJSON_GetObjectItemCaseSensitive(leitos_json, "leitos");
        if (!cJSON_IsArray(leitos_array_json)) {
            fprintf(stderr, "Erro: A chave 'leitos' não é um array no JSON.\n");
            cJSON_Delete(medicos_json); // Libera memória dos médicos
            cJSON_Delete(leitos_json);
            return;
        }
    }

    // --- ETAPA 3: LOOP DE CRIAÇÃO DE LEITOS ---
    int totalLeitos;
    printf("Quantos Leitos deseja criar? ");
    scanf("%d", &totalLeitos);

    for (int i = 0; i < totalLeitos; i++) {
        char nomeLeito[20];
        
        printf("\n--- Criando Leito %d de %d ---\n", i + 1, totalLeitos);
        printf("Insira o Nome do Leito (ex: Leito 101): ");
        scanf(" %[^\n]", nomeLeito);

        // Apresenta a lista de médicos para seleção
        printf("\nSelecione o Médico Responsável:\n");
        for (int j = 0; j < num_medicos; j++) {
            printf(" %d - %s\n", j + 1, lista_nomes_medicos[j]);
        }

        int escolha_medico;
        do {
            printf("Escolha uma opção (1 a %d): ", num_medicos);
            scanf("%d", &escolha_medico);
            if (escolha_medico < 1 || escolha_medico > num_medicos) {
                printf("Opção inválida! Tente novamente.\n");
            }
        } while (escolha_medico < 1 || escolha_medico > num_medicos);
        
        // O índice do array é a escolha do usuário - 1
        char *medico_responsavel_escolhido = lista_nomes_medicos[escolha_medico - 1];

        // Cria o objeto JSON para o novo leito
        cJSON *leito_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(leito_obj, "nomeLeito", nomeLeito);
        cJSON_AddBoolToObject(leito_obj, "ocupado", false);
        cJSON_AddStringToObject(leito_obj, "nomePacienteNoLeito", "Nenhum");
        cJSON_AddStringToObject(leito_obj, "medicoResponsavel", medico_responsavel_escolhido);

        cJSON_AddItemToArray(leitos_array_json, leito_obj);

        printf("\nLeito %s criado com sucesso! Médico Responsável: %s\n", nomeLeito, medico_responsavel_escolhido);
        system("pause");
        system("cls");
        printf(" [-------------- CRIAR LEITO --------------]\n\n");
    }

    // --- ETAPA 4: SALVAR AS ALTERAÇÕES EM leitos.json ---
    char *json_string_modificado = cJSON_Print(leitos_json);
    
    FILE* file_saida = fopen("leitos.json", "w");
    if (file_saida == NULL) {
        perror("Erro ao abrir 'leitos.json' para escrita");
    } else {
        fputs(json_string_modificado, file_saida);
        fclose(file_saida);
        printf("Todos os leitos foram salvos com sucesso em 'leitos.json'!\n");
    }

    // --- ETAPA 5: LIBERAR TODA A MEMÓRIA ALOCADA ---
    free(json_string_modificado);
    cJSON_Delete(leitos_json);
    cJSON_Delete(medicos_json);

    // Libera a lista de nomes de médicos
    for (int i = 0; i < num_medicos; i++) {
        free(lista_nomes_medicos[i]);
    }
    free(lista_nomes_medicos);
    
    system("pause");
    return;
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
        printf(" 3- Ver Pacientes");
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
