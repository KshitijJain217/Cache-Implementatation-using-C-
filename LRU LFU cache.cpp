// ============================================================
// LRU Cache + LFU Cache — Interview-Ready Implementation
// C++17 | Clean, Commented, Beginner-Friendly
// ============================================================

#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

// ============================================================
// LRU CACHE
// Strategy: HashMap + Doubly Linked List
// List stores {key, value} pairs
// Map stores key → iterator into the list (for O(1) access)
// ============================================================

class LRUCache {
private:
    int capacity;

    // Doubly linked list: front = most recently used
    //                     back  = least recently used (evict this)
    list<pair<int,int>> lruList; // {key, value}

    // Maps key → iterator pointing to its node in lruList
    // This lets us find & move any node in O(1)
    unordered_map<int, list<pair<int,int>>::iterator> cacheMap;

public:
    LRUCache(int cap) : capacity(cap) {}

    // GET: Return value for key, or -1 if not found
    // Also marks this key as "most recently used"
    int get(int key) {
        // Key doesn't exist → cache miss
        if (cacheMap.find(key) == cacheMap.end())
            return -1;

        // Move this node to the FRONT of the list (most recently used)
        // splice() moves an element within a list without copying — O(1)
        lruList.splice(lruList.begin(), lruList, cacheMap[key]);

        return cacheMap[key]->second; // return the value
    }

    // PUT: Insert or update a key-value pair
    void put(int key, int value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Key already exists → update value and move to front
            cacheMap[key]->second = value;
            lruList.splice(lruList.begin(), lruList, cacheMap[key]);
            return;
        }

        // Cache is full → evict the Least Recently Used (back of list)
        if ((int)lruList.size() == capacity) {
            int lruKey = lruList.back().first; // key of the LRU node
            lruList.pop_back();                // remove from list
            cacheMap.erase(lruKey);            // remove from map
        }

        // Insert new key at the FRONT (most recently used position)
        lruList.push_front({key, value});
        cacheMap[key] = lruList.begin(); // store iterator
    }

    // Utility: print current cache state (front = most recent)
    void display() {
        cout << "LRU Cache [front->back]: ";
        for (auto& [k, v] : lruList)
            cout << "[" << k << ":" << v << "] ";
        cout << "\n";
    }
};


// ============================================================
// LFU CACHE
// Strategy: Three HashMaps + minFreq tracker
//
// keyMap:   key → {value, frequency}
// iterMap:  key → iterator in its frequency bucket list
// freqMap:  frequency → list of keys (front=most recent, back=LRU)
// minFreq:  the current minimum frequency (for O(1) eviction)
// ============================================================

class LFUCache {
private:
    int capacity;
    int minFreq; // track the minimum frequency at all times

    // key → {value, frequency}
    unordered_map<int, pair<int,int>> keyMap;

    // key → iterator in freqMap[freq] list
    unordered_map<int, list<int>::iterator> iterMap;

    // frequency → list of keys with that frequency
    // Front of list = most recently used (for LRU tie-breaking)
    // Back of list  = least recently used (evict this on tie)
    unordered_map<int, list<int>> freqMap;

    // Internal helper: increment frequency of an existing key
    // Called by both get() and put() on existing keys
    void incrementFreq(int key) {
        int freq = keyMap[key].second;       // current frequency
        int val  = keyMap[key].first;        // current value

        // Remove key from its current frequency bucket
        freqMap[freq].erase(iterMap[key]);

        // If this bucket is now empty AND it was the min-freq bucket,
        // increment minFreq (the new min must be freq+1)
        if (freqMap[freq].empty()) {
            freqMap.erase(freq);
            if (minFreq == freq)
                minFreq++;
        }

        // Insert key at FRONT of the (freq+1) bucket
        // Front = most recently used within this frequency
        int newFreq = freq + 1;
        freqMap[newFreq].push_front(key);

        // Update keyMap and iterMap
        keyMap[key]  = {val, newFreq};
        iterMap[key] = freqMap[newFreq].begin();
    }

public:
    LFUCache(int cap) : capacity(cap), minFreq(0) {}

    // GET: Return value or -1, and bump frequency
    int get(int key) {
        if (keyMap.find(key) == keyMap.end())
            return -1; // cache miss

        incrementFreq(key); // accessing = using once more
        return keyMap[key].first;
    }

    // PUT: Insert new key or update existing one
    void put(int key, int value) {
        if (capacity <= 0) return;

        if (keyMap.find(key) != keyMap.end()) {
            // Key exists → just update value and bump frequency
            keyMap[key].first = value;
            incrementFreq(key);
            return;
        }

        // Cache is full → evict Least Frequently Used
        // Tie-break: LRU within minFreq bucket = back of the list
        if ((int)keyMap.size() == capacity) {
            int evictKey = freqMap[minFreq].back(); // LFU + LRU key
            freqMap[minFreq].pop_back();
            if (freqMap[minFreq].empty())
                freqMap.erase(minFreq);

            keyMap.erase(evictKey);
            iterMap.erase(evictKey);
        }

        // Insert new key with frequency = 1
        minFreq = 1; // new key always starts at freq 1 → new minimum
        freqMap[1].push_front(key);

        keyMap[key]  = {value, 1};
        iterMap[key] = freqMap[1].begin();
    }

    // Utility: print cache contents grouped by frequency
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


// ============================================================
// MAIN — Demo with step-by-step output
// ============================================================

int main() {
    cout << "========================================\n";
    cout << "          LRU CACHE DEMO (cap=3)        \n";
    cout << "========================================\n";

    LRUCache lru(3);

    lru.put(1, 10); lru.display(); // [1:10]
    lru.put(2, 20); lru.display(); // [2:20, 1:10]
    lru.put(3, 30); lru.display(); // [3:30, 2:20, 1:10]

    cout << "get(1) = " << lru.get(1) << "\n"; // 10, moves 1 to front
    lru.display();                               // [1:10, 3:30, 2:20]

    lru.put(4, 40); // Cache full → evict key 2 (LRU = back)
    lru.display();  // [4:40, 1:10, 3:30]

    cout << "get(2) = " << lru.get(2) << "\n"; // -1 (evicted)
    cout << "get(3) = " << lru.get(3) << "\n"; // 30

    lru.put(1, 99); // Update existing key
    lru.display();  // [1:99, 3:30, 4:40]

    cout << "\n========================================\n";
    cout << "          LFU CACHE DEMO (cap=3)        \n";
    cout << "========================================\n";

    LFUCache lfu(3);

    lfu.put(1, 10); lfu.display(); // freq1: [1]
    lfu.put(2, 20); lfu.display(); // freq1: [2, 1]
    lfu.put(3, 30); lfu.display(); // freq1: [3, 2, 1]

    cout << "get(1) = " << lfu.get(1) << "\n"; // 10, freq 1→2
    cout << "get(1) = " << lfu.get(1) << "\n"; // 10, freq 2→3
    cout << "get(2) = " << lfu.get(2) << "\n"; // 20, freq 1→2
    lfu.display();
    // freq1:[3]  freq2:[2]  freq3:[1]

    lfu.put(4, 40); // full → evict key 3 (minFreq=1, LRU in f1)
    lfu.display();  // freq1:[4]  freq2:[2]  freq3:[1]

    cout << "get(3) = " << lfu.get(3) << "\n"; // -1 (evicted)
    cout << "get(4) = " << lfu.get(4) << "\n"; // 40

    lfu.put(2, 99); // Update key 2: value→99, freq bumped
    lfu.display();

    return 0;
}