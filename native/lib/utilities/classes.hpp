#ifndef CLASSES_HPP
#define CLASSES_HPP

/*
 * static liftime shared singleton pattern. This isn't inherintly thread safe
 *  so usage should be used in a thread safe manner. Either think hard 
 *  about usage or BYO saftey mechanism
 */

template <typename T>
class Singleton {
public:
    static T& instance() {
       static T _instance; 
       return _instance;
    }
protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton & s) = delete;
    Singleton& operator=(const Singleton & s) = delete;
};


#endif // CLASSES_HPP