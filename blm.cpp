#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <math.h> 
#include <fstream>
#include <chrono>
using namespace std;

struct Instancia {
    int m;                  // número de máquinas
    int n;                  // número de tarefas
    vector<int> tarefas;    // tempo de cada tarefa

    Instancia(int paramM, float paramR) {
        m = paramM;
        n = static_cast<int>(std::pow(m, paramR)); // pow arredonda pra baixo
    }
    
    void tarefaRandom( mt19937& gen ) { // gera novos valores para todas as tarefas
        uniform_int_distribution<> tempo (1,100);

        tarefas.resize(n);

        for( int i=0; i<n ;i++ )
            tarefas[i] = tempo(gen);

    }
};

struct Solucao {
    vector<vector<int>> maquinas; // cada máquina guarda os índices das tarefas
    vector<int> carga;            // carga de cada máquina
    int makespan;
    int iteracoes;

    Solucao(int m) {
        maquinas.resize(m);
        carga.assign(m, 0);
        makespan = 0;
        iteracoes = 0;
    }

    void estadoInicial ( Instancia& inst ) { // distribuição inicial das tarefas para a primeira máquina
        for (int i = 0; i < inst.n; i++) {;
            maquinas[0].push_back(i);
            carga[0] += inst.tarefas[i];
        }
        makespan = carga[0];
    }

    void iterarPrimeiraMelhora ( Instancia& inst ) {
        // int indexMakespan = distance( carga.begin(), max_element( carga.begin(), carga.end() ) ); // verificar qual maquina tem o makespan

        // for (int i = indexMakespan+1; i < inst.m; i++) {                                    // procura a primeira melhora pelos vizinhos
        //     int novoMakespan = carga[i] + inst.tarefas[ maquinas[indexMakespan].back() ];   // simular passar a ultima tarefa para vizinho
        //     if( novoMakespan < makespan ) {                                                 // se reduz o makespan, executa a mudança
        //         maquinas[i].push_back( maquinas[indexMakespan].back() );                    // copia a tarefa para o vizinho
        //         carga[indexMakespan] -= inst.tarefas[ maquinas[indexMakespan].back() ];
        //         carga[i] += inst.tarefas[ maquinas[indexMakespan].back() ];
        //         maquinas[indexMakespan].pop_back();                                         // deleta a tarefa da pilha original
                
        //         makespan = *max_element( carga.begin(), carga.end() );

        //         iterarPrimeiraMelhora(inst);
        //         iteracoes++;
        //         break;
        //     }
        // }        // se o makespan fica igual ou piora, encerrar
        // A SOLUÇÃO RECURSIVA ACABA CRASHANDO EM M=50 R=2
        
        bool melhorou = true;

        while (melhorou) {
            melhorou = false;

            int indexMakespan = distance( carga.begin(), max_element(carga.begin(), carga.end()) );

            for (int i = indexMakespan + 1; i < inst.m; i++) {

                if (maquinas[indexMakespan].empty())
                    break;

                int tarefa = maquinas[indexMakespan].back();
                int novoMakespan = carga[i] + inst.tarefas[tarefa];

                if ( novoMakespan < makespan ) {

                    maquinas[i].push_back(tarefa);
                    maquinas[indexMakespan].pop_back();

                    carga[indexMakespan] -= inst.tarefas[tarefa];
                    carga[i] += inst.tarefas[tarefa];

                    makespan = *max_element(carga.begin(), carga.end());

                    iteracoes++;
                    melhorou = true;
                    break;
                }
            }
        }
    }
};

int main() {
    random_device rd;
    mt19937 gen(rd());

    ofstream arqResult ("results.txt");
    if ( arqResult.is_open() ){
        arqResult << "heuristica,n,m,replicacao,tempo,iteracoes,valor,parametro\n";
        arqResult.close();
    } else {
        cout << "Erro ao abrir arquivo txt\n";
        return 0;
    }
    
    
    for ( int m : {10, 20, 50} ) {
        for ( float r : {1.5, 2.0} ) {            
            Instancia inst(m, r);

            for ( int i = 0; i < 10; i++ ) {
                
                inst.tarefaRandom( gen );

                Solucao sol(inst.m);
                sol.estadoInicial(inst);

                auto inicio = std::chrono::high_resolution_clock::now();

                sol.iterarPrimeiraMelhora(inst);

                auto fim = std::chrono::high_resolution_clock::now();
                double tempo = std::chrono::duration<double, milli>(fim - inicio).count();

                arqResult.open("results.txt", fstream::app); // fstream::app - modo em que o arquivo é aberto(app = append)
                if ( arqResult.is_open() ){
                    //              heuristica,n,m,replicacao,tempo,iteracoes,valor(makespan),parametro
                    arqResult << "Monotona - Primeira Melhora," << inst.n << "," << inst.m << "," << i+1 << "," << tempo << "ms," << sol.iteracoes << "," << sol.makespan << ",NA\n";
                    arqResult.close();
                } else {
                    cout << "Erro ao abrir arquivo txt\n";
                    return 0;
                }

                if(inst.n == 31) {
                    int i = 1;
                    for ( vector<int> y : sol.maquinas ) {
                        cout << "maquina " << i << ":\n";
                        for (int x : y){
                            cout << inst.tarefas[x] << endl;
                        }
                        cout << "-------------\n";
                        i++;
                    }
                }
           
            }
            
        }
    }

    return 0;
}