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
        if(exito < 0){
            cout << "No se ejecuto el comando: ";
            perror("");
        }
    }else{
        wait(NULL);
        return;
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
        } else{
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