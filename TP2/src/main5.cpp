#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <tuple>
#include <limits>
#include <algorithm>

using namespace std;

const int INF = numeric_limits<int>::max();

class Grafo {
public:
    vector<vector<pair<int, int>>> adjList;
    vector<vector<int>> adjListInversa;
    vector<int> antecessor;
    vector<int> distBFS;
    map<pair<int, int>, pair<int, int>> antecessorDijkstra;
    map<pair<int, int>, int> distDijkstra;
    map<pair<int, int>, bool> visitado;

    Grafo(int numEspacos) {
        adjList.resize(numEspacos);
        adjListInversa.resize(numEspacos);
        antecessor.resize(numEspacos, INF);
        distBFS.resize(numEspacos, INF);
    }

    void bfs(int inicio) {
        fill(distBFS.begin(), distBFS.end(), INF);
        fill(antecessor.begin(), antecessor.end(), INF);
        antecessor[inicio] = inicio;
        distBFS[inicio] = 0;

        queue<int> fila;
        fila.push(inicio);
        while (!fila.empty()) {
            int vertice = fila.front();
            fila.pop();
            for (int adj : adjListInversa[vertice]) {
                if (distBFS[vertice] + 1 < distBFS[adj]) {
                    distBFS[adj] = distBFS[vertice] + 1;
                    antecessor[adj] = vertice;
                    fila.push(adj);
                }
            }
        }
    }

    pair<int, int> dijkstra(int inicio, int maxTurnos, int recursosTurno, const set<pair<int, int>>& bloqueios) {
        distDijkstra.clear();
        antecessorDijkstra.clear();
        visitado.clear();

        distDijkstra[{inicio, 0}] = 0;
        antecessorDijkstra[{inicio, 0}] = {inicio, 0};

        priority_queue<tuple<int, int, int, int>> pq;
        pq.push({0, 0, inicio, 0});

        while (!pq.empty()) {
            auto [custo, recurso, vertice, turno] = pq.top();
            pq.pop();

            if (vertice == adjList.size() - 1) {
                return {vertice, turno};
            }
            if (visitado[{vertice, turno}] || turno + 1 > maxTurnos) {
                continue;
            }

            visitado[{vertice, turno}] = true;
            custo *= -1;
            recurso += recursosTurno;
            turno++;

            for (auto& [adj, peso] : adjList[vertice]) {
                if (!bloqueios.count({adj, turno - 1}) && !bloqueios.count({adj, turno})) {
                    if (recurso >= peso && custo + peso < distDijkstra[{adj, turno}]) {
                        distDijkstra[{adj, turno}] = custo + peso;
                        antecessorDijkstra[{adj, turno}] = {vertice, turno - 1};
                        pq.push({-distDijkstra[{adj, turno}], recurso - peso, adj, turno});
                    }
                }
            }
        }

        return {-1, -1};
    }
};

class Jogo {
public:
    int numEspacos, numConexoes, numMonstros, maxTurnos, recursosTurno;
    Grafo* grafo;
    vector<vector<int>> trajetosMonstros;
    set<pair<int, int>> bloqueios;

    Jogo(int nEspacos, int nConexoes, int nMonstros, int maxT, int recTurno)
        : numEspacos(nEspacos), numConexoes(nConexoes), numMonstros(nMonstros), maxTurnos(maxT), recursosTurno(recTurno) {
        grafo = new Grafo(numEspacos);
    }

    ~Jogo() {
        delete grafo;
    }

    void processarMonstros(const vector<int>& indicesMonstros) {
        trajetosMonstros.resize(numMonstros);
        for (int i = 0; i < numMonstros; ++i) {
            int atual = indicesMonstros[i];
            int turnoBloqueio = 0;
            while (atual != 0 && grafo->antecessor[atual] != INF) {
                trajetosMonstros[i].emplace_back(atual);
                bloqueios.insert({atual, turnoBloqueio++});
                atual = grafo->antecessor[atual];
            }
            trajetosMonstros[i].emplace_back(0); // Add starting point to path
            for (int turno = turnoBloqueio; turno <= maxTurnos; ++turno) {
                bloqueios.insert({0, turno});
            }
        }
    }

