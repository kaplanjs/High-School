#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define TYPE 371293 //13^5
#define STRAIGHT_FLUSH 8*TYPE
#define FOUR_KIND 7*TYPE
#define FULL_HOUSE 6*TYPE
#define FLUSH 5*TYPE
#define STRAIGHT 4*TYPE
#define THREE_KIND 3*TYPE
#define TWO_PAIR 2*TYPE
#define ONE_PAIR 1*TYPE
#define HIGH_CARD 0

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::max;
using std::min;

typedef vector<int> VI;

int quicksort_part(vector<int>& a, vector<int>& og, int low, int high) {   //partition for quicksort
    int median = (a[high] > a[low] ? min(a[high], max(a[low], a[(high + low)/2])) : max(a[high], min(a[low], a[(high + low)/2])));
        //return median of a[low], a[high], a[(low + high)/2]
    while(low <= high) {
        while(a[low] < median) ++low;
        while(a[high] > median) --high;
        if(low <= high) {   //make same changes to original vector (og) as to sorting vector (a)
            int temp = a[low];
            int temp_og = og[low];
            a[low] = a[high];
            og[low++] = og[high];
            a[high] = temp;
            og[high--] = temp_og;
        }
    }
    return low;
}

void quicksort(vector<int>& a, vector<int>& og, int low, int high) {   //chocolate quicksort
    while(low < high) {
        int m = quicksort_part(a, og, low, high);
        if(m - low < high - m + 1) {
            quicksort(a, og, low, m-1);
            low = m;
        } else {
            quicksort(a, og, m, high);
            high = m - 1;
        }
    }
}

void poker_sort(vector<int>& a) {
    vector<int> sort_vec(a.size()); //initialize sorting vector (ordered hands corresponding to original hands in vector (a))
    for(int index = 0; index < a.size(); ++index) {   //identify type of each hand
        div_t temp = div(a[index], 52);
        vector<int> hand(5, temp.rem);
        for(int i = 1; i < 5; ++i) {    //determine each card in hand
            temp = div(temp.quot, 52);
            hand[i] = temp.rem;
        }
        vector<int> rank_count(13, 0);
        for(int i = 0; i < 5; ++i) {    //create frequency vector for each possible card rank
            ++rank_count[hand[i]/4];
        }
        int mode_freq = 1;
        int mode = 0;
        for(int i = 0; i < 13; ++i) {
            if(rank_count[i] >= mode_freq) {    //determine mode of hand and number of occurrences of mode
                mode_freq = rank_count[i];
                mode = i;
            }
        }
        int ord_hand = 0;   //order cards in hand by priority (in base 13, first digit designates type of hand)
        if(mode_freq > 1) {
            for(int i = 0; i < mode_freq; ++i) {
                ord_hand += mode;
                ord_hand *= 13;
            }
        }
        switch(mode_freq) { //use frequency of mode to determine type of hand
            case 4: //four of a kind
                for(int i = 0; i < 13; ++i) {   //add fifth card to hand
                    if(rank_count[i] == 1) {
                        ord_hand += i;
                        break;
                    }
                }
                ord_hand += FOUR_KIND;
                break;
            case 3: {   //three of a kind, full house
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie full house), add cards to hand
                        ord_hand *= 13;
                        ord_hand += 14*i + FULL_HOUSE;
                        mode_freq = 0;  //skip counting as three of a kind
                        break;
                    }
                }
                if(mode_freq) { //if no second mode (ie three of a kind)
                    int j = 2;
                    for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                        if(rank_count[i] && i != mode) {
                            ord_hand += i;
                            if(--j) ord_hand *= 13;

                        }
                    }
                    ord_hand += THREE_KIND;
                }
                break;
            } case 2: { //two pair, one pair
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie two pair), add cards to hand
                        ord_hand *= 13;
                        ord_hand += 14*i;
                        for(int j = 0; j < 13; ++j) {   //add last card to hand
                            if(rank_count[j] == 1) {
                                ord_hand *= 13;
                                ord_hand += j + TWO_PAIR;
                                break;
                            }
                        }
                        mode_freq = 0;  //skip counting as one pair
                        break;
                    }
                }
                if(mode_freq) { //if no second mode (ie one pair)
                    int j = 3;
                    for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                        if(rank_count[i] && i != mode) {
                            ord_hand += i;
                            if(--j) ord_hand *= 13;

                        }
                    }
                    ord_hand += ONE_PAIR;
                }
                break;
            } default: {    //if no repeated cards: straight, flush, straight flush
                bool check_flush = 1;
                int suit = hand[0]%4;
                for(int i = 1; i < 5; ++i) {    //compare suit of first card to other cards
                    if(hand[i]%4 != suit) { //not a flush if any suits don't match
                        check_flush = 0;
                        break;
                    }
                }
                int start = 0;
                while(!rank_count[start]) ++start;  //find rank of lowest card
                int finish = 12;
                while(!rank_count[finish]) --finish;    //find rank of highest card
                if(finish - start == 4) {   //no repeated cards, rank range of 4 --> straight
                    ord_hand += finish;
                    while(finish && rank_count[--finish]) {   //add cards in descending order
                        ord_hand *= 13;
                        ord_hand += finish;
                    }
                    if(check_flush) ord_hand += STRAIGHT_FLUSH; //straight + flush --> straight flush
                    else ord_hand += STRAIGHT;  //otherwise a straight
                    break;
                } else if(finish - start == 12) {   //special case straight starting with ace (A, 2, 3, 4, 5)
                    do {} while(!rank_count[--finish]);
                    if(finish - start == 3) {   //if rank range is 12, check if lowest four cards form a (sub)straight
                        ord_hand += 90258;  //add cards (only possible case, (3, 2, 1, 0, 12) in base 13)
                        if(check_flush) ord_hand += STRAIGHT_FLUSH;
                        else ord_hand += STRAIGHT;
                        break;
                    }
                }
                int j = 5;
                for(int i = 12; i >= 0; --i) {  //add cards in descending order
                    if(rank_count[i]) {
                        ord_hand += i;
                        if(--j) ord_hand *= 13;
                    }
                }
                if(check_flush) ord_hand += FLUSH;  //flush if flush but not straight
                else ord_hand += HIGH_CARD; //otherwise high card (no repeated cards, flush, or straight)
                break;
            }
        }
        sort_vec[index] = ord_hand; //add hand to sorting vector
        hand.clear();   //clear vectors after use to reserve memory
        rank_count.clear();
    }
    quicksort(sort_vec, a, 0, a.size() - 1);    //sort hands
    /*
    for(int cards : sort_vec) { //print cards in each hand
        div_t temp = div(cards, 13);
        cout << temp.rem << ", ";
        for(int i = 1; i < 5; ++i) {
            temp = div(temp.quot, 13);
            cout << temp.rem;
            if(i < 4) cout << ", ";
            else cout << endl;
        }
    }
    */
}

