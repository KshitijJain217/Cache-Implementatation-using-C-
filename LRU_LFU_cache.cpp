
#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;


class LRUCache {
private:
    int capacity;

    list<pair<int,int>> lruList;


    unordered_map<int, list<pair<int,int>>::iterator> cacheMap;

public:
    LRUCache(int cap) : capacity(cap) {}

    int get(int key) {
        if (cacheMap.find(key) == cacheMap.end())
            return -1;


        lruList.splice(lruList.begin(), lruList, cacheMap[key]);

        return cacheMap[key]->second;
    }


    void put(int key, int value) {
        if (cacheMap.find(key) != cacheMap.end()) {
          
            cacheMap[key]->second = value;
            lruList.splice(lruList.begin(), lruList, cacheMap[key]);
            return;
        }

        if ((int)lruList.size() == capacity) {
            int lruKey = lruList.back().first; 
            lruList.pop_back();                
            cacheMap.erase(lruKey);         
        }

  
        lruList.push_front({key, value});
        cacheMap[key] = lruList.begin();
    }

    void display() {
        cout << "LRU Cache [front->back]: ";
        for (auto& [k, v] : lruList)
            cout << "[" << k << ":" << v << "] ";
        cout << "\n";
    }
};


class LFUCache {
private:
    int capacity;
    int minFreq; 

    unordered_map<int, pair<int,int>> keyMap;

    unordered_map<int, list<int>::iterator> iterMap;

    unordered_map<int, list<int>> freqMap;

    void incrementFreq(int key) {
        int freq = keyMap[key].second;
        int val  = keyMap[key].first;

        freqMap[freq].erase(iterMap[key]);

        if (freqMap[freq].empty()) {
            freqMap.erase(freq);
            if (minFreq == freq)
                minFreq++;
        }

        int newFreq = freq + 1;
        freqMap[newFreq].push_front(key);

        keyMap[key]  = {val, newFreq};
        iterMap[key] = freqMap[newFreq].begin();
    }

public:
    LFUCache(int cap) : capacity(cap), minFreq(0) {}

    int get(int key) {
        if (keyMap.find(key) == keyMap.end())
            return -1;

        incrementFreq(key);
        return keyMap[key].first;
    }

    void put(int key, int value) {
        if (capacity <= 0) return;

        if (keyMap.find(key) != keyMap.end()) {
            keyMap[key].first = value;
            incrementFreq(key);
            return;
        }

        if ((int)keyMap.size() == capacity) {
            int evictKey = freqMap[minFreq].back();
            freqMap[minFreq].pop_back();
            if (freqMap[minFreq].empty())
                freqMap.erase(minFreq);

            keyMap.erase(evictKey);
            iterMap.erase(evictKey);
        }

        minFreq = 1;
        freqMap[1].push_front(key);

        keyMap[key]  = {value, 1};
        iterMap[key] = freqMap[1].begin();
    }

    void display() {
        cout << "LFU Cache [freq -> keys]: ";
        for (auto& [freq, keys] : freqMap) {
            cout << "f" << freq << ":[";
            for (int k : keys) cout << k << "(" << keyMap[k].first << ") ";
            cout << "] ";
        }
        cout << "(minFreq=" << minFreq << ")\n";
    }
};


int main() {
    cout << "          LRU CACHE DEMO (cap=3)        \n";


    LRUCache lru(3);

    lru.put(1, 10); lru.display();
    lru.put(2, 20); lru.display();
    lru.put(3, 30); lru.display();

    cout << "get(1) = " << lru.get(1) << "\n";
    lru.display();

    lru.put(4, 40);
    lru.display();

    cout << "get(2) = " << lru.get(2) << "\n";
    cout << "get(3) = " << lru.get(3) << "\n";

    lru.put(1, 99);
    lru.display();

    cout << "          LFU CACHE DEMO (cap=3)        \n";


    LFUCache lfu(3);

    lfu.put(1, 10); lfu.display();
    lfu.put(2, 20); lfu.display();
    lfu.put(3, 30); lfu.display();

    cout << "get(1) = " << lfu.get(1) << "\n";
    cout << "get(1) = " << lfu.get(1) << "\n";
    cout << "get(2) = " << lfu.get(2) << "\n";
    lfu.display();

    lfu.put(4, 40);
    lfu.display();

    cout << "get(3) = " << lfu.get(3) << "\n";
    cout << "get(4) = " << lfu.get(4) << "\n";

    lfu.put(2, 99);
    lfu.display();

    return 0;
}


