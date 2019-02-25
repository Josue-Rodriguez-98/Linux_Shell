#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>
/* usando otra funcion clear */
#define clear() printf("\033[H\033[J")
using namespace std;

void header(){
    char cwd[1024];
    char *user;
    user = getenv("USER");
    getcwd(cwd, sizeof(cwd));
    cout << user << "@myComputer:~" << cwd << ":>";
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

void executeCatCommand(char** args) {
    pid_t pidt = fork();
    if (pidt < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pidt == 0) {
        int success = execlp(args[0], args[0], args[1], NULL);
        if (success < 0) {
            cout << endl;
            perror("");
            cout << endl;
        }
    } else {
        wait(NULL);
        return;
    }
}

void executeSimpleCommand(char** args){
    pid_t pidt = fork();
    if(pidt == 0) {
        int exito = execvp(args[0],args);
        if(exito < 0){
            cout<<"No se ejecuto el comando: " << endl;
            perror("");
        }
    } else {
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
    char* args[100];
    int posActual = 0;
    int espacios = contarEspacios(buffer);
    //cout<<"hasta aqui todo bien..."<<espacios<<"\n";
    if (espacios == 0) {
        args[0] = buffer;
    } else{
        while (posActual <= espacios) {
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
    if (strlen(historia.c_str()) > 0) {
        //cout<<"entro al if mayor de 0\n";
        add_history(historia.c_str());
        //cout<<"lo agrego al history\n";
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "close") == 0) {
            cout << "quitting..." << endl;
            exit(0);
        }
        else if (strcmp(args[0], "clear") == 0) {
            clear();
        }
        else if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) < 0) {
                cout << "cd to " << args[1] << " no se pudo ejecutar.";
                perror(":>");
                cout << "\n";
            }
        } else if (strcmp(args[0], "cat")==0) {
            if (args[1] == NULL) {
                cout << "You didn't specify a file!" << endl;
            } else if (strcmp(args[1],">")==0) {
                if (args[2] == NULL) {
                    cout << "You didn't specify a file!" << endl;
                } else {
                    string line;
                    cout << "> ";
                    ofstream myfile;
                    myfile.open (args[2]);
                    while(getline(cin, line)) {
                        myfile << line << endl;
                        cout << "> ";
                        line = "";
                    }
                    myfile.close();
                    cout << endl;
                }
            } else {
                executeCatCommand(args);
                cout << endl;
            }
        } else {
            //cout << "hols" << "\n";
            executeSimpleCommand(args);
            //cout<<"\n";>
        }
    }
}
// function for finding pipe 
int parsePipe(char* str, char** strpiped) { 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "<"); 
        if (strpiped[i] == NULL) {
            break; 
        }
    } 

    if (strpiped[1] == NULL) {
        return 0; // returns zero if no pipe is found. 
    } else {
        return 1; 
    } 
}

int main(){
    while (true){
        header();
        interpretCmd();
    }
    return 0;
}