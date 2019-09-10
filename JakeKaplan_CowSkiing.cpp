#include <iostream>
#include <vector>
#include <random>

using std::cout;
using std::cin;
using std::endl;
using std::vector;

struct Vertex {
    Vertex(): deg(0), nbrs() {}
    void add_edge(int to) { nbrs.push_back(to); ++deg; }

    int deg;    //number of edges
    vector<int> nbrs;   //which vertices each edge goes to
};

typedef vector<Vertex> Graph;

//without topological ordering

Graph g;    //initial graph
vector<int> vertexPaths;  //number of paths from a given vertex
int modulus = 1000000007;  //return answer in modulus 1000000007

int getGraph() {
    int N, E, u, v;
    cin >> N >> E;
    g.resize(N);
    for(int i = 0; i < E; ++i) { //add each edge to g
        cin >> u >> v;
        g[--u].add_edge(--v);
    }
    return N;
}

int countPaths(int v) {
    if(vertexPaths[v]) return vertexPaths[v];   //don't recalculate if number of paths already calculated
    if(!g[v].deg) return 1; //return 1 if at end of path
    for(int i = 0; i < g[v].deg; ++i) { //number of paths from vertex equal to sum of paths from children vertices
        int temp = countPaths(g[v].nbrs[i]);
        if(temp >= modulus - vertexPaths[v]) vertexPaths[v] -= modulus;
        vertexPaths[v] += temp;
    }
    return vertexPaths[v];
}

void numPaths() {
    vertexPaths.assign(getGraph(), 0);  //create graph and initialize DFS path counter
    cout << countPaths(0) << endl;
    /*
    int totalPaths = 0;
    for(int i = nv - 1; i >= 0; --i) {  //total paths equal to sum of paths from each starting point
        if(!vertexPaths[i] && slope[i].deg) {
            int temp = countPaths(i);
            if(temp >= modulus - totalPaths) totalPaths -= modulus;
            totalPaths += temp;
        }
    }
    cout << totalPaths << endl;
    */
}


//with topological ordering
/*
Graph g;    //initial graph
Graph slope;    //topologically ordered graph
vector<int> slopeMap;   //maps vertices in g to slope
vector<int> vertexPaths;  //number of paths from a given vertex
int modulus = 1000000007;  //return answer in modulus 2^30 - 1

int getGraph() {
    int N, E, u, v;
    cin >> N >> E;
    g.resize(N);
    for(int i = 0; i < E; ++i) { //add each edge to g
        cin >> u >> v;
        g[--u].add_edge(--v);
    }
    return N;
}

void visit(int v) {
    g[v].seen = true;   //vertex visited
    for(int i = 0; i < g[v].deg; ++i) { //visit all neighbors of vertex
        int to = g[v].nbrs[i];
        if(!g[to].seen) visit(to);  //only visit if not already visited
    }
    slope.push_back(g[v]);  //push vertices to slope in postorder (reverse topological order)
    slopeMap[v] = slope.size() - 1; //map vth vertex to its position in slope
}

int countPaths(int v) {
    if(vertexPaths[v]) return vertexPaths[v];   //don't recalculate if number of paths already calculated
    if(!slope[v].deg) return 1; //return 1 if at end of path
    for(int i = 0; i < slope[v].deg; ++i) { //number of paths from vertex equal to sum of paths from children vertices
        int temp = countPaths(slopeMap[slope[v].nbrs[i]]);
        if(temp >= modulus - vertexPaths[v]) vertexPaths[v] -= modulus;
        vertexPaths[v] += temp;
    }
    return vertexPaths[v];
}

void numPaths() {
    int N = getGraph();    //create graph, initialize other structures
    slope.reserve(N);
    slopeMap.assign(N, 0);
    vertexPaths.assign(N, 0);
    for(int i = 0; i < N; ++i) {   //topologically order graph while visiting vertices
        if(!g[i].seen) visit(i);
    }
    cout << countPaths(N - 1) << endl;
    /*
    int totalPaths = 0;
    for(int i = nv - 1; i >= 0; --i) {  //total paths equal to sum of paths from each starting point
        if(!vertexPaths[i] && slope[i].deg) {
            int temp = countPaths(i);
            if(temp >= modulus - totalPaths) totalPaths -= modulus;
            totalPaths += temp;
        }
    }
    cout << totalPaths << endl;

}
*/


int main() {
    numPaths();
}


/*
int main() {
    std::random_device seed;
    std::mt19937 gen;   //19937^2 + 1 is prime

    //int modulus = 1000000007;

    int nv;
    cout << "Enter a number of vertices for your test case:" << endl;
    cin >> nv;
    cout << "Copy and paste the following test case:" << endl;
    cout << nv << " " << nv*(nv - 1)/2 << endl;
    vector<int> p(nv, 1);
    p[0] = 0;
    p[nv - 1] = nv - 1;
    for(int i = 2; i < nv - 1; ++i) {
        auto u = std::uniform_int_distribution<int>(1, i);
        int j = u(gen);
        p[i] = p[j];
        p[j] = i;
    }
    for(int i = 1; i < nv; ++i) {
        for(int j = 0; j < i; ++j) {
            cout << p[j] + 1 << " " << p[i] + 1 << endl;
        }
    }
    numPaths(); //CHANGE THIS LINE TO THE NAME OF YOUR FUNCTION
    int testNum = 1;
    for(int i = 0; i < nv - 2; ++i) testNum = (testNum << 1)%modulus;
    cout << "Your code should have returned: " << testNum << endl;
}
*/
