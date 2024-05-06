#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <tuple>
#include <limits>

using namespace std;

const int INF = numeric_limits<int>::max();

// Variáveis globais
int numEspacos, numConexoes, numMonstros, maxTurnos, recursosTurno;
vector<vector<pair<int, int>>> adjList;     // Lista de adjacência para armazenar os pesos e conexões entre espaços.
vector<vector<int>> adjListInversa;         // Lista de adjacência inversa para uso na busca em largura.
vector<vector<int>> trajetosMonstros;       // Armazena os trajetos dos monstros.
set<pair<int, int>> bloqueios;              // Conjunto de espaços e turnos bloqueados pelos monstros.

vector<int> antecessor;
vector<int> distBFS;

map<pair<int, int>, pair<int, int>> antecessorDijkstra; // Mapa para antecessor no Dijkstra.
map<pair<int, int>, int> distDijkstra;                  // Mapa para distâncias no Dijkstra.
map<pair<int, int>, bool> visitado;                     // Mapa para controle de visitados no Dijkstra.

/**
 * Realiza a busca em largura (BFS) para determinar caminhos mínimos do vértice inicial para todos os outros.
 * 
 * @param inicio O vértice de onde a BFS começa.
 */
void bfs(int inicio = 0) {
    antecessor = distBFS = vector<int>(numEspacos, INF);    // Inicializa vetores com INF.
    antecessor[inicio] = inicio;                            // O próprio vértice é seu antecessor inicial.
    distBFS[inicio] = 0;                                    // Distância para si mesmo é zero.

    queue<int> fila;
    fila.push(inicio);
    while (!fila.empty()) {
        int vertice = fila.front();
        fila.pop();
        for (auto& adj : adjListInversa[vertice]) {
            if (distBFS[vertice] + 1 <= distBFS[adj]) {
                if (distBFS[vertice] + 1 < distBFS[adj]) {
                    distBFS[adj] = distBFS[vertice] + 1;
                    antecessor[adj] = vertice;
                    fila.push(adj);
                } else if (distBFS[vertice] + 1 == distBFS[adj] && vertice < antecessor[adj]) {
                    antecessor[adj] = vertice;
                }
            }
        }
    }
}

/**
 * Executa o algoritmo de Dijkstra no grafo para encontrar o caminho de custo mínimo.
 * 
 * @param inicio O vértice inicial do caminho.
 * @return Um par contendo o último vértice do caminho e o turno em que ele foi alcançado.
 */
pair<int, int> dijkstra(int inicio = 0) {
    distDijkstra[{inicio, 0}] = 0;
    antecessorDijkstra[{inicio, 0}] = {inicio, 0};

    priority_queue<tuple<int, int, int, int>> pq;   // Fila de prioridade para Dijkstra com peso negativo para min heap.
    pq.push({0, 0, inicio, 0});

    while (!pq.empty()) {
        auto [custo, recurso, vertice, turno] = pq.top();
        pq.pop();

        if (vertice == numEspacos - 1) {    // Verifica se chegou no último espaço.
            return {vertice, turno};
        }
        if (visitado[{vertice, turno}] || turno + 1 > maxTurnos) {  // Pula iterações desnecessárias.
            continue;
        }

        visitado[{vertice, turno}] = true;
        custo *= -1;
        recurso += recursosTurno;
        turno++;

        for (auto& [adj, peso] : adjList[vertice]) {
            if (!bloqueios.count({adj, turno - 1}) && !bloqueios.count({adj, turno})) {
                if (!distDijkstra.count({adj, turno})) {
                    distDijkstra[{adj, turno}] = INF;
                }

                if (recurso >= peso && distDijkstra[{vertice, turno - 1}] + peso < distDijkstra[{adj, turno}]) {
                    antecessorDijkstra[{adj, turno}] = {vertice, turno - 1};
                    distDijkstra[{adj, turno}] = distDijkstra[{vertice, turno - 1}] + peso;
                    pq.push({-distDijkstra[{adj, turno}], recurso - peso, adj, turno});
                }
            }
        }
    }

    return {-1, -1};    // Retorna -1 se não encontrar um caminho válido.
}

