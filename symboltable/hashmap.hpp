/*
MIT License

Copyright (c) 2023 Max Goren - root@uniqueptr.com, www.maxgcoding.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef iterable_hashtable_hpp
#define iterable_hashtable_hpp
#include <iostream>
#include "hashfn.hpp"

template <typename K, typename V, class hasher = hashfn<K>>
class hashmap {
    private:
        constexpr static double MAX_LOADFACTOR = 0.5;
        constexpr static double MAX_SHRINKFACTOR = 0.78;
        struct MapEntry {
            K key;
            V value;
            bool empty;
            bool tombstone;
            MapEntry(K _key, V _value) {
                key = _key; value = _value;
                empty = false;
                tombstone = false;
            }
            MapEntry() { empty = true; tombstone = false; }
            MapEntry(const MapEntry& t) {
                key = t.key;
                value = t.value;
                empty = t.empty;
                tombstone = t.tombstone;
            }
            bool isEmpty() const { return empty; }
            bool tombstoned() const { return tombstone; }
        };
        MapEntry *table;
        int maxn;
        int n;
        int tombstones;
        void growAndRehash() {
            int oldmax = maxn;
            maxn = 2*oldmax;
            MapEntry* temp = table;
            table = new MapEntry[maxn];
            n = 0;
            for (int i = 0; i < oldmax; i++) {
                if (!temp[i].isEmpty() && !temp[i].tombstoned())
                    put(temp[i].key, temp[i].value);
            }
            delete [] temp;
        }
        void shrinkAndRehash() {
            int tmxn = maxn/1.21;
            int oldmax = maxn;
            do { tmxn+=5; } while (((double)n/tmxn) > MAX_LOADFACTOR);
            maxn = tmxn;
            MapEntry* temp = table;
            table = new MapEntry[maxn];
            n = 0;
            for (int i = 0; i < oldmax; i++) {
                if (!temp[i].isEmpty() && !temp[i].tombstoned())
                    put(temp[i].key, temp[i].value);
            }
            delete [] temp;
        }
        double loadfactor() {
            return (double)n/maxn;
        }
    public:
        class Iterator {
            private:
                MapEntry* __MapEntryPtr;
            public:
                Iterator(MapEntry* c) : __MapEntryPtr(c) {
                    
                }
                Iterator(const Iterator& o) {
                    __MapEntryPtr = o.__MapEntryPtr;
                }
                Iterator() : __MapEntryPtr(nullptr) { }
                pair<K,V> operator*() {
                    if (__MapEntryPtr->isEmpty() || __MapEntryPtr->tombstoned()) {
                        while (__MapEntryPtr->isEmpty() || __MapEntryPtr->tombstoned()) {
                            __MapEntryPtr++;
                        }
                    }
                    return make_pair(__MapEntryPtr->key, __MapEntryPtr->value);
                }
                Iterator operator++() {
                    do {
                            __MapEntryPtr++;
                    } while (__MapEntryPtr->isEmpty() || __MapEntryPtr->tombstoned());
                    return *this;
                }
                Iterator operator++(int) {
                    Iterator tmp = *this;
                    ++*this;
                    return tmp;
                }
                bool operator==(const Iterator& o) const {
                    return __MapEntryPtr == o.__MapEntryPtr;
                }
                bool operator!=(const Iterator& o) const {
                    return !(*this==o);
                }
                Iterator operator=(const Iterator& o) {
                    __MapEntryPtr = o.__MapEntryPtr;
                    return *this;
                }
        };
        Iterator _begin;
        Iterator _end;
        Iterator _found;
    public:
        hashmap(int max = 1013) {
            maxn = max;
            n = 0;
            table = new MapEntry[maxn];
            tombstones = 0;
        }
        hashmap(const hashmap& o) {
            maxn = o.maxn;
            n = 0;
            table = new MapEntry[maxn];
            tombstones = 0;
            if (!o.empty()) {
                for (auto it = o.begin(); it != o.end(); it++) {
                    put((*it).first, (*it).second);
                }
            }
        }
        ~hashmap() {
            delete [] table;
        }
        void put(K key, V value) {
            if (loadfactor() > MAX_LOADFACTOR) {
                growAndRehash();
            }
            int idx = hasher()(key) % maxn;
            int m = 1;
            while (!table[idx].isEmpty()) {
                if (key == table[idx].key && !table[idx].tombstoned()) {
                    table[idx].value = value;
                    return;
                }
                idx = (idx + (m*m)) % maxn;
                m++;
            }
            table[idx].key = key;
            table[idx].value = value;
            table[idx].empty = false;
            n++;
        }
        bool empty() const {
            return n == 0;
        }
        int size() const {
            return n;
        }
        int maxsize() const {
            return maxn;
        }
        Iterator& find(K key) {
            int idx = hashfn<K>()(key) % maxn;
            int m = 1;
            while (!table[idx].isEmpty()) {
                if (key == table[idx].key) {
                    _found = table[idx].tombstoned() ? end():Iterator(table+idx);
                    return _found;
                }
                idx = (idx + (m*m)) % maxn;
                m++;
            }
            return end();
        }
        void erase(K key) {
            int idx = hashfn<K>()(key) % maxn;
            int m = 1;
            bool found = false;
            while (!table[idx].isEmpty()) {
                if (key == table[idx].key && !table[idx].tombstoned()) {
                    found = true;
                    break;
                }
                idx = (idx + (m*m)) % maxn;
                m++;
            }
            if (found) {
                table[idx].tombstone = true;
                n--;
                tombstones++;
                if (((double)(n+tombstones)/maxn) > MAX_SHRINKFACTOR) {
                    shrinkAndRehash();
                }
            }
        }
        Iterator begin() {
            if (empty()) return end();
            _begin = Iterator(table);
            return _begin;
        }
        Iterator& end() {
            _end = Iterator(table+maxn);
            return _end;
        }
        const Iterator begin() const {
            if (empty()) return end();
            return Iterator(table);
        }
        const Iterator end() const {
            return Iterator(table+maxn);
        }
        V& operator[](K key) {
            if (find(key) == end()) {
                put(key, V());
            }
            int idx = hasher()(key) % maxn;
            int m = 1;
            while (!table[idx].isEmpty()) {
                if (key == table[idx].key)
                    break;
                idx = (idx + (m*m)) % maxn;
                m++;
            }
            return table[idx].value;
        }
        void clear() {
            for (int i = 0; i < maxn; i++) {
                table[i].empty = true;
                table[i].tombstone = false;
            }
        }
        hashmap operator=(const hashmap& o) {
            maxn = o.maxn;
            n = 0;
            table = new MapEntry[maxn];
            tombstones = 0;
            if (!o.empty()) {
                for (auto it : o) {
                    put(it.first, it.second);
                }
            }
            return *this;
        }
};

#endif