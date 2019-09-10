#include <iostream>
#include <vector>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

typedef vector<int> VI;
typedef vector<long long> VL;

vector<VL> n_hanoi;
vector<VI> k_hanoi;

void hanoi_init() {
    k_hanoi.assign(11, {}); //set size of grid to 11xNULL (0 to 10 towers)
    n_hanoi.assign(11, {});
    k_hanoi[3] = {0, 0};    //first two elements of row 3 for k_hanoi, n_hanoi
    n_hanoi[3] = {0, 1};
    for(int d = 2; d <= 10000; ++d) {   //3 towers, d disks:
        //k_hanoi[3].push_back(d - 1);    //all but 1 disk to spare
        //if(n_hanoi[3].size() == d && 2*n_hanoi[3][d - 1] + 1 > 0) n_hanoi[3].push_back(2*n_hanoi[3][d - 1] + 1);  //d^2 - 1 moves
        if(2*n_hanoi[3][d - 1] + 1 > 0) {
            k_hanoi[3].push_back(d - 1);    //all but 1 disk to spare
            n_hanoi[3].push_back(2*n_hanoi[3][d - 1] + 1);  //d^2 - 1 moves
        } else {
            d = 10001;   //if overflow: stop adding elements to row
            //k_hanoi[3].resize(10001);
            //n_hanoi[3].resize(10001);
        }
    }
    for(int t = 4; t < 11; ++t) {
        k_hanoi[t] = {0, 0};    //first two elements of row t for k_hanoi, n_hanoi
        n_hanoi[t] = {0, 1};
        for(int d = 2; d <= 10000; ++d) {   //t towers, d disks:
            k_hanoi[t].push_back(k_hanoi[t][d - 1]);    //k_hanoi (e value) for d disks same as for d - 1
            n_hanoi[t].push_back(2*n_hanoi[t][k_hanoi[t][d]] + n_hanoi[t - 1][d - k_hanoi[t][d]]);
            if(n_hanoi[t][d] - 2*n_hanoi[t][k_hanoi[t][d] + 1] - n_hanoi[t - 1][d - k_hanoi[t][d] - 1] > 0) {
                //check if incrementing k_hanoi is more efficient
                ++k_hanoi[t][d];    //if so, increment k_hanoi
                n_hanoi[t][d] = 2*n_hanoi[t][k_hanoi[t][d]] + n_hanoi[t - 1][d - k_hanoi[t][d]];
                    //define n_hanoi by new k_hanoi
            }
            if(n_hanoi[t][d] < 0) { //if overflow: remove most recent element, stop adding elements to row
                k_hanoi[t].pop_back();
                n_hanoi[t].pop_back();
                d = 10001;
                //k_hanoi[t].resize(10001, 0);
                //n_hanoi[t].resize(10001, 0);
            }
        }
    }

}

void OGhanoi(vector<VI>& moves, int d, int f, int t, int s) {   //hanoi for 3 towers, d disks
    if(d) {
        OGhanoi(moves, d - 1, f, s, t); //top (d - 1) to spare
        moves.push_back({f, t});    //bottom disk to destination
        OGhanoi(moves, d - 1, s, t, f); //(d - 1) from spare to destination
    }
}

void hanoi(vector<VI>& moves, int n_twrs, int n_dsks, VI& aux){
    if(n_dsks){
        if(n_dsks > k_hanoi[n_twrs].size() || n_twrs > k_hanoi.size()) {    //check valid inputs, no overflow
            cerr << "Invalid input. Make sure 2 < n_twrs < 11, 0 < n_dsks < 10001, and number of moves doesn't overflow." << endl;
            exit(-1);
        }
        else if(n_twrs > 3) {

            int spare = aux[n_twrs - 1];    //last available tower is spare
            aux.pop_back();
            aux.insert(aux.begin() + 1, spare); //spare becomes destination
            hanoi(moves, n_twrs, k_hanoi[n_twrs][n_dsks], aux); //move top k_hanoi towers to spare

            aux.erase(aux.begin() + 1); //(n - 1) towers to move (n_dsks - k_hanoi) disks
            hanoi(moves, n_twrs - 1, n_dsks - k_hanoi[n_twrs][n_dsks], aux);    //move remaining disks to destination

            aux.insert(aux.begin() + 2, aux[0]);    //spare becomes origin
            aux[0] = spare;
            hanoi(moves, n_twrs, k_hanoi[n_twrs][n_dsks], aux); //move top k_hanoi towers from spare to destination
            aux[0] = aux[2];    //reset aux
            aux.erase(aux.begin() + 2);
            aux.push_back(spare);

        } else {    //when only 3 towers available
            OGhanoi(moves, n_dsks, aux[0], aux[1], aux[2]); //record moves using original hanoi algorithm
        }
    }
}

