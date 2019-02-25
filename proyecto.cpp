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
    cout << "hols111" << endl;
    cout << "esto: " << argsOut[1][0] << endl;
    
    return argsOut;
}


void e (char** args1, char** args2){
    int des_p[2];
    if(pipe(des_p) == -1) {
        perror("Pipe falló");
        return;
    }

    cout << "ha" << endl;

    pid_t pid1 = fork();
    if(pid1 == 0)
    {
        close(STDOUT_FILENO);
        dup(des_p[1]);
        close(des_p[0]);
        close(des_p[1]);


        execvp(args1[0], args1);
        perror("execvp de args1 falló");
        exit(1);
    }

    pid_t pid2 = fork();
    if(pid2 == 0)
    {
        close(STDIN_FILENO);
        dup(des_p[0]);
        close(des_p[1]);
        close(des_p[0]);


        execvp(args2[0], args2);
        perror("execvp de args2 falló");
        exit(1);
    }

    close(des_p[0]);
    close(des_p[1]);
    wait(0);
    wait(0);
}

void e2(char** args, char** args2) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0){
        close(pipefd[0]); // close reading end in the child

        dup2(pipefd[1], 1); // send stdout to the pipe
        dup2(pipefd[1], 2); // send stderr to the pipe

        close(pipefd[1]); // this descriptor is no longer needed

        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }else{
        // parent
        const int buffer_size = 1024;

        char buffer[buffer_size];

        for(int i = 0; i < buffer_size; i++)
        {
            //buffer[i] = NULL;
        }        

        close(pipefd[1]); // close the write end of the pipe in the parent

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }

        cout << args2[0] << endl;
        cout << buffer << endl;

        //execl(args2[0], const_cast<char*>(buffer));
        execl("/bin/cat", "ola", 0);
        perror("execvp de args2 falló");
    }
}

void executeSimplePipe(char*** args){
    cout << "esto2 - " << args[0][0] << endl;
    cout << "esto3 - " << args[1][0] << endl;
    int des_p[2];
    char** args1 = args[0];
    char** args2 = args[1];

    cout << "esto2 - " << args1[0] << endl;
    cout << "esto3 - " << args2[0] << endl;

    e2(args1, args2);
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
        }else{
            //cout << "hols" << "\n";
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