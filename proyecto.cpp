#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

void header(){
    char cwd[1024];
    char *user;
    user = getenv("USER");
    
    getcwd(cwd, sizeof(cwd));
    cout << user << "@" << "computer" << ":" << cwd << "$ ";
}

int contarEspacios(char *cadena){
    int retorno = 0;
    for (int i = 0; i < strlen(cadena); i++){
        if (cadena[i] == ' '){
            retorno += 1;
        }
    }
    return retorno;
}

void executeSimpleCommand(char** args){
    pid_t pidt = fork();
    if(pidt == 0){
        int exito = execvp(args[0],args);
        if (exito < 0){
            perror("No se ejecuto el comando");
        }
    }else{
        wait(NULL);
        return;
    }
}

int contarPipes(char** args, int size){
    int cont = 0;
    for(size_t i = 0; i < size; i++){
        if(!strcmp(args[i], "|")){
            cont++;            
        }
    }
    
    return cont;
}

char*** dosComandos(char** args, int size){
    char* arg1[100];
    char* arg2[100];

    for(int i; i < 100; i++){
        arg1[i] = NULL;
        arg2[i] = NULL;
    }

    char** argsO[2];
    argsO[0] = arg1;
    argsO[1] = arg2;

    char*** argsOut = argsO;

    bool pie = false;
    int cont = 0;

    for(size_t i = 0; i < size; i++)
    {
        if(pie){
            argsOut[1][cont] = args[i];
            cont++;
        }else{
            if(!strcmp(args[i], "|")){
                pie = true;
                argsOut[0][i] = NULL;
            }else{
                argsOut[0][i] = args[i];
            }
        }
    }
    argsOut[1][cont] = NULL;    
    return argsOut;
}

void processSimplePipe(char** args, char** args2) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0){
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }else{
        const int buffer_size = 1024;
        char buffer[buffer_size];

        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }

        if(fork() == 0){
            //execvp(args2[0], args2, const_cast<char *>(buffer));
            execlp(args2[0], args2[0], buffer, 0);
            //execlp("cowsay", "cowsay", "ola", 0);
            perror("execlp de args2 falló");
        }
        
        wait(0);
        wait(0);
    }
}

void processComplexPipe(char **args, char **args2)
{
    int conts = 0;

    try{
        while(args2[conts] != NULL){
            conts++;
            cout << "cont -> " << conts << endl;                                                                        
        }
    }catch(...){
        perror("Error contango argumentos agrs2");
    }

    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0){
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }else{
        const int buffer_size = 1024;
        char buffer[buffer_size];

        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }

        args2[conts] = buffer;
        args2[conts + 1] = NULL;

        if(fork() == 0){
            //execvp(args2[0], args2, const_cast<char *>(buffer));             
            execvp(args2[0], args2);
            //execlp("cowsay", "cowsay", "ola", 0);
            perror("execlp de args2 falló");
        }
        
        wait(0);
        wait(0);
    }
}

void executeSimplePipe(char*** args){
    int des_p[2];
    char** args1 = args[0];
    char** args2 = args[1];

    processComplexPipe(args1, args2);
}

char* processToBuffer(char **args)
{
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0)
    {
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        cout << "Procesando a buffer: " << args[0] << endl;
        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }
    else
    {
        const int buffer_size = 1024;
        char buffer[buffer_size];

        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
        {
        }

        cout << "Contenido del bufer:" << endl << buffer << endl <<  endl;

        wait(0);
        return buffer;
    }

    return 0;
}

void executeMultiplePipe(char*** args, int size){
    int des_p[2];
    char* out;

    out = processToBuffer(args[0]);

    for(int i = 0; i < size; i++)
    {
        char* argsIn[2];
        argsIn[0] = args[i][0];
        argsIn[1] = out;

        //cout << "Out right now: " << out;

        if (i < size - 1) {
            out = processToBuffer(argsIn);
        }else{
            executeSimpleCommand(argsIn);
        }        
    }
}


void interpretCmd(){
    char *buffer;
    buffer = readline("");
    string historia = buffer;

    //cout << historia << "\n";
    //cout<<"lo leyo bien: "<<buffer<<"\n";

    bool seguir = true;
    char *args[100];
    int posActual = 0;
    int espacios = contarEspacios(buffer);
    //cout<<"hasta aqui todo bien..."<<espacios<<"\n";
    if (espacios == 0)    {
        args[0] = buffer;
    } else{
        while (posActual <= espacios){
            //cout<<"entro al while"<<"\n";
            args[posActual] = strsep(&buffer, " ");
            //cout << args[posActual] << "\n";
            //cout<<"lo separo bien \n";
            posActual++;
        }
        //cout<<"salio del while \n";
        args[posActual] = NULL;
        //cout<<"agrego el ultimo null"<<"\n";
    }
    if (strlen(historia.c_str()) > 0){
        //cout<<"entro al if mayor de 0\n";
        add_history(historia.c_str());
        //cout<<"lo agrego al history\n";
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "close") == 0){
            exit(0);
        }
        else if (strcmp(args[0], "clear") == 0){
            for (int i = 0; i < 100; i++){
                cout << '\n';
            }
        }
        else if (strcmp(args[0], "cd") == 0){
            if (chdir(args[1]) < 0){
                cout << "cd to " << args[1] << " no se pudo ejecutar.";
                perror(":>");
                cout << "\n";
            }
        }else if (contarPipes(args, posActual) == 1){
            char*** comandosPipe;
            comandosPipe = dosComandos(args, posActual);

            executeSimplePipe(comandosPipe);

        }else if (contarPipes(args, posActual) > 1){
            int pipeCount = contarPipes(args, posActual);

            int cont = 0;
            const char delim[2] = "|";
            char *token;

            char *argString = const_cast<char*>(historia.c_str());
            token = strtok(argString, delim);

            char** argArray[1024];

            while (token != NULL)
            {
                char* param[1024];
                int cont2 = 0;

                token = strtok(NULL, delim);

                const char delim2[2] = " ";
                char *token2;

                token2 = strtok(token, delim2);

                while (token2 != NULL)
                {                    
                    token2 = strtok(NULL, delim2);
                    
                    if (token2 == 0) {
                        param[cont] = token2;
                        cont2++;

                        cout << "Token 1: " << token << endl << "Token 2: " << token2 << endl << endl;
                    }else{
                        cout << "Token vacío ignorado" << endl;
                    }

                }

                argArray[cont] = param;


                cont++;
            }

            executeMultiplePipe(argArray, pipeCount);

        }else{
            

            executeSimpleCommand(args);
            //cout<<"\n";
       }
    }
}

int main(){
    while (true){
        header();
        interpretCmd();
    }
    return 0;
}