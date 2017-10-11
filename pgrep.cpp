#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<vector>
#include <iostream>
#include<string.h>
#include <regex>

char *line;
int score;
int end = 1;

std::mutex res_thread, laod_line;

unsigned working;


std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */
std::vector<std::regex *> reg;    // zoradene regexy
std::vector<int> min_score;     // skore regexu

char *read_line(int *res) {
    std::string line;
    char *str;
    if (std::getline(std::cin, line)) {
        str=(char *) malloc(sizeof(char)*(line.length()+1));
        strcpy(str,line.c_str());
        *res=1;
        return str;
    } else {
        *res=0;
        return NULL;
    }

}

void f(int ID){
    std::mutex *my_mutex;
    std::regex *my_regex;
    int my_score;


    my_mutex = zamky[ID];
    my_regex = reg[ID];
    my_score = min_score[ID];


    int res = 0;
    while (1){
        while (!my_mutex->try_lock()){
            if (!end){
                return;
            }
        }

        res = std::regex_match(line, (*my_regex));
        res_thread.lock();
        score += res?my_score:0;
        working -= 1;
        if (working == 0){
            laod_line.unlock();
        }
        res_thread.unlock();
    }
}

int main(int argc, char **argv) {
    laod_line.lock();
    /*************************
     * deklaracia premennych
     */
    unsigned num = 0; // pocet regex
    int minimum;
    std::vector<std::thread *> thread;

    /**************************
     * kontroala vstupu a init
     */

    if ((argc%2 != 0) || (argc < 4)){
        fprintf(stderr,"Zle zadane argumenty\n");
        return 1;
    }
    num = (argc - 2)/2;
    minimum = strtod(argv[1],NULL); //todo skontrolovat argv[1] ci je cislo
    // nastavenie velikosti poli
    zamky.resize(num);
    thread.resize(num);
    min_score.resize(num);
    reg.resize(num);

    // inicializovanie poli
    for (unsigned i=0; i<num; i++){
        std::mutex *new_mutex = new std::mutex();
        std::regex *new_regex = new std::regex(argv[2+2*i]);
        int new_score = (int) strtod(argv[3+2*i],NULL); //todo skontrolovat ci retazec je cislo
        //zamknutie mutex
        new_mutex->try_lock();
        //nastavenie poli
        zamky[i] = new_mutex;
        reg[i] = new_regex;
        min_score[i] = new_score;
    }
    // spustenie threadou
    for (unsigned i=0; i<num; i++){
        std::thread *new_thread = new std::thread(f,i);
        thread[i] = new_thread;
    }
    /**********************************
     * Vlastni vypocet pgrep
     * ********************************/
    line = read_line(&end);
    while (end){
        score = 0;
        working = num;

        for (unsigned i=0; i<num; i++){
            zamky[i]->unlock();
        }
        laod_line.lock(); //todo deadlock
        if (minimum <= score){
            printf("%s\n",line);
        }
        free(line);
        line = read_line(&end);
    }

    /**********************************
     * Uvolneni pameti
     * ********************************/

    for (unsigned i=0; i<num; i++){
        (*(thread[i])).join();
        delete thread[i];
        delete zamky[i];
        delete reg[i];
    }

    return 0;
}
