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
    float p;                // percentual de perturbação
    vector<int> tarefas;    // tempo de cada tarefa

    Instancia( int paramM, float paramR, float paramP ) {
        m = paramM;
        n = static_cast<int>(std::pow(m, paramR)); // pow arredonda pra baixo
        p = paramP;
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
        bool melhorou = true;

        while (melhorou) {
            melhorou = false;

            int indexMakespan = distance( carga.begin(), max_element(carga.begin(), carga.end()) );

            for (int i = 0; i < inst.m; i++) {

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

    void iterarMelhorMelhora(Instancia& inst) {
        bool melhorou = true;

        while (melhorou) {
            melhorou = false;

            int indexMakespan = distance( carga.begin(), max_element(carga.begin(), carga.end()) );

            int indexMelhorDestino = -1;
            int indexMelhorTarefa = 0;

            int melhorMakespan = makespan;

            int maiorPeso = inst.tarefas[maquinas[indexMakespan][0]];
            for (int pos = 1; pos < maquinas[indexMakespan].size(); pos++) {
                int tarefa = maquinas[indexMakespan][pos];
                if (inst.tarefas[tarefa] > maiorPeso) {
                    maiorPeso = inst.tarefas[tarefa];
                    indexMelhorTarefa = pos;
                }
            }
            int tarefa = maquinas[indexMakespan][indexMelhorTarefa];
            int peso = inst.tarefas[tarefa];
            for (int i = 0; i < inst.m; i++) {
                if (i == indexMakespan)
                    continue;
                vector<int> cargaSimulada = carga;
                cargaSimulada[indexMakespan] -= peso;
                cargaSimulada[i] += peso;
                int makespanSimulado =
                    *max_element(cargaSimulada.begin(), cargaSimulada.end());
                if (makespanSimulado < melhorMakespan) {
                    melhorMakespan = makespanSimulado;
                    indexMelhorDestino = i;
                }
            }
            if (indexMelhorDestino != -1) {
                maquinas[indexMelhorDestino].push_back(tarefa);
                maquinas[indexMakespan].erase(
                    maquinas[indexMakespan].begin() + indexMelhorTarefa
                );
                carga[indexMakespan] -= peso;
                carga[indexMelhorDestino] += peso;
                makespan = melhorMakespan;
                iteracoes++;
                melhorou = true;
            }

            // for (int pos = 0; pos < maquinas[indexMakespan].size(); pos++) { // testa todas as tarefas da máquina crítica
            //     int tarefa = maquinas[indexMakespan][pos];
            //     int peso = inst.tarefas[tarefa];

            //     for (int i = 0; i < inst.m; i++) { // testa todos os destinos
            //         if (i == indexMakespan)
            //             continue;

            //         vector<int> cargaSimulada = carga;

            //         cargaSimulada[indexMakespan] -= peso;
            //         cargaSimulada[i] += peso;

            //         int makespanSimulado = *max_element( cargaSimulada.begin(), cargaSimulada.end() );

            //         if ( makespanSimulado < melhorMakespan ) {
            //             melhorMakespan = makespanSimulado;
            //             indexMelhorDestino = i;
            //             indexMelhorTarefa = pos;
            //         }
            //     }
            // }

            // if (indexMelhorDestino != -1) {

            //     int tarefa =
            //         maquinas[indexMakespan][indexMelhorTarefa];

            //     int peso = inst.tarefas[tarefa];

            //     maquinas[indexMelhorDestino].push_back(tarefa);
            //     maquinas[indexMakespan].erase( maquinas[indexMakespan].begin() + indexMelhorTarefa );

            //     carga[indexMakespan] -= peso;
            //     carga[indexMelhorDestino] += peso;

            //     makespan = melhorMakespan;

            //     iteracoes++;
            //     melhorou = true;
            // }
        }
    }

    void redistribuiAleatorio ( mt19937& gen, Instancia& inst ) {
        uniform_int_distribution<> maq (0,inst.m-1);

        int numTrocasAleatorias = floor(inst.n * inst.p);
        int origem, destino;
        for ( int i=0; i<numTrocasAleatorias; i++ ) {
            do {
                origem = maq(gen);
            } while (maquinas[origem].empty());
            do {
                destino = maq( gen );
            } while (origem == destino);
            
            maquinas[destino].push_back( maquinas[origem].back() );
            carga[destino] += inst.tarefas[ maquinas[origem].back() ];
            carga[origem] -= inst.tarefas[ maquinas[origem].back() ];
            maquinas[origem].pop_back();
        }
        makespan = *max_element( carga.begin(), carga.end() );
    }
};

int main() {
    random_device rd;
    mt19937 gen(rd());

    ofstream arqResult ("results2.txt");
    if ( arqResult.is_open() ){
        arqResult << "heuristica,n,m,replicacao,tempo,iteracoes,valor,parametro\n";
        arqResult.close();
    } else {
        cout << "Erro ao abrir arquivo txt\n";
        return 0;
    }
    
    Instancia inst(50, 2, 0.9);
    inst.tarefaRandom( gen );
    Solucao sol(inst.m);
    sol.estadoInicial(inst);
    
    for ( int m : {10, 20, 50} ) {
        for ( float r : {1.5, 2.0} ) {        
            for ( float p : {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9} ) {   
                Instancia inst(m, r, p);

                for ( int i = 0; i < 10; i++ ) {
                
                    inst.tarefaRandom( gen );

                    Solucao sol(inst.m);
                    sol.estadoInicial(inst);

                    auto inicio = std::chrono::high_resolution_clock::now();
                           
                    int tentativasSemMelhora = 0;
                    sol.iterarPrimeiraMelhora(inst);
                    sol.iterarMelhorMelhora(inst);
                    Solucao sol_2 = sol;
                    
                    long long iteracoesTotal = sol.iteracoes;
                    // cout << sol.makespan << " n="<<inst.n<< " p="<< p<< " i="<< i << endl;
                    do {
                        sol_2.redistribuiAleatorio( gen, inst );
                        sol_2.iterarPrimeiraMelhora(inst);
                        sol_2.iterarMelhorMelhora( inst );

                        iteracoesTotal += sol_2.iteracoes;
                                    
                        if ( sol_2.makespan < sol.makespan ){
                            // cout <<"melhora: " << sol.makespan << " -> " << sol_2.makespan << " n="<<inst.n<< " p="<< p<< " i="<< i << endl;
                            sol = sol_2;
                            tentativasSemMelhora = 0;
                        } else {
                            //cout << "sem melhora"<< endl;
                            //sol_2 = sol;
                            tentativasSemMelhora++;
                        }

                    } while ( tentativasSemMelhora < 1000 );
                    

                    auto fim = std::chrono::high_resolution_clock::now();
                    double tempo = std::chrono::duration<double, milli>(fim - inicio).count();

                    arqResult.open("results2.txt", fstream::app); // fstream::app - modo em que o arquivo é aberto(app = append)
                    if ( arqResult.is_open() ){
                        //              heuristica,n,m,replicacao,tempo,iteracoes,valor(makespan),parametro
                        arqResult << "BLNM - Busca Local Iterada," << inst.n << "," << inst.m << "," << i+1 << "," << tempo << "ms," << iteracoesTotal << "," << sol.makespan << ","<< inst.p <<"\n";
                        arqResult.close();
                    } else {
                        cout << "Erro ao abrir arquivo txt\n";
                        return 0;
                    }
                    
                    // if( inst.n == 31 ) {
                    //     int k = 1;
                    //     for ( vector<int> y : sol.maquinas ) {
                    //         cout << "maquina " << k << ":\n";
                    //         for (int x : y){
                    //             cout << inst.tarefas[x] << endl;
                    //         }
                    //         cout << "-------------\n";
                    //         k++;
                    //     }
                    // }

                }
            }       
        }
    }

    return 0;
}