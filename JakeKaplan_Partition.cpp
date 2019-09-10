#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

typedef unsigned long long ULL;
typedef vector<ULL> VULL;
typedef vector<int> VI;

vector<VULL> q_table;

ULL part_q_helper(int n, int b) {   //b = bound on next element of partition (to ensure partition is non-increasing)
    if(q_table[n][b]) return q_table[n][b]; //return value if already calculated previously
    ULL x = (n <= b);   //add 1 to x to account for i = n case unless bound restricts i
    for(int i = 1; i <= b && i < n; ++i) {
    //number of partitions with given kth term is sum of partitions for each possible (k+1)st term
        x += (q_table[n - i][i] ? q_table[n - i][i] : q_table[n - i][i] = part_q_helper(n - i, i));
        //add value to table if not already added
    }
    return x;
}

ULL part_q(int n) {
    n > 2 ? q_table.assign(n + 1, {}) : q_table.assign(3, {});  //initialize table with number of rows
    q_table[1].assign(n + 1, 1);    //exactly 1 partition of 1
    for(int i = 2; i <= n; ++i) {
        q_table[i].assign(n - i + 2, 0);    //initialize number of elements in each row
        q_table[i][1] = 1;  //only 1 partition if bound is 1
    }
    ULL x = 1;  //accounts for i = n case
    for(int i = 1; i < n; ++i) {    //number of partitions is sum of partitions for each possible 1st term
        x += part_q_helper(n - i, i);
    }
    return x;
}

VI part_k(int n, ULL k) {
    k = part_q(n) - k;  //how many partitions are after the kth partition? (work from last to kth partition)
    VI output;
    while(n) {
        int i = 1;
        while((n == i ? 1 : part_q_helper(n - i, i)) < k) {
        //mth term of partition is least i such that number of partitions with mth term = i doesn't exceed k
            k -= (n == i ? 1 : part_q_helper(n - i, i));
            ++i;
        }
        output.push_back(i);    //mth term of partition = i
        n -= i; //(m+1)st and later terms make a partition of (n - i), so repeat with n -= i for next term
    }
    return output;
}

ULL part_i(const VI& p) {
    int n = 0;
    ULL k = 0;
    for(int i = 0; i < p.size(); ++i) {
        n += p[i];  //n is (by definition) the sum of each element of p
    }
    part_q(n);  //generate table of part_q_helper values for reference
    for(int i = 0; i < p.size(); ++i) {
        for(int j = (i && p[i - 1] < n ? p[i - 1] : n); j > p[i]; --j) {
        //partition number is total number of partitions with greater than or equal terms
            k += (j == n ? 1 : part_q_helper(n - j, j));
        }
        n -= p[i];
        //number of partitions with first i terms equal and greater (i+1)st term depends on number of partitions of (n - p[i])
    }
    return k;
}
/*
int main() {
    //define test cases
    int n = 7, k = 10, m = 416;
    VI input{3, 1, 1, 1, 1};

    //print part_q
    cout << "part_q(" << n << ") = " << part_q(n) << endl;
    VI output = part_k(n, k);

    //print part_k
    cout << "part_k(" << n << ", " << k << ") = {";
    for(int i = 0; i < output.size() - 1; ++i) {
        cout << output[i] << ", ";
    }
    cout << output[output.size() - 1] << "}" << endl;

    //print part_i
    cout << "part_i({";
    for(int i = 0; i < input.size() - 1; ++i) {
        cout << input[i] << ", ";
    }
    cout << input[input.size() - 1] << "}) = " << part_i(input) << endl;

    //check that part_i is inverse of part_k
    cout << (part_q(m) == part_i(part_k(m, part_q(m) - 1)) + 1 ? "All Clear! :)" : "Error! :(") << endl;

    return 0;
}
*/
