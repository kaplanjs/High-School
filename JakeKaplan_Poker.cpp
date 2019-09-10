#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <random>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::sort;
using std::string;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

///Strategy:
/**
high card: drop 3 lowest
    except: straight or flush off by one
pair: drop 3 solos
    except: high card is Q+ or flush off by one
two pair: drop solo
three of a kind: drop two solos
    except: flush off by one
straight: drop none
flush: drop none
full house: drop none
four kind: drop solo (doesn't really matter)
    except: solo is 9+
straight flush: drop none
**/

random_device seed;
mt19937 gen(seed());
//mt19937 gen(17);

int n;
bool output = 0;

vector<double> weight(9);   //probability that given hand will beat another hand (for brute force expVal calculation)
vector<string> print_vec = {"high_card", "one_pair", "two_pair", "three_kind", "straight", "flush", "full_house", "four_kind", "straight_flush"};

vector<double> poker_prob(const vector<int>& a, bool print = 0) {
    vector<double> prob(9, 0);
    for(int i = 0; i < a.size(); ++i) {   //identify type of each hand
        div_t temp = div(a[i], 52);
        vector<int> hand{temp.rem};
        for(int j = 1; j < 5; ++j) {    //determine each card in hand
            temp = div(temp.quot, 52);
            hand.push_back(temp.rem);
        }
        vector<int> rank_count(13, 0);
        for(int j = 0; j < 5; ++j) {    //create frequency vector for each possible card rank
            ++rank_count[hand[j]/4];
        }
        int mode_freq = 1;
        int mode = 0;
        for(int j = 0; j < 13; ++j) {
            if(rank_count[j] >= mode_freq) {    //determine mode of hand and number of occurrences of mode
                mode_freq = rank_count[j];
                mode = j;
            }
        }
        switch(mode_freq) { //use frequency of mode to determine type of hand
            case 4: //four of a kind
                ++prob[7];
                break;
            case 3: {   //three of a kind, full house
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie full house)
                        ++prob[6];
                        mode_freq = 0;  //skip counting as three of a kind
                        break;
                    }
                }
                if(mode_freq) ++prob[3];    //if no second mode (ie three of a kind)
                break;
            } case 2: { //two pair, one pair
                mode_freq = 1;
                for(int i = 0; i < 13; ++i) {
                    if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie two pair)
                        ++prob[2];
                        mode_freq = 0;  //skip counting as one pair
                        break;
                    }
                }
                if(mode_freq) ++prob[1];    //if no second mode (ie one pair)
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
                    if(check_flush) ++prob[8];  //straight + flush --> straight flush
                    else ++prob[4]; //otherwise a straight
                    break;
                } else if(finish - start == 12) {   //special case straight starting with ace (A, 2, 3, 4, 5)
                    do {} while(!rank_count[--finish]);
                    if(finish - start == 3) {   //if rank range is 12, check if lowest four cards form a (sub)straight
                        if(check_flush) ++prob[8];
                        else ++prob[4];
                        break;
                    }
                }
                if(check_flush) ++prob[5];  //flush if flush but not straight
                else ++prob[0]; //otherwise high card (no repeated cards, flush, or straight)
                break;
            }
        }
        hand.clear();   //clear vectors after use to reserve memory
        rank_count.clear();
    }
    for(int i = 0; i < 9; ++i) {    //experimental probability of getting each kind of hand
        prob[i] /= a.size();
        if(print) cout << 100*prob[i] << "% for " << print_vec[i] << endl;
    }
    return prob;
}

bool compare(int i, int j) { return (i > j); }  //for sorting in descending order

void drop(vector<int>& hand, const vector<int>& drop_cards) {
    if(output) cout << drop_cards.size() << " ";
    vector<int> sort_hand = hand;
    sort(sort_hand.begin(), sort_hand.end(), compare);  //order hand in descending order (prevents d.erase() error)
    vector<int> d(52);
    for(int i = 0; i < 52; ++i) d[i] = i;
    for(int card : sort_hand) d.erase(d.begin() + card);    //don't reselect cards already picked
    for(int card : drop_cards) {    //replace each card in drop_cards vector (at random)
        auto u = uniform_int_distribution<int>(0, d.size() - 1);
        int idx = u(gen);
        if(output) cout << hand[card] << " ";
        hand[card] = d[idx];
        d.erase(d.begin() + idx);
    }
    if(output) cout << endl;
    return;
}

