#include <algorithm>
#include <climits>
#include <cstdio>
#include <list>
#include <set>
#include <utility>
#include <vector>

using namespace std;

// Class to represent a vertex in the graph
class Vertex {
public:
    int discoveryTime; // Time when the vertex was discovered during DFS
    int lowLink; // Lowest discovery time reachable from the vertex
    Vertex() : discoveryTime(0), lowLink(INT_MAX) {}

    void updateDiscoveryTime(int newTime) {
        discoveryTime = newTime;
    }

    void updateLowLink(int newLow) {
        lowLink = min(lowLink, newLow);
    }
};

vector<Vertex> vertices; 
set<int> links; // Store edge links in a set, as they need to be printed in ascending order
int nClusters = 0;

typedef pair<int, int> Edge;

list<Edge> EdgeList;
set<set<int>> clusters; // Clusters must be sorted lexicographically. Ou seja, se temos os clusters a = {1, 3, 7} e b = {1, 2, 100}, o cluster b deve ser listado antes do cluster a, e, portanto, possuir um identificador menor.
set<set<int>> forest;
typedef set<int> VertexSet;

// Function to identify and retrieve clusters in the graph
void findClusters(int u, int v) {
    VertexSet clusterVertices;
    list<Edge> tempEdgeList; // Temporary stack to hold the edges of the current cluster

    // Pop edges from EdgeList until we find the edge that starts the current cluster
    while (!EdgeList.empty() && (EdgeList.back().first != u || EdgeList.back().second != v)) {
        tempEdgeList.push_front(EdgeList.back()); // Move the edge to the temporary stack
        EdgeList.pop_back();
    }

    // If we found the starting edge, add it to the temporary stack
    if (!EdgeList.empty()) {
        tempEdgeList.push_front(EdgeList.back());
        EdgeList.pop_back();
    }

    // Now, tempEdgeList contains the edges of the current cluster in reverse order
    // Add Vertexs to the cluster in the correct order
    while (!tempEdgeList.empty()) {
        Edge edge = tempEdgeList.front();
        tempEdgeList.pop_front();
        clusterVertices.insert(edge.first);
        clusterVertices.insert(edge.second);
    }

    // Add the cluster to the set of clusters
    clusters.insert(clusterVertices);
    nClusters++;
}

// Depth-first search to identify articulation points and clusters
void dfs(vector<vector<int>> &Graph, int indexVertex, int parent, int time) {
    time++;
    vertices[indexVertex].updateDiscoveryTime(time); 
    vertices[indexVertex].updateLowLink(time); 
    int children = 0;
    vector<int>& adjacentes = Graph[indexVertex];

    for(int i = 0; i < (int)adjacentes.size(); i++) {
        if(vertices[adjacentes[i]-1].discoveryTime == 0) { 
            children++;
            EdgeList.push_back(make_pair(indexVertex+1, adjacentes[i])); 

            dfs(Graph, adjacentes[i]-1, indexVertex+1, time);

            if(vertices[adjacentes[i]-1].lowLink < vertices[indexVertex].lowLink) {
                vertices[indexVertex].updateLowLink(vertices[adjacentes[i]-1].lowLink);
            }
            
            if(parent != -1 && vertices[adjacentes[i]-1].lowLink >= vertices[indexVertex].discoveryTime) {
                links.insert(indexVertex+1);               
            }

            if((vertices[indexVertex].discoveryTime == 1 && children > 1) || (vertices[indexVertex].discoveryTime > 1 && vertices[adjacentes[i]-1].lowLink >= vertices[indexVertex].discoveryTime)) {
                findClusters(indexVertex+1, adjacentes[i]);
            }

        } else if(adjacentes[i] != parent) { 
            if(vertices[adjacentes[i]-1].discoveryTime < vertices[indexVertex].lowLink) {
                vertices[indexVertex].updateLowLink(vertices[adjacentes[i]-1].discoveryTime);
            }
            if(vertices[adjacentes[i]-1].discoveryTime < vertices[indexVertex].discoveryTime) {
                EdgeList.push_back(make_pair(indexVertex+1, adjacentes[i]));
            }
        }
    }

    if(parent == -1 && children > 1) {
        links.insert(indexVertex+1);      
    }
}


int main() {
    int nVertexs, nEdges;
    int time = 0;
    if (scanf("%d %d", &nVertexs, &nEdges) != 2) {
        fprintf(stderr, "Error reading nVertexs and nEdges\n");
        return 1;
    }
    vector<vector<int>> Graph(nVertexs);
    vertices.resize(nVertexs);

    // Read edges of the graph
    for (int i = 0; i < nEdges; i++) {
        int Vertex1, Vertex2;
        if (scanf("%d %d", &Vertex1, &Vertex2) != 2) {
            fprintf(stderr, "Error reading Vertex1 and Vertex2 at iteration %d\n", i);
            return 1;
        }

        Graph[Vertex1 - 1].push_back(Vertex2);
        Graph[Vertex2 - 1].push_back(Vertex1);
    }

    // Perform DFS to identify articulation points and clusters
    for (int i = 0; i < nVertexs; i++) {
        if (vertices[i].discoveryTime == 0) {
            dfs(Graph, i, -1, time);
        }
        if (!EdgeList.empty()) {
            set<int> clusterVertices;
            while (!EdgeList.empty()) {
                clusterVertices.insert(EdgeList.back().first);
                clusterVertices.insert(EdgeList.back().second);
                EdgeList.pop_back();
            }
            clusters.insert(clusterVertices);
            nClusters++;
        }
    }

    // Output the number of articulation points
    printf("%d\n", (int)links.size());
    for(set<int>::iterator itr = links.begin(); itr != links.end(); itr++) {
        printf("%d\n", *itr);
    }

    // Output the number of clusters
    printf("%d\n", nClusters);

    int i = 1;
    int j = 1;
    int nEdgesForest = 0;

    // Output clusters
    set<set<int>>::iterator itr1;
    set<int>::iterator itr;
    for(itr1 = clusters.begin(); itr1 != clusters.end(); itr1++) {
        printf("%d %lu ", nVertexs+i, itr1->size());

        itr = itr1->begin(); 
        while (itr != itr1->end()) {
            if(links.find(*itr) != links.end()) {
                set<int> elemForest;
                elemForest.insert(nVertexs+i);
                elemForest.insert(*itr);
                forest.insert(elemForest);
                nEdgesForest++;
            }
            if(j == (int)itr1->size()) printf("%d\n", *itr);
            else printf("%d ", *itr);
            j++;
            itr++;
        }
        j = 1;
        i++;
    }

    // Output forest
    printf("%d %d\n", nClusters+(int)links.size(), nEdgesForest);
    for(itr1 = forest.begin(); itr1 != forest.end(); itr1++) {
        printf("%d %d\n", *(itr1->begin()), *(itr1->rbegin()));
    }

    return 0;
}

