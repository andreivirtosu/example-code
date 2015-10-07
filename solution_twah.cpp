#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

struct Record
{
    int timestamp;
    int id;
    double price;

    bool operator<(const Record& r)
    {
        return price < r.price; // ignored precision
    }
};

class Orderbook
{
public:
    Orderbook():
        twah(0.0),
        qty(0)
    {}

    void insert(Record);
    void erase(int timestamp, int id);
    double getHighestPrice();

    double getTWAH(){ return twah; }

private:
    void updateTWAH(int timestamp, double price);

    double twah; // Time Weighted Average Highest Price
    int qty; // sum of all time periods for highest prices

    int lastReceivedTimestamp;
    vector<Record> records;
};

// Highest price is the first element of MaxHeap. Complexity: O(1)
double Orderbook::getHighestPrice()
{
    return records.empty() ? NAN : records.front().price;
}

// Adds new element, keeping the MaxHeap structure. Complexity: O(logn)
void Orderbook::insert(Record r)
{
    if (!records.empty()) {
        updateTWAH(r.timestamp, records.front().price);
    }

    records.push_back(r);
    push_heap(records.begin(), records.end());
    lastReceivedTimestamp = r.timestamp;
}

// Erase element, keeping the MaxHeap structure. Complexity: O(n)
void Orderbook::erase(int timestamp, int id)
{
    updateTWAH(timestamp, records.front().price);
    lastReceivedTimestamp = timestamp;

    auto it = find_if(records.begin(), records.end(),
            [id](const Record& r)->bool{ return r.id==id; });

    if (it != records.end()) {
      iter_swap(it, --records.end());
      records.pop_back();
      make_heap(records.begin(), records.end());
    }
}

void Orderbook::updateTWAH(int timestamp, double topPrice)
{
    int newQty = timestamp - lastReceivedTimestamp;
    twah = ( newQty*topPrice + twah*qty) / (qty+newQty);
    qty += newQty;
}


int main(int argc, char *argv[])
{
    if (argc<2){
        cout <<"no input file!";
        return 1;
    }

    ifstream in(argv[1]);

    string line;
    Orderbook ob;
    Record r;
    char ch;
    while (getline(in, line)) {
       stringstream ss(line);
       ss >> r.timestamp; 
       ss >> ch;
       ss >> r.id;
       if (ch == 'I') {
           ss >> r.price;
           ob.insert(r);
       } else {
           ob.erase(r.timestamp, r.id);
       }
    }
    
    cout << ob.getTWAH();

    return 0;
}