    // Função para imprimir o caminho
    void imprimirCaminho(pair<int, int> pos) {
        if (pos.first == -1) {
            printf("Nao foi encontrado um caminho\n");
            return;
        }

        int custoTotal = grafo->distDijkstra[pos]; 
        int turnosJogados = pos.second;
        printf("%d %d\n", custoTotal, turnosJogados);

        vector<int> caminho;
        caminho.push_back(1); 
        
        while (pos != grafo->antecessorDijkstra[pos]) {
            caminho.emplace_back(pos.first + 1); 
            pos = grafo->antecessorDijkstra[pos];
        }
        reverse(caminho.begin(), caminho.end());

        for (size_t i = 0; i < caminho.size(); ++i) {
            printf("%d", caminho[i]);
            if (i < caminho.size() - 1) {
                printf(" ");
            }
        }
        printf("\n");
    }
};

int main() {
    int numEspacos, numConexoes, numMonstros, maxTurnos, recursosTurno;

    // Leitura de entrada
    if (scanf("%d %d %d %d %d", &numEspacos, &numConexoes, &numMonstros, &maxTurnos, &recursosTurno) != 5) {
        cerr << "Erro na leitura dos dados iniciais.\n";
        return 1;
    }

    Jogo jogo(numEspacos, numConexoes, numMonstros, maxTurnos, recursosTurno);

    // Lê os índices dos monstros
    vector<int> indicesMonstros(numMonstros);
    for (int& indice : indicesMonstros) {
        if (scanf("%d", &indice) != 1) {
            cerr << "Erro na leitura dos índices dos monstros.\n";
            return 1;
        }
        indice--;  // Ajusta o índice para ser base 0
    }

    // Lê as conexões do grafo
    for (int i = 0; i < numConexoes; ++i) {
        int de, para, peso;
        if (scanf("%d %d %d", &de, &para, &peso) != 3) {
            cerr << "Erro na leitura das conexões.\n";
            return 1;
        }
        de--; para--;  // Ajusta índices para serem base 0
        jogo.grafo->adjList[de].emplace_back(para, peso);
        jogo.grafo->adjListInversa[para].emplace_back(de);
    }

    // Executa BFS para encontrar trajetos mínimos
    jogo.grafo->bfs(0);  // Assume-se que o vértice inicial é 0

    // Processa os monstros e calcula os trajetos e bloqueios
    jogo.processarMonstros(indicesMonstros);

    // Executa o algoritmo de Dijkstra para encontrar o caminho de custo mínimo
    auto resultado = jogo.grafo->dijkstra(0, maxTurnos, recursosTurno, jogo.bloqueios);

    // Imprime o resultado da busca
    cout << (resultado.first == -1 ? 0 : 1) << endl;
    
    // Imprime os trajetos dos monstros
    for (const auto& trajeto : jogo.trajetosMonstros) {
        cout << trajeto.size() << " ";
        for (size_t i = 0; i < trajeto.size(); ++i) {
            cout << trajeto[i] + 1 << (i == trajeto.size() - 1 ? '\n' : ' ');
        }
    }

    // Se um caminho foi encontrado, imprime-o
    if (resultado.first != -1) {
        jogo.imprimirCaminho(resultado);
    } else {
        // Considera o caso onde não há caminho válido
        // Ajuste adicional para encontrar e imprimir o caminho modificado
        int maxVertice = 0, maxTurno = 0;
        for (const auto& entry : jogo.grafo->distDijkstra) {
            if (entry.second == INF) continue;
            auto [vertex, turno] = entry.first;
            if (turno > maxTurno || (turno == maxTurno && entry.second < jogo.grafo->distDijkstra[{maxVertice, maxTurno}])) {
                maxVertice = vertex;
                maxTurno = turno;
            }
        }

        if (jogo.bloqueios.count({maxVertice, maxTurno + 1})) {
            jogo.grafo->distDijkstra[{maxVertice, maxTurno + 1}] = jogo.grafo->distDijkstra[{maxVertice, maxTurno}] + 1;
            jogo.grafo->antecessorDijkstra[{maxVertice, maxTurno + 1}] = {maxVertice, maxTurno};
            maxTurno++;
        }

        jogo.imprimirCaminho({maxVertice, maxTurno});
    }

    return 0;
}