double checkDrop(const vector<int>& hand, const vector<int>& drop_cards) {  //brute force check expected value after dropping
    vector<int> a(1000), temp;  //sample 1000 hands
    for(int i = 0; i < a.size(); ++i) { //drop and reselect cards randomly for each hand
        temp = hand;
        drop(temp, drop_cards);
        a[i] = temp[0];
        for(int j = 1; j < 5; ++j) {
            a[i] *= 52;
            a[i] += temp[j];
        }
    }
    vector<double> prob = poker_prob(a);    //check statistics on sample
    double expVal = 0;
    for(int i = 0; i < 9; ++i) expVal += prob[i]*weight[i]; //where weight is probability that hand wins (percentile)
    return expVal;
}

void order(vector<int>& hand, const vector<int>& ord_hand) {    //order hand in order of ord_hand
    vector<int> temp(5), new_hand(5, -1);
    for(int i = 0; i < 5; ++i) temp[i] = hand[i]/4;
    for(int i = 0; i < 5; ++i) {
        for(int j = 0; j < 5; ++j) {
            if(new_hand[j] < 0 && temp[i] == ord_hand[j]) {
                new_hand[j] = hand[i];
                break;
            }
        }
    }
    hand = new_hand;
}

void straightFlush(vector<int>& hand, bool bf) {
    return;
}

void fourKind(vector<int>& hand, bool bf) {
    if(hand[4] < 28) drop(hand, {4});   //drop solo card if 7 or less
    return;
}

void fullHouse(vector<int>& hand, bool bf) {
    return;
}

void flush(vector<int>& hand, bool bf) {
    return;
}

void straight(vector<int>& hand, bool bf) {
    return;
}

void threeKind(vector<int>& hand, bool bf) {
    if(!bf) {
        int suit = hand[0]%4;
        for(int i = 1; i < 3; ++i) {
            if(hand[i]%4 != suit) {
                drop(hand, {3, 4}); //ignore flush if doesn't contain three of a kind
                return;
            }
        }
        if(hand[3]%4 == suit) drop(hand, {4});  //if one off from flush, drop odd card out
        else if(hand[4]%4 == suit) drop(hand, {3});
        else drop(hand, {3, 4});    //otherwise drop both solo cards
    } else{ //brute force all combinations of dropping two or less solo cards
        int bestCase = 0;
        double best = weight[3];
        if(checkDrop(hand, {3}) > best) {
            best = checkDrop(hand, {3});
            bestCase = 1;
        }
        if(checkDrop(hand, {4}) > best) {
            best = checkDrop(hand, {4});
            bestCase = 2;
        }
        if(checkDrop(hand, {3, 4}) > best) drop(hand, {3, 4});
        else if(bestCase == 2) drop(hand, {4});
        else if(bestCase == 1) drop(hand, {3});
    }
    return;
}

