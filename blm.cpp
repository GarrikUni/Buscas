#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <math.h> 
#include <fstream>
#include <chrono>
using namespace std;

struct Instancia
{
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
        int indexMakespan = distance( carga.begin(), max_element( carga.begin(), carga.end() ) ); // verificar qual maquina tem o makespan

        for (int i = indexMakespan+1; i < inst.m; i++) {                                    // procura a primeira melhora pelos vizinhos
            int novoMakespan = carga[i] + inst.tarefas[ maquinas[indexMakespan].back() ];   // simular passar a ultima tarefa para vizinho
            if( novoMakespan < makespan ) {                                                 // se reduz o makespan, executa a mudança
                maquinas[i].push_back( maquinas[indexMakespan].back() );                    // copia a tarefa para o vizinho
                carga[indexMakespan] -= inst.tarefas[ maquinas[indexMakespan].back() ];
                makespan = carga[indexMakespan];
                carga[i] += inst.tarefas[ maquinas[indexMakespan].back() ];
                maquinas[indexMakespan].pop_back();                                         // deleta a tarefa da pilha original
                
                iterarPrimeiraMelhora(inst);
                iteracoes++;
                break;
            }
        }

        // se o makespan fica igual ou piora, encerrar
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
    
    
    int replicacao = 1;
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

            replicacao++;

            // for ( int y=0; y < sol.maquinas.size(); y++ ) {
            //     cout << "maquina " << y << ":\n";
            //     cout << "carga " << sol.carga[y] << "\n";
            //     cout << endl;
            // }


            }
            
        }
    }

    return 0;
}