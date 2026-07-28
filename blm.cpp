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
    
    void tarefaRandom( mt19937& gen ) {
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

    void solucaoInicial ( mt19937& gen, Instancia inst ) { // distribuição aleatória inicial das tarefas para as máquinas
        uniform_int_distribution<> maq(0, inst.m - 1);

        for (int i = 0; i < inst.n; i++) {
            int destino = maq(gen);

            maquinas[destino].push_back(i);
            carga[destino] += inst.tarefas[i];
        }

        makespan = *max_element(carga.begin(), carga.end());
    }
};

int main() {
    random_device rd;
    mt19937 gen(rd());

    vector<Instancia> instancias;

    for ( int m : {10, 20, 50} ) {
        for ( float r : {1.5, 2.0} ) {
            instancias.push_back( Instancia(m, r) );
        }
    }

    cout << "m = " << instancias[0].m << endl;
    cout << "n = " << instancias[0].n << endl;

    instancias[0].tarefaRandom( gen );

    for ( int x : instancias[0].tarefas ) {
        cout << x << endl;
    }

    Solucao sol(instancias[0].m);

    sol.solucaoInicial(gen, instancias[0]);

    int i = 0;
    for ( vector<int> y : sol.maquinas ) {
        cout << "maquina " << i << ":\n";
        for ( int x : y ) {
            cout << x << endl;
        }
        cout << endl;
        i++;
    }

    return 0;
}