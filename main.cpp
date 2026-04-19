#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <functional>

class Value;

using namespace std;

using Valueptr = shared_ptr<Value>;

class Value : public enable_shared_from_this<Value> 
{
    private:
        inline static size_t currentID = 0;
        float data;
        float gradient;
        string operation;
        size_t id;
        vector<Value> prev;
    
    private:
        Value(float data, const string& operation, size_t id) : data(data), operation(operation), id(id){};

    public:
        static Valueptr create(float data, const string& operation = "")
        {
            return shared_ptr<Value>(new Value(data, operation, currentID++));
        }

        ~Value()
        {
            --currentID;
        }

    };

int main(){
    
    auto a = Value::create(1.0, "+");
    // auto c = add(a + b);

    // auto a = shared_ptr<Value>();
}