void twoPair(vector<int>& hand, bool bf) {
    if(!bf) {
        vector<int> suit(4);
        for(int i = 0; i < 5; ++i) ++suit[hand[i]%4];
        for(int i = 0; i < 4; ++i) {
            if(suit[i] == 4) {
                for(int j = 0; j < 5; ++j) {
                    if(hand[j]%4 != i) {
                        drop(hand, {j});    //if one off from flush, drop odd card out
                        return;
                    }
                }
            }
        }
        drop(hand, {4});    //otherwise drop solo card
    } else {    //brute force every possible option for dropping cards
        bool do_drop = 0;
        double temp;
        vector<vector<int>> bestCase(4);
        vector<double> best(4, 0);
        best[0] = weight[2];    //expVal of dropping no cards
        for(int i = 0; i < 5; ++i) {    //all options for dropping a single card
            temp = checkDrop(hand, {i});
            if(temp > best[1]) {
                best[1] = temp;
                bestCase[1] = {i};
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping two cards
            for(int j = 0; j < i; ++j) {
                temp = checkDrop(hand, {i, j});
                if(temp > best[2]) {
                    best[2] = temp;
                    bestCase[2] = {i, j};
                }
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping three cards
            for(int j = 0; j < i; ++j) {
                for(int k = 0; k < j; ++k) {
                    temp = checkDrop(hand, {i, j, k});
                    if(temp > best[3]) {
                        best[3] = temp;
                        bestCase[3] = {i, j, k};
                    }
                }
            }
        }
        for(int i = 1; i < 4; ++i) {    //find greatest expVal
            if(best[i] > best[0]) {
                best[0] = best[i];
                bestCase[0] = bestCase[i];
                do_drop = 1;
            }
        }
        if(do_drop) drop(hand, bestCase[0]);
    }
    return;
}

void onePair(vector<int>& hand, bool bf) {
    if(!bf) {
        vector<int> suit(4);
        for(int i = 0; i < 5; ++i) ++suit[hand[i]%4];
        for(int i = 0; i < 4; ++i) {
            if(suit[i] == 4) {
                for(int j = 0; j < 5; ++j) {
                    if(hand[j]%4 != i) {
                        drop(hand, {j});    //if one off from flush, drop odd card out
                        return;
                    }
                }
            }
        }
        if(hand[2] - hand[4] < 5) { //if one off from straight, drop odd card out
            if(hand[0] < hand[2] && hand[0] > hand[4]) {
                drop(hand, {0});
                return;
            } else if(hand[2] - hand[4] < 4 && (hand[0] == hand[2] + 1 || hand[0] == hand[4] - 1)) {
                drop(hand, {0});
                return;
            }
        }
        if(hand[2] > 9) drop(hand, {3, 4}); //if high card is Q or better, only drop other two solo cards
        else drop(hand, {2, 3, 4}); //otherwise drop all solo cards
    } else {    //brute force every possible option for dropping cards
        bool do_drop = 0;
        double temp;
        vector<vector<int>> bestCase(4);
        vector<double> best(4, 0);
        best[0] = weight[1];    //expVal of dropping no cards
        for(int i = 0; i < 5; ++i) {    //all options for dropping a single card
            temp = checkDrop(hand, {i});
            if(temp > best[1]) {
                best[1] = temp;
                bestCase[1] = {i};
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping two cards
            for(int j = 0; j < i; ++j) {
                temp = checkDrop(hand, {i, j});
                if(temp > best[2]) {
                    best[2] = temp;
                    bestCase[2] = {i, j};
                }
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping three cards
            for(int j = 0; j < i; ++j) {
                for(int k = 0; k < j; ++k) {
                    temp = checkDrop(hand, {i, j, k});
                    if(temp > best[3]) {
                        best[3] = temp;
                        bestCase[3] = {i, j, k};
                    }
                }
            }
        }
        for(int i = 1; i < 4; ++i) {    //find greatest expVal
            if(best[i] > best[0]) {
                best[0] = best[i];
                bestCase[0] = bestCase[i];
                do_drop = 1;
            }
        }
        if(do_drop) drop(hand, bestCase[0]);
    }
    return;
}

void highCard(vector<int>& hand, bool bf) {
    if(!bf) {
        vector<int> suit(4);
        for(int i = 0; i < 5; ++i) ++suit[hand[i]%4];
        for(int i = 0; i < 4; ++i) {
            if(suit[i] == 4) {
                for(int j = 0; j < 5; ++j) {
                    if(hand[j]%4 != i) {
                        drop(hand, {j});    //if one off from flush, drop odd card out
                        return;
                    }
                }
            }
        }
        if(hand[0] - hand[4] == 5) drop(hand, {4}); //if one off from straight, drop odd card out
        else drop(hand, {2, 3, 4}); //otherwise drop three lowest cards
    } else {    //brute force every possible option for dropping cards (does not weight by high card!)
        bool do_drop = 0;
        double temp;
        vector<vector<int>> bestCase(4);
        vector<double> best(4, 0);
        best[0] = weight[0];    //expVal of dropping no cards
        for(int i = 0; i < 5; ++i) {    //all options for dropping a single card
            temp = checkDrop(hand, {i});
            if(temp > best[1]) {
                best[1] = temp;
                bestCase[1] = {i};
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping two cards
            for(int j = 0; j < i; ++j) {
                temp = checkDrop(hand, {i, j});
                if(temp > best[2]) {
                    best[2] = temp;
                    bestCase[2] = {i, j};
                }
            }
        }
        for(int i = 0; i < 5; ++i) {    //all options for dropping three cards
            for(int j = 0; j < i; ++j) {
                for(int k = 0; k < j; ++k) {
                    temp = checkDrop(hand, {i, j, k});
                    if(temp > best[3]) {
                        best[3] = temp;
                        bestCase[3] = {i, j, k};
                    }
                }
            }
        }
        for(int i = 1; i < 4; ++i) {    //find greatest expVal
            if(best[i] > best[0]) {
                best[0] = best[i];
                bestCase[0] = bestCase[i];
                do_drop = 1;
            }
        }
        if(do_drop) drop(hand, bestCase[0]);
    }
    return;
}

void poker(vector<int>& a, const bool bf = 0) {
    vector<int> hand;
    if(a.size() == 5) hand = a;
    else {
        div_t temp = div(a[0], 52);
        hand.push_back(temp.rem);
        for(int i = 1; i < 5; ++i) {    //determine each card in hand
            temp = div(temp.quot, 52);
            hand.push_back(temp.rem);
        }
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
    switch(mode_freq) { //use frequency of mode to determine type of hand
        case 4: //four of a kind
            for(int i = 0; i < 13; ++i) {   //add fifth card to hand
                if(rank_count[i] == 1) {
                    ord_hand.push_back(i);
                    break;
                }
            }
            order(hand, ord_hand);  //reorder hand
            fourKind(hand, bf);
            break;
        case 3: {   //three of a kind, full house
            mode_freq = 1;
            for(int i = 0; i < 13; ++i) {
                if(rank_count[i] > mode_freq && i != mode) {    //check for second mode (ie full house)
                    ord_hand.insert(ord_hand.end(), 2, i);  //add cards to hand
                    order(hand, ord_hand);  //reorder hand
                    fullHouse(hand, bf);
                    mode_freq = 0;  //skip counting as three of a kind
                    break;
                }
            }
            if(mode_freq) { //if no second mode (ie three of a kind)
                for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                    if(rank_count[i] && i != mode) ord_hand.push_back(i);
                }
                order(hand, ord_hand);  //reorder hand
                threeKind(hand, bf);
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
                    order(hand, ord_hand);  //reorder hand
                    twoPair(hand, bf);
                    mode_freq = 0;  //skip counting as one pair
                    break;
                }
            }
            if(mode_freq) { //if no second mode (ie one pair)
                for(int i = 12; i >= 0; --i) {  //add remaining cards in descending order
                    if(rank_count[i] && i != mode) ord_hand.push_back(i);
                }
                order(hand, ord_hand);  //reorder hand
                onePair(hand, bf);
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
                for(int i = finish; i >= start; --i) ord_hand.push_back(i); //add cards in descending order
                order(hand, ord_hand);  //reorder hand
                if(check_flush) straightFlush(hand, bf); //straight + flush --> straight flush
                else straight(hand, bf);  //otherwise a straight
                break;
            } else if(finish - start == 12) {   //special case straight starting with ace (A, 2, 3, 4, 5)
                do {} while(!rank_count[--finish]);
                if(finish - start == 3) {   //if rank range is 12, check if lowest four cards form a (sub)straight
                    ord_hand = {3, 2, 1, 0, 12};  //add cards (only possible case)
                    order(hand, ord_hand);  //reorder hand
                    if(check_flush) straightFlush(hand, bf);
                    else straight(hand, bf);
                    break;
                }
            }
            for(int i = 12; i >= 0; --i) {  //add cards in descending order
                if(rank_count[i]) ord_hand.push_back(i);
            }
            order(hand, ord_hand);  //reorder hand
            if(check_flush) flush(hand, bf);  //flush if flush but not straight
            else highCard(hand, bf); //otherwise high card (no repeated cards, flush, or straight)
            break;
        }
    }
    if(a.size() == 1) {
        a[0] = hand[0]; //convert hand back to integer ID
        for(int i = 1; i < 5; ++i) {
            a[0] *= 52;
            a[0] += hand[i];
        }
    }
    return;
}


int main() {    //read input, print cards to drop
    output = 1;
    vector<int> hand(5);
    for(int i = 0; i < 5; ++i) cin >> hand[i];
    poker(hand);
}

/*
int main() {    //run statistical analysis
    srand(time(0));
    //srand(17);

    n = 1000000;    //number of hands to sample

    vector<int> d(52);
    for(int i = 0; i < 52; ++i) d[i] = i;

    vector<int> a(n);
    for(int i = 0; i < n; ++i) {    //generate n random hands
        vector<int> h(5);
        int hand = 0;
        for(int j = 0; j < 5; ++j) {
            int idx = rand()%d.size();
            h[j] = d[idx];
            d.erase(d.begin() + idx);   //select without replace
            hand *= 52;
            hand += h[j];
        }
        a[i] = hand;
        for(int j = 0; j < 5; ++j) d.push_back(h[j]);   //replace each card before selecting next hand
    }
    vector<int> temp(1);
    //vector<int> b(a); //generate copy of original (random) sample

    clock_t start = std::clock();

    cout << "Original Statistics:" << endl;
    poker_prob(a, 1);   //run probability on original (random) hands
    for(int i = 0; i < n; ++i) {
        temp[0] = a[i];
        poker(temp);    //perform strategy on each hand
        a[i] = temp[0];
    }
    cout << endl << "Adjusted Statistics:" << endl;
    poker_prob(a, 1);   //run probability on adjusted hands

    //run brute force code
    *//*
    weight = poker_prob(a);
    cout << endl;
    for(int i = 0; i < 9; ++i) {
        if(i) weight[i] += weight[i-1]; //calculate weighting (percent of sample hands less than or equal to given hand)
    }
    for(int i = 0; i < 10; ++i) {
        for(int j = 0; j < n/10; ++j) {
            temp[0] = b[i*n/10 + j];
            poker(temp, 1); //run brute force on each hand (from original sample)
            b[i*n/10 + j] = temp[0]
        }
        cout << " . (" << i+1 << "0%) .";   //print progress markers (every 10%)
    }
    cout << endl << endl << "Brute Force Statistics:" << endl;
    poker_prob(b, 1);   //run probability on brute forced hands
    *//*

    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

    cout << duration << "seconds\n";
}
*/
