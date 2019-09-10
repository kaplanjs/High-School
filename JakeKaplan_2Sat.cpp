#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::map;
using std::set;

struct Vertex {
    Vertex(): seen(false), deg(0), nbrs(), literals() {}
    void add_edge(int to) { nbrs.push_back(to); ++deg; }
    void check_add_edge(int to) {
        for(int i = 0; i < deg; ++i) {
            if(nbrs[i] == to) return;
        }
        nbrs.push_back(to);
        ++deg;
    }

    bool seen;
    int deg;    //number of edges
    vector<int> nbrs;   //which vertices each edge goes to
    set<int> literals;
};

typedef vector<Vertex> Graph;

Graph g;    //initial graph
Graph g_t;
Graph dag;
map<int, int> skew;
vector<int> reverse_skew;
vector<int> g_ordered;
vector<int> strong_comp_map;
vector<set<int>> dag_ordered;
vector<char> model;
int strong_comps = -1;
int cases_to_satisfy;
bool not_satisfiable = false;

int getGraph() {
    int nv, nc, u, v;
    cin >> nv >> nc;
    cases_to_satisfy = nv;
    g.resize(2*nv + 1);
    g_t.resize(2*nv + 1);
    reverse_skew.resize(2*nv + 1);
    strong_comp_map.resize(2*nv + 1);
    dag.resize(2*nv + 1);   //find better upper bound?
    model.resize(nv, 'X');
    g_ordered.reserve(2*nv + 1);
    for(int i = 1; i < nv + 1; ++i) {
        skew[i] = i;
        skew[-i] = 2*nv + 1 - i;
        reverse_skew[i] = i;
        reverse_skew[2*nv + 1 - i] = -i;
    }
    for(int i = 0; i < nc; ++i) { //add each edge to g
        cin >> u >> v;
        g[skew[-u]].add_edge(skew[v]);
        g[skew[-v]].add_edge(skew[u]);
        g_t[skew[v]].add_edge(skew[-u]);
        g_t[skew[u]].add_edge(skew[-v]);
    }
    return nv;
}

void ogTopOrder(int v) {
    g[v].seen = true;   //vertex visited
    for(int i = 0; i < g[v].deg; ++i) { //visit all neighbors of vertex
        int to = g[v].nbrs[i];
        if(!g[to].seen) ogTopOrder(to);  //only visit if not already visited
    }
    g_ordered.push_back(v);  //push vertices to g_ordered in postorder (reverse topological order)
}

void condense(int v) {
    if(dag[strong_comps].literals.find(-reverse_skew[v]) != dag[strong_comps].literals.end()) not_satisfiable = true;
    dag[strong_comps].literals.insert(reverse_skew[v]);
    strong_comp_map[v] = strong_comps;
    g_t[v].seen = true;
    for(int i = 0; i < g_t[v].deg; ++i) {
        int to = g_t[v].nbrs[i];
        if(!g_t[to].seen) condense(to);
        else if(strong_comp_map[to] != strong_comps) dag[strong_comp_map[to]].check_add_edge(strong_comps);
    }
}

void remixTopOrder(int v) {
    dag[v].seen = true;   //vertex visited
    for(int i = 0; i < dag[v].deg; ++i) { //visit all neighbors of vertex
        int to = dag[v].nbrs[i];
        if(!dag[to].seen) remixTopOrder(to);  //only visit if not already visited
    }
    dag_ordered.push_back(dag[v].literals);  //push literals in strong component to dag_ordered in postorder
}

int main() {
    int nv = getGraph();
    for(int i = 1; i < 2*nv + 1; ++i) {
        if(!g[i].seen) ogTopOrder(i);
    }
    for(int i = 2*nv - 1; i >= 0; --i) {
        if(!g_t[g_ordered[i]].seen) {
            ++strong_comps;
            condense(g_ordered[i]);
        }
        if(not_satisfiable) {
            cout << "Not satisfiable" << endl;
            return 0;
        }
    }
    dag_ordered.reserve(strong_comps + 1);
    for(int i = 0; i <= strong_comps; ++i) {
        if(!dag[i].seen) remixTopOrder(i);
    }
    for(set<int> literals : dag_ordered) {
        for(int literal : literals) {
            if(model[abs(literal) - 1] == 'X') {
                model[abs(literal) - 1] = (literal > 0 ? 'T' : 'F');
                if(!(--cases_to_satisfy)) break;
            }
        }
        if(!cases_to_satisfy) break;
    }
    for(int i = 0; i < nv; ++i) cout << model[i] << endl;
}