// Função para imprimir o caminho
void imprimirCaminho(pair<int, int> pos) {
    printf("%d %d\n", distDijkstra[pos], pos.second);

    vector<int> caminho;
    while (pos != antecessorDijkstra[pos]) {
        caminho.emplace_back(pos.first);
        pos = antecessorDijkstra[pos];
    }
    caminho.emplace_back(0);

    for (int i = caminho.size() - 1; i >= 0; --i) {
        printf("%d%c", caminho[i] + 1, i == 0 ? '\n' : ' ');
    }
}

int main() {
    // Leitura de entrada
    if (scanf("%d %d %d %d %d", &numEspacos, &numConexoes, &numMonstros, &maxTurnos, &recursosTurno) != 5) {
        cerr << "Erro na leitura dos dados iniciais.\n";
        return 1;
    }
    adjList = vector<vector<pair<int, int>>>(numEspacos);
    adjListInversa = vector<vector<int>>(numEspacos);
    vector<int> indicesMonstros(numMonstros);
    trajetosMonstros = vector<vector<int>>(numMonstros);

    for (auto& indice : indicesMonstros) {
        if (scanf("%d", &indice) != 1) {
            cerr << "Erro na leitura dos índices dos monstros.\n";
            return 1;
        }
        --indice;
    }

    for (int i = 0; i < numEspacos; ++i) {
        adjList[i].emplace_back(i, 1);  // Autoconexão para cada espaço.
    }

    for (int i = 0; i < numConexoes; ++i) {
        int de, para, peso;
        if (scanf("%d %d %d", &de, &para, &peso) != 3) {
            cerr << "Erro na leitura das conexões.\n";
            return 1;
        }
        --de;
        --para;
        adjList[de].emplace_back(para, peso);
        adjListInversa[para].emplace_back(de);
    }

    bfs();  // Executa BFS para encontrar trajetos mínimos.

    for (int i = 0; i < numMonstros; ++i) {
        int turnoBloqueio = 0;
        if (antecessor[indicesMonstros[i]] != INF) {
            int atual = indicesMonstros[i];
            while (true) {
                trajetosMonstros[i].emplace_back(atual);
                bloqueios.insert({atual, turnoBloqueio++});
                if (atual == 0) break;
                atual = antecessor[atual];
            }

            for (int turno = turnoBloqueio; turno <= maxTurnos; ++turno) {
                bloqueios.insert({0, turno});
            }
        } else {
            trajetosMonstros[i].emplace_back(indicesMonstros[i]);
            for (int turno = 0; turno <= maxTurnos; ++turno) {
                bloqueios.insert({indicesMonstros[i], turno});
            }
        }
    }

    pair<int, int> resultado = dijkstra();
    printf("%d\n", resultado.first == -1 ? 0 : 1);

    for (auto& trajeto : trajetosMonstros) {
        printf("%zu ", trajeto.size());
        for (size_t i = 0; i < trajeto.size(); ++i) {
            printf("%d%c", trajeto[i] + 1, i == trajeto.size() - 1 ? '\n' : ' ');
        }
    }

    if (resultado.first != -1) {
        imprimirCaminho(resultado);
    } else {
        int maxVertice = 0, maxTurno = 0;
        for (auto& entrada : distDijkstra) {
            if (entrada.second == INF) continue;
            if (entrada.first.second > maxTurno || (entrada.first.second == maxTurno && entrada.second < distDijkstra[{maxVertice, maxTurno}])) {
                maxVertice = entrada.first.first;
                maxTurno = entrada.first.second;
            }
        }

        if (bloqueios.count({maxVertice, maxTurno + 1})) {
            distDijkstra[{maxVertice, maxTurno + 1}] = distDijkstra[{maxVertice, maxTurno}] + 1;
            antecessorDijkstra[{maxVertice, maxTurno + 1}] = {maxVertice, maxTurno};
            maxTurno++;
        }

        imprimirCaminho({maxVertice, maxTurno});
    }

    return 0;
}