/*
int main() {
    vector<int> v;
    int quantity = 1000000;  //number of hands to sort
    long long max_val = 52*52*52*52*52 - 1; //max value is all aces of spades (ID 51)
    srand((unsigned)time(0));   //seed random number generator with current time
    for(int i = 0; i < quantity; ++i) v.push_back(max_val*rand()/(RAND_MAX + 1.0)); //add random hand IDs to vector
    //cout << "The correct order of ";
    //for(int i = 0; i < v.size() - 1; ++i) cout << v[i] << ", ";   //print hands in initial order
    //cout << v.back() << " is:" << endl;
    poker_sort(v);  //sort hands
    //for(int i = 0; i < v.size() - 1; ++i) cout << v[i] << ", ";   //print hands in sorted order
    //cout << v.back() << endl;
    return 0;
}
*/
/*
int main() {
    srand(time(0));

    int n = 1000000;

    vector<int> d(52);
    for(int i = 0; i < 52; ++i) d[i] = i;

    vector<int> a(n);
    for(int i = 0; i < n; ++i) {
        vector<int> h(5);
        int hand = 0;
        for(int j = 0; j < 5; ++j) {
            int idx = rand()%d.size();
            h[j] = d[idx];
            d.erase(d.begin() + idx);
            hand *= 52;
            hand += h[j];
        }
        a[i] = hand;
        for(int j = 0; j < 5; ++j) d.push_back(h[j]);
    }

    clock_t start = std::clock();
    poker_sort(a);
    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

    cout << duration << "seconds\n";
}
*/
/*
using namespace std;
#include<fstream>
#include<ctime>
#include<vector>
#include<iostream>
#include<map>

int main() {
  ifstream in("in.txt");

  int m, n, rr, v;
  in >> m;
  for (int mm = 0; mm < m; mm++) {
    in >> n;
    cout << n << " elements... ";
    vector<int> a(n);
    map<int, int> r;
    for (int i = 0; i < n; i++) {
      in >> a[i] >> rr;
      r[a[i]] = rr;
    }

    clock_t start = std::clock();
    poker_sort(a);
    cout << "\t"<<( std::clock() - start ) / (double) CLOCKS_PER_SEC << " seconds\n";
    cout << "\tvalidating...\t";
    for (int i = 0; i < n - 1; i++)
      if (r[a[i]] > r[a[i + 1]]) {
        cout << "\n\thand " << a[i] << " = (";
        v = a[i];
        for (int j = 0; j < 5; j++) {
          if (j) cout << "\t";
          cout << (v % 52) / 4 << " " << ((v % 4) == 0 ? 'A' : (v % 4) == 1 ? 'B' : (v % 4) == 2 ? 'C' : 'D');
          v /= 52;
        }
        cout << ") is better than hand " << a[i + 1] << " = (";
        v = a[i+1];
        for (int j = 0; j < 5; j++) {
          if (j) cout << "\t";
          cout << (v % 52) / 4 << " " << ((v % 4) == 0 ? 'A' : (v % 4) == 1 ? 'B' : (v % 4) == 2 ? 'C' : 'D');
          v /= 52;
        }
        cout << ") but you put these in the sorted array in the opposite order\n";
        return -1;
      }
    cout << "valid!\n";
  }
}
*/
