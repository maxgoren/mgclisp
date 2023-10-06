#ifndef iterable_map_interface_hpp
#define iterable_map_interface_hpp

class Iter {
    public:
        ~Iter() { }
};

template <class K, class V>
class IterableMap {
    public:
        virtual bool empty() const = 0;
        virtual int  size() const = 0;
        virtual void put(K,V) = 0;
        virtual void erase(K) = 0;
        virtual Iter& find(K) = 0;
        virtual const Iter& begin() const = 0;
        virtual const Iter& end() const = 0;
};

#endif