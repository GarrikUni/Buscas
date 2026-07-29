#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <math.h> 
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

    Solucao(int m) {
        maquinas.resize(m);
        carga.assign(m, 0);
        makespan = 0;
    }

    void estadoInicial ( Instancia& inst ) { // distribuição inicial das tarefas para a primeira máquina
        for (int i = 0; i < inst.n; i++) {;
            maquinas[0].push_back(i);
            carga[0] += inst.tarefas[i];
        }
        makespan = carga[0];
        //makespan = *max_element(carga.begin(), carga.end());
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
                break;
            }
        }

        // se o makespan fica igual ou piora, encerrar
    }
};

int main() {
    random_device rd;
    mt19937 gen(rd());

    //vector<Instancia> instancias;

    for ( int m : {10, 20, 50} ) {
        for ( float r : {1.5, 2.0} ) {            
            //instancias.push_back( Instancia(m, r) );
            Instancia inst(m, r);
            inst.tarefaRandom( gen );

            Solucao sol(inst.m);

            sol.estadoInicial(inst);

            sol.iterarPrimeiraMelhora(inst);

            // for ( int y=0; y < sol.maquinas.size(); y++ ) {
            //     cout << "maquina " << y << ":\n";
            //     cout << "carga " << sol.carga[y] << "\n";
            //     cout << endl;
            // }
        }
    }

    // cout << "m = " << instancias[0].m << endl;
    // cout << "n = " << instancias[0].n << endl;

    // instancias[0].tarefaRandom( gen );

    // for ( int x : instancias[0].tarefas ) {
    //     cout << x << endl;
    // }

    // Solucao sol(instancias[0].m);

    // sol.estadoInicial(instancias[0]);

    // int i = 0;
    // for ( vector<int> y : sol.maquinas ) {
    //     cout << "maquina " << i << ":\n";
    //     for ( int x : y ) {
    //         cout << instancias[0].tarefas[x] << endl;
    //     }
    //     cout << endl;
    //     i++;
    // }

    return 0;
}