/*
using std::string;

int main() {
    int twrs = 8, dsks = 324;
    hanoi_init();   //generate k_hanoi (e values) and n_hanoi (number of moves) grids
    vector<VI> moves;
    vector<int> aux;
    for(int i = 0; i < twrs; ++i) { //initialize aux (vector of towers) to {0, 1, ..., twrs - 1}
        aux.push_back(i);
    }
    vector<VI> tester;
    tester.assign(10, {});  //initialize tester vector (each element a vector representing a tower)
    for(int i = 0; i < dsks; ++i) { //add dsks disks to first tower
        tester[0].push_back(i);
    }
    hanoi(moves, twrs, dsks, aux);  //generate vector of moves
    for(int i = 0; i < n_hanoi[twrs][dsks]; ++i) {
        //cout << moves[i][0] + 1 << " ==> " << moves[i][1] + 1 << endl;  //print moves to console ("f ==> t" format)
        if(!tester[moves[i][0]].size() || tester[moves[i][1]].size() && tester[moves[i][0]].back() < tester[moves[i][1]].back()) {
            //check if each move is valid (disk exists in start tower and is smaller than in final tower)
            cerr << "ERROR: Move " << i << " is invalid!" << endl;
            return 0;
        } else {
            tester[moves[i][1]].push_back(tester[moves[i][0]].back());  //add top disk of start tower to top of final tower
            tester[moves[i][0]].pop_back(); //remove top disk from start tower
        }
    }
    if(dsks - tester[1].size()) {   //check if not all disks in final tower
        cerr << "ERROR: Not all disks moved to final tower!" << endl;
        return 0;
    } else {
        for(int i = 0; i < dsks; ++i) {
            if(tester[1][i] != i) { //check if disks in wrong order in final tower
                cerr << "ERROR: Disks in wrong order in final tower!" << endl;
                return 0;
            }
        }
        cout << "All clear! :)" << endl;
        //cout << string(18, ' ') << string(1, 'x') <<"\n"; cout << string(17, ' ') << string(3, 'x') <<"\n"; cout << string(16, ' ') << string(5, 'x') <<"\n"; cout << string(15, ' ') << string(7, 'x') <<"\n"; cout << string(15, ' ') << string(7, 'x') <<"\n"; cout << string(13, ' ') << string(11, 'x') <<"\n"; cout << string(9, ' ') << string(20, 'x') << string(10, ' ') << string(1, 'H') << string(1, 'A') << string(2, 'P') << string(1, 'Y') <<"\n"; cout << string(12, ' ') << string(6, '/') << string(6, '\\') << string(13, ' ') << string(1, 'H') << string(1, 'A') << string(2, 'L') << string(1, 'O') << string(1, 'W') << string(2, 'E') << string(1, 'N') <<"\n"; cout << string(11, ' ') << string(3, '/') << string(2, ' ') << string(1, 'O') << string(3, ' ') << string(1, 'O') << string(2, ' ') << string(2, '\\') << string(14, ' ') << string(1, 'D') << string(1, 'U') << string(1, 'D') << string(1, 'E') <<"\n"; cout << string(11, ' ') << string(3, '/') << string(4, ' ') << string(1, 'U') << string(4, ' ') << string(3, '\\') <<"\n"; cout << string(11, ' ') << string(2, '/') << string(3, ' ') << string(1, '\\') << string(3, '_') << string(1, '/') << string(3, ' ') << string(2, '\\') << string(9, ' ') << string(1, '/') << string(1, '\\') << string(7, ' ') << string(1, '.') <<"\n"; cout << string(14, ' ') << string(1, 'a') << string(7, ' ') << string(1, 'a') << string(12, ' ') << string(1, '\\') << string(2, ' ') << string(1, '\\') << string(1, ' ') << string(2, '_') << string(1, ' ') << string(1, '/') << string(1, '|') <<"\n"; cout << string(12, ' ') << string(5, 'a') << string(3, ' ') << string(5, 'a') << string(12, ' ') << string(1, 'o') << string(3, ' ') << string(1, 'o') <<"\n"; cout << string(9, ' ') << string(19, 'a') << string(7, ' ') << string(3, ':') << string(1, ' ') << string(1, '@') << string(1, ' ') << string(3, ':') <<"\n"; cout << string(8, ' ') << string(7, 'a') << string(2, ' ') << string(4, 'a') << string(3, ' ') << string(6, 'a') << string(7, ' ') << string(1, '\\') << string(1, '/') << string(1, ' ') << string(1, '\\') << string(1, '/') << string(4, ' ') << string(1, '\\') << string(5, ' ') << string(1, 'M') << string(2, 'e') << string(1, '-') << string(1, ' ') << string(1, 'e') << string(2, 'o') << string(1, 'W') << string(1, ' ') << string(1, '!') <<"\n"; cout << string(7, ' ') << string(5, 'a') << string(15, ' ') << string(4, 'a') << string(9, ' ') << string(1, '\\') << string(4, ' ') << string(3, '\\') <<"\n"; cout << string(6, ' ') << string(5, 'a') << string(2, ' ') << string(5, 'a') << string(1, ' ') << string(7, 'a') << string(2, ' ') << string(3, 'a') << string(10, ' ') << string(1, '\\') << string(3, ' ') << string(2, '#') << string(2, ' ') << string(1, '\\') <<"\n"; cout << "\n"; cout << string(70, '#') <<"\n"; cout << "\n"; cout << string(1, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') <<"\n"; cout << string(4, '#') << string(2, ' ') << string(3, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(2, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(2, '#') << string(2, ' ') << string(3, '#') <<"\n"; cout << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(4, ' ') << string(1, '#') << string(6, ' ') << string(1, '#') <<"\n"; cout << "\n"; cout << string(7, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(4, ' ') << string(2, '#') << string(2, ' ') << string(1, '#') << string(5, ' ') << string(1, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(1, '#') << string(2, ' ') << string(2, '#') <<"\n"; cout << string(7, ' ') << string(4, '#') << string(2, ' ') << string(3, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(2, '-') << string(2, ' ') << string(1, '#') << string(2, '-') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') <<"\n"; cout << string(7, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(1, ' ') << string(3, '#') << string(1, ' ') << string(3, '#') << string(2, ' ') << string(2, '#') << string(4, ' ') << string(3, '#') << string(3, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(2, '#') << string(2, ' ') << string(1, '#') <<"\n"; cout << "\n"; cout << string(71, '#') <<"\n";
    }
    return 0;
}
*/
/*
#include <fstream>

using namespace std;

int main() {
  hanoi_init();

  ifstream in ("in.txt");
  ifstream out ("out.txt");

  int n, k;
  in >> n >> k;
  for (int i = 0; i < n; i++) {
    int a, b;
    long long ans;
    in >> a >> b;
    out >> ans;
    if (n_hanoi.size() <= a || n_hanoi[a].size() <= b) {
      cout << "your code can't handle " << a << " pillars and " << b << " disks\n";
      return 3735928559;
    }
    if (n_hanoi[a][b] != ans) {
      cout << "number of moves to do " << a << " pillars and " << b << " disks is " << ans << ", but you gave " << n_hanoi[a][b] << "\n";
      return 3735928559;
    }
  }

  for (int i = 0; i < k; i++) {
    int a, b;
    long long x;
    in >> a >> b;
    out >> x;

    if (n_hanoi.size() <= a || n_hanoi[a].size() <= b) {
      cout << "your code can't handle " << a << " pillars and " << b << " disks\n";
      return 3735928559;
    }
    if (n_hanoi[a][b] != x) {
      cout << "number of moves to do " << a << " pillars and " << b << " disks is " << x << ", but you gave " << n_hanoi[a][b] << "\n";
      return 3735928559;
    }


    vector<VI> v;
    VI aux(a);
    for (int j = 0; j < aux.size(); j++)
      aux[j] = j;
    hanoi(v, a, b, aux);
    vector<vector<int> > p(a);
    for (int j=0;j<b;j++)
      p[0].push_back(b-j-1);
    for (int j = 0; j < v.size(); j++) {
      int f = v[j][0], t = v[j][1];
      if (!p[f].size() || p[t].size() && p[f].back()>p[t].back()){
        cout << "right number of moves but moves are wrong for " << a << " pillars and " << b << " disks\n";
        cout << "tried to make invalid move "<<f<<" ==> "<<t<<" move number "<<j<<"\n";
        return 3735928559;
      }

      p[t].push_back(p[f].back());
      p[f].resize(p[f].size()-1);
    }

    if (p[0].size() || p[1].size()!=b){
      cout << "right number of moves but moves are wrong for " << a << " pillars and " << b << " disks\n";
      cout << "not all pillars moved\n";
    }

  }

  cout << "all clear :D !!!\n";

  cout << string(18, ' ') << string(1, 'x') <<"\n"; cout << string(17, ' ') << string(3, 'x') <<"\n"; cout << string(16, ' ') << string(5, 'x') <<"\n"; cout << string(15, ' ') << string(7, 'x') <<"\n"; cout << string(15, ' ') << string(7, 'x') <<"\n"; cout << string(13, ' ') << string(11, 'x') <<"\n"; cout << string(9, ' ') << string(20, 'x') << string(10, ' ') << string(1, 'H') << string(1, 'A') << string(2, 'P') << string(1, 'Y') <<"\n"; cout << string(12, ' ') << string(6, '/') << string(6, '\\') << string(13, ' ') << string(1, 'H') << string(1, 'A') << string(2, 'L') << string(1, 'O') << string(1, 'W') << string(2, 'E') << string(1, 'N') <<"\n"; cout << string(11, ' ') << string(3, '/') << string(2, ' ') << string(1, 'O') << string(3, ' ') << string(1, 'O') << string(2, ' ') << string(2, '\\') << string(14, ' ') << string(1, 'D') << string(1, 'U') << string(1, 'D') << string(1, 'E') <<"\n"; cout << string(11, ' ') << string(3, '/') << string(4, ' ') << string(1, 'U') << string(4, ' ') << string(3, '\\') <<"\n"; cout << string(11, ' ') << string(2, '/') << string(3, ' ') << string(1, '\\') << string(3, '_') << string(1, '/') << string(3, ' ') << string(2, '\\') << string(9, ' ') << string(1, '/') << string(1, '\\') << string(7, ' ') << string(1, '.') <<"\n"; cout << string(14, ' ') << string(1, 'a') << string(7, ' ') << string(1, 'a') << string(12, ' ') << string(1, '\\') << string(2, ' ') << string(1, '\\') << string(1, ' ') << string(2, '_') << string(1, ' ') << string(1, '/') << string(1, '|') <<"\n"; cout << string(12, ' ') << string(5, 'a') << string(3, ' ') << string(5, 'a') << string(12, ' ') << string(1, 'o') << string(3, ' ') << string(1, 'o') <<"\n"; cout << string(9, ' ') << string(19, 'a') << string(7, ' ') << string(3, ':') << string(1, ' ') << string(1, '@') << string(1, ' ') << string(3, ':') <<"\n"; cout << string(8, ' ') << string(7, 'a') << string(2, ' ') << string(4, 'a') << string(3, ' ') << string(6, 'a') << string(7, ' ') << string(1, '\\') << string(1, '/') << string(1, ' ') << string(1, '\\') << string(1, '/') << string(4, ' ') << string(1, '\\') << string(5, ' ') << string(1, 'M') << string(2, 'e') << string(1, '-') << string(1, ' ') << string(1, 'e') << string(2, 'o') << string(1, 'W') << string(1, ' ') << string(1, '!') <<"\n"; cout << string(7, ' ') << string(5, 'a') << string(15, ' ') << string(4, 'a') << string(9, ' ') << string(1, '\\') << string(4, ' ') << string(3, '\\') <<"\n"; cout << string(6, ' ') << string(5, 'a') << string(2, ' ') << string(5, 'a') << string(1, ' ') << string(7, 'a') << string(2, ' ') << string(3, 'a') << string(10, ' ') << string(1, '\\') << string(3, ' ') << string(2, '#') << string(2, ' ') << string(1, '\\') <<"\n"; cout << "\n"; cout << string(70, '#') <<"\n"; cout << "\n"; cout << string(1, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') <<"\n"; cout << string(4, '#') << string(2, ' ') << string(3, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(2, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(2, '#') << string(2, ' ') << string(3, '#') <<"\n"; cout << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(4, ' ') << string(1, '#') << string(6, ' ') << string(1, '#') <<"\n"; cout << "\n"; cout << string(7, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(4, ' ') << string(2, '#') << string(2, ' ') << string(1, '#') << string(5, ' ') << string(1, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(1, '#') << string(2, ' ') << string(2, '#') <<"\n"; cout << string(7, ' ') << string(4, '#') << string(2, ' ') << string(3, '#') << string(2, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(2, '-') << string(2, ' ') << string(1, '#') << string(2, '-') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') <<"\n"; cout << string(7, ' ') << string(1, '#') << string(2, ' ') << string(1, '#') << string(1, ' ') << string(1, '#') << string(3, ' ') << string(1, '#') << string(1, ' ') << string(3, '#') << string(1, ' ') << string(3, '#') << string(2, ' ') << string(2, '#') << string(4, ' ') << string(3, '#') << string(3, ' ') << string(4, '#') << string(1, ' ') << string(4, '#') << string(1, ' ') << string(2, '#') << string(2, ' ') << string(1, '#') <<"\n"; cout << "\n"; cout << string(71, '#') <<"\n";

  in.close();
  out.close();
  return 0;
}
*/
