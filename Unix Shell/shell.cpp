#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <cstring>
#include "Tokenizer.h"
#include <fcntl.h>

// all the basic colours for a shell prompt
#define RED     "\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE	"\033[1;34m"
#define WHITE	"\033[1;37m"
#define NC      "\033[0m"

using namespace std;

int main () {
    int in = dup(STDIN_FILENO);
    int out = dup(STDOUT_FILENO);
    char prevDir[1500];
    prevDir[0] = 0;

    int status;
    vector<pair<pid_t, string>> zombies;

    for (;;) 
    {
        time_t now = time(0);
        string formatDate = ctime(&now);
        char dir[1500];
        getcwd(dir, sizeof(dir));
        if(formatDate.back() == '\n')
        {
            formatDate.pop_back();
        }
        // need date/time, username, and absolute path to current dir
        cout << formatDate+" " << GREEN << getenv("USER") << WHITE <<":" << BLUE << dir << NC << "$ ";
        
        // get user inputted command
        string input;
        getline(cin, input);

        // checking zombie process here since that's how the actual terminal does it, or so it seems.. 
        // but you know what they say: If it walks like a duck, talks like a duck, it probably is a duck
        for (size_t i = 0; i < zombies.size(); i++)
        {
            status = 0;
            int pid = waitpid(zombies[i].first, &status, WNOHANG);

            if (pid > 0)
            {
                cerr << "[" << i + 1 << "]   Done                    " << zombies[i].second << '\n'; // may cause process to take longer in background but worth it
                zombies.erase(zombies.begin() + i);
            }
            if (pid < 0)
            {
                perror("waitpid");
                exit(1);
            }
            if (status > 1)
            {
                exit(status);
            }
        }

        if (input == "exit") {  // print exit message and break out of infinite loop
            cout << RED << "Now exiting shell..." << endl << "Goodbye" << NC << endl;
            break;
        }

        // get tokenized commands from user input
        Tokenizer tknr(input);
        if (tknr.hasError()) {  // continue to next prompt if input had an error
            continue;
        }

        // // print out every command token-by-token on individual lines
        // // prints to cerr to avoid influencing autograder
        // for (auto cmd : tknr.commands) {
        //     cerr << "Command :";
        //     for (auto str : cmd->args) {
        //         cerr << "|" << str << "| ";
        //     }
        //     if (cmd->hasInput()) {
        //         cerr << "in< " << cmd->in_file << " ";
        //     }
        //     if (cmd->hasOutput()) {
        //         cerr << "out> " << cmd->out_file << " ";
        //     }
        //     cerr << endl;
        // }

        for(size_t i = 0; i < tknr.commands.size(); i++)
        {
            int fd[2];

            if (pipe(fd) < 0)
            {
                perror("Failed to pipe");
                exit(1);
            }

            // Check if first command is cd or not, fully support cd redirection (>, <) and cd piping just like the real terminal
            if (i == 0 && tknr.commands[0] -> args[0] == "cd")
            {
                if (tknr.commands[i] -> args[1] == "-") 
                {
                    if (prevDir[0] == 0)
                    {
                        cout << "bash: cd: OLDPWD not set\n";
                        continue;
                    }
                    if (tknr.commands.size() == 1)
                    {
                        if (chdir(prevDir) != 0)
                        {
                            perror("Unable to change path");
                            exit(1);
                        }
                        if (tknr.commands[0] -> hasOutput())
                        {
                            
                            int outFile = open(tknr.commands[i] -> out_file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
                            if(outFile < 0)
                            {
                                perror("open");
                                exit(1);
                            }
                            dup2(outFile, STDOUT_FILENO);
                            close(outFile);
                            cout << prevDir << endl;
                        }
                        strcpy(prevDir, dir);
                        continue;
                    }
                }
                else
                {
                    if (tknr.commands[0] -> hasOutput())
                    {
                        int outFile = open(tknr.commands[i] -> out_file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
                        if(outFile < 0)
                        {
                            perror("open");
                            exit(1);
                        }
                        dup2(outFile, STDOUT_FILENO);
                        close(outFile);
                        cout << "\0";
                    }
                    if (tknr.commands.size() == 1)
                    {
                        if (chdir(tknr.commands[i] -> args[1].c_str()) != 0)
                        {
                            perror("Unable to change path");
                            exit(1);
                        }
                        strcpy(prevDir, dir);
                        continue;
                    }
                }
            }

            std::vector<char*> args;
            for (size_t j = 0; j < tknr.commands[i]->args.size(); j++)
                {
                    args.push_back((char*) tknr.commands[i]->args[j].c_str());
                }
                
            args.push_back(NULL);
            
            // fork to create child
            pid_t pid = fork();
            if (pid < 0) {  // error check
                perror("fork");
                exit(1);
            }

            if (pid == 0) 
            {  // if child, exec to run command
                if (i < tknr.commands.size() - 1)
                    {
                        dup2(fd[1], STDOUT_FILENO);
                    }
                close(fd[0]);

                if (tknr.commands[i] -> hasInput())
                {
                    int inFile = open(tknr.commands[i] -> in_file.c_str(), O_RDONLY, 0666);
                    dup2(inFile, STDIN_FILENO);
                    close(inFile);
                }
                if (tknr.commands[i] -> hasOutput())
                {
                    int outFile = open(tknr.commands[i] -> out_file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
                    dup2(outFile, STDOUT_FILENO);
                    close(outFile);
                }

                // Imitate real terminal wackiness in cd
                if (tknr.commands[i] -> args[0] == "cd")
                {
                    if (tknr.commands[i] -> args[1] == "-")
                    {
                        cout << prevDir << endl;
                    }
                    else
                    {
                        cout << endl;
                    }
                    exit(0);
                }

                if (execvp(args[0], args.data()) < 0) {  // error check
                    perror("execvp");
                    exit(1);
                }
            }
            else {  // if parent, wait for child to finish, sike don't do dat
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                if (tknr.commands[i] -> isBackground())
                {
                    string command;

                    cerr << "[" << zombies.size() + 1 << "] " << pid << endl;

                    for (size_t j = 0; j < args.size() - 1; j++)
                    {
                        if (!tknr.commands[i]->args[j].empty())
                        {
                            command += " ";
                        }
                        command += tknr.commands[i] -> args[j];
                    }
                    zombies.push_back(make_pair(pid, command));
                }
                else{
                    waitpid(pid, &status, 0);
                    if (status > 1)
                    {
                        exit(status);
                    }
                }
            }
        }
        dup2(in, STDIN_FILENO);
        dup2(out, STDOUT_FILENO);
    }
}