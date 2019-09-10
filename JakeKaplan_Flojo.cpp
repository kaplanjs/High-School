#include <iostream>
#include <vector>
using std::vector;
using std::cout;

template<class T>   //requires type of List to be defined (Link<type>)
struct Link {
    explicit Link(const T& info, Link *next = 0) : info(info), next(next) { }
    //requires info to be defined when List is made

    // This avoids stack overflow
    ~Link() {
        Link *p = next;
        while (p) {
            Link *q = p->next;
            p->next = 0;
            delete p;
            p = q;
        }
    }

    T info;
    Link *next;
};

vector<int> josephus(int n, int k) {
    vector<int> output; //define output vector
    Link<int> *q = new Link<int>(1);    //create first link "q"
    Link<int> *head = new Link<int>(n, q);  //create end link "head"
    Link<int> *p = head;    //place pointer "p" at last (or 0th) link
    Link<int> *r;
    for(int i = 1; i < n-1; ++i) {  //create n-2 links before end link (n total links)
        r = new Link<int>(n-i, head);
        head = r;   //"head" becomes most recent link
    }
    q -> next = head;   //first link points to most recent link
    head = q;   //"head" becomes first link
    while(n){
        for(int i = 0; i < (k - 1)%n; i++){ //move p to (k-1)st link in loop
            p = q;
            q = p -> next;
        }
        p -> next = q -> next;  //(k-1)st link points to (k+1)st link (remove kth link from loop)
        output.push_back(q -> info);    //record link number of kth link
        q = p -> next;
        n--;    //subtract from link count
    }
    return output;
}

vector<int> loopTail(Link<int> *head) {
    vector<int> output; //define output vector
    if(head) {  //check if list is empty
        Link<int> *p = head -> next;
        Link<int> *q = head;
        bool i = 1;
        int j = 1;
        while(p != q && p) {    //check if p and q overlap
            p = p -> next;
            i = 1 - i;
            if(i){  //q increments every other time p increments
                q = q -> next;
            }
            j++;    //in case p and q don't overlap, count until p no longer exists (list ends)
        }
        if(p == q) {    //list contains a loop
            j = 0;
            do {
                p = p -> next;
                j++;    //count number of times p increments before returning to q (length of loop)
            } while(p != q);
            output.push_back(j);    //record length of loop
            p = head;   //return p and q to head of list
            q = head;
            for(int k = 0; k < j; k++) {    //move q to one loop length after p
                q = q -> next;
            }
            j = 0;
            while(p != q) {
                p = p -> next;
                q = q -> next;
                j++;    //count how many times q increments before entering loop (length of tail)
            }
            output.push_back(j);    //record length of tail
        } else {    //list doesn't contain a loop
            output.push_back(0);    //record length of loop as 0
            output.push_back(j);    //record length of tail as length of list
        }
    } else {    //list is empty
        output.assign(2, 0);    //record length of loop and tail as both 0
    }
    return output;
}

/*
int main() {

}
*/
