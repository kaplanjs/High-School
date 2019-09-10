#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
//#include <algorithm>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
//using std::reverse;

typedef vector<int> VI;
typedef vector<VI> VVI;

void bucket_sort(vector<VI>& hands, vector<int> degree) {   //sorts by rank of highest priority (degree) card

    vector<VVI> bucket(13); //13 buckets, one for each rank
    while(hands.size()) {   //add each hand to a bucket based on rank of priority card
        bucket[hands.back()[degree.back()]].push_back(hands.back());
        hands.pop_back();
    }

    /*
    vector<VVI> bucket(13, VVI(hands.size(), VI(6))); //13 buckets, one for each rank, assume no more than a fifth have same card
    for(int i = 0; i < hands.size(); ++i) { //add each hand to a bucket based on rank of priority card
        for(int j = 0; j < 6; ++j) bucket[hands.back()[degree.back()]][i][j] = hands.back()[j];
        hands.pop_back();
    }
    */
    if(degree.size() > 1) { //if cards can be more finely distinguished, sort each bucket
        degree.pop_back();
        for(int i = 0; i < 13; ++i) {   //sort buckets with more than one hand
            if(bucket[i].size() > 1) bucket_sort(bucket[i], degree);
        }
    }
    for(int i = 0; i < 13; ++i) hands.insert(hands.begin(), bucket[i].begin(), bucket[i].end()); //combine buckets in order
}

void poker_sort(vector<int>& a) {
    vector<VI> straight_flush, four_kind, full_house, flush, straight, three_kind, two_pair, one_pair, high_card;
    while(a.size()) {   //identify type of each hand
        div_t temp = div(a.back(), 52);
        vector<int> hand{temp.rem};
        for(int i = 1; i < 5; ++i) {    //determine each card in hand
            temp = div(temp.quot, 52);
            hand.push_back(temp.rem);
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
        vector<int> ord_hand;   //order cards in hand by priority
        if(mode_freq > 1) ord_hand.assign(mode_freq, mode);
        ord_hand.insert(ord_hand.begin(), a.back());    //store hand ID with hand
        a.pop_back();
        switch(mode_freq) { //use frequency of mode to determine type of hand
            case 4: //four of a kind
                for(int i = 0; i < 13; ++i) {   //add fifth card to hand
                    if(rank_count[i] == 1) {
                        ord_hand.push_back(i);
                        break;
                    }
                }
                four_kind.push_back(ord_hand);
                break;
            case 3: {   //three of a kind, full house
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie full house)
                        ord_hand.insert(ord_hand.end(), 2, i);  //add cards to hand
                        full_house.push_back(ord_hand);
                        mode_freq = 0;  //skip counting as three of a kind
                        break;
                    }
                }
                if(mode_freq) { //if no second mode (ie three of a kind)
                    for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                        if(rank_count[i] && i != mode) ord_hand.push_back(i);
                    }
                    three_kind.push_back(ord_hand);
                }
                break;
            } case 2: { //two pair, one pair
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie two pair)
                        ord_hand.insert(ord_hand.end(), 2, i);  //add cards to hand
                        for(int j = 0; j < 13; ++j) {   //add last card to hand
                            if(rank_count[j] == 1) {
                                ord_hand.push_back(j);
                                break;
                            }
                        }
                        two_pair.push_back(ord_hand);
                        mode_freq = 0;  //skip counting as one pair
                        break;
                    }
                }
                if(mode_freq) { //if no second mode (ie one pair)
                    for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                        if(rank_count[i] && i != mode) ord_hand.push_back(i);
                    }
                    one_pair.push_back(ord_hand);
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
                    while(rank_count[finish]) ord_hand.push_back(finish--); //add cards in descending order
                    if(check_flush) straight_flush.push_back(ord_hand); //straight + flush --> straight flush
                    else straight.push_back(ord_hand);  //otherwise a straight
                    break;
                } else if(finish - start == 12) {   //special case straight starting with ace (A, 2, 3, 4, 5)
                    do {} while(!rank_count[--finish]);
                    if(finish - start == 3) {   //if rank range is 12, check if lowest four cards form a (sub)straight
                        ord_hand.insert(ord_hand.end(), {3, 2, 1, 0, 12});  //add cards (only possible case)
                        if(check_flush) straight_flush.push_back(ord_hand);
                        else straight.push_back(ord_hand);
                        break;
                    }
                }
                for(int i = 12; i >= 0; --i) {  //add cards in descending order
                    if(rank_count[i]) ord_hand.push_back(i);
                }
                if(check_flush) flush.push_back(ord_hand);  //flush if flush but not straight
                else high_card.push_back(ord_hand); //otherwise high card (no repeated cards, flush, or straight)
                break;
            }
        }
        hand.clear();
        rank_count.clear();
        ord_hand.clear();
    }

    //sort each type of hand by its corresponding accuracy
    bucket_sort(straight_flush, {1});
    bucket_sort(four_kind, {5, 1});
    bucket_sort(full_house, {4, 1});
    bucket_sort(flush, {5, 4, 3, 2, 1});
    bucket_sort(straight, {1});
    bucket_sort(three_kind, {5, 4, 1});
    bucket_sort(two_pair, {5, 3, 1});
    bucket_sort(one_pair, {5, 4, 3, 1});
    bucket_sort(high_card, {5, 4, 3, 2, 1});

    /*
    //combine buckets (hand types) in order
    for(vector<VI> hands : {high_card, one_pair, two_pair, three_kind, straight, flush, full_house, four_kind, straight_flush}) {
        for(int i = hands.size() - 1; i >= 0; --i) a.push_back(hands[i][0]);
        //cout << hands.size() << endl;   //print number of hands of each type
    }
    */

    //combine buckets (hand types) EXPLICITLY in order (I wish I could use a for loop)
    for(int i = high_card.size() - 1; i >= 0; --i) a.push_back(high_card[i][0]);
    for(int i = one_pair.size() - 1; i >= 0; --i) a.push_back(one_pair[i][0]);
    for(int i = two_pair.size() - 1; i >= 0; --i) a.push_back(two_pair[i][0]);
    for(int i = three_kind.size() - 1; i >= 0; --i) a.push_back(three_kind[i][0]);
    for(int i = straight.size() - 1; i >= 0; --i) a.push_back(straight[i][0]);
    for(int i = flush.size() - 1; i >= 0; --i) a.push_back(flush[i][0]);
    for(int i = full_house.size() - 1; i >= 0; --i) a.push_back(full_house[i][0]);
    for(int i = four_kind.size() - 1; i >= 0; --i) a.push_back(four_kind[i][0]);
    for(int i = straight_flush.size() - 1; i >= 0; --i) a.push_back(straight_flush[i][0]);

    /*
    for(vector<VI> hands: {straight_flush, four_kind, full_house, flush, straight, three_kind, two_pair, one_pair, high_card}) {
        for(vector<int> hand : hands) { //print each hand
            for(int i = 0; i < 5; ++i) cout << hand[i] << ", ";
            cout << hand[5] << endl;
        }
    }
    */

    //std::reverse(a.begin(), a.end());   //apparently sorting algorithms sort least to greatest
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
