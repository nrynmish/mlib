#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <functional>
#include <unordered_set>

class Value;

using namespace std;

using Valueptr = shared_ptr<Value>;

struct Hash {
    size_t operator()(const Valueptr value) const;
};

class Value : public enable_shared_from_this<Value> 
{
    private:
        inline static size_t currentID = 0;
        float data;
        float gradient;
        string operation;
        size_t id;
        vector<Valueptr> prev;
        function<void()> backward; 
        friend struct Hash;  
    
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

        static Valueptr add(const Valueptr& lhs, const Valueptr& rhs)
        {
            auto result = Value::create(lhs->data + rhs->data, "+");
            result->prev = {lhs, rhs};
            result->backward =[lhs_weak = weak_ptr<Value>(lhs), rhs_weak = weak_ptr<Value>(rhs), result_weak = weak_ptr<Value>(result)]()
            {
                lhs_weak.lock()->gradient += result_weak.lock()->gradient;
                rhs_weak.lock()->gradient += result_weak.lock()->gradient;
            };
            return result;
        }

        static Valueptr mul(const Valueptr& lhs, const Valueptr& rhs)
        {
            auto result = Value::create(lhs->data * rhs->data, "*");
            result->prev = {lhs, rhs};
            result->backward = [lhs_weak = weak_ptr<Value>(lhs), rhs_weak = weak_ptr<Value>(rhs), result_weak = weak_ptr<Value>(result)]()
            {
                lhs_weak.lock()->gradient += rhs_weak.lock()->data * result_weak.lock()->gradient;
                rhs_weak.lock()->gradient += lhs_weak.lock()->data * result_weak.lock()->gradient;
            };
            return result;
        }

        static Valueptr substract(const Valueptr& lhs, const Valueptr& rhs)
        {
            auto result = Value::create(lhs->data - rhs->data, "-");
            result->prev = {lhs, rhs};
            result->backward = [lhs_weak = weak_ptr<Value>(lhs), rhs_weak = weak_ptr<Value>(rhs), result_weak = weak_ptr<Value>(result)]()
            {
                lhs_weak.lock()->gradient += result_weak.lock()->gradient;
                rhs_weak.lock()->gradient -= result_weak.lock()->gradient;
            };
            return result;
        }

        void buildTopo(Valueptr v, unordered_set<Valueptr, Hash>& visited, vector<Valueptr>& topo)
        {
            if(visited.find(v) == visited.end())
            {
                visited.insert(v);
                for(const auto& child : v->prev){
                    buildTopo(child, visited, topo);
                }
                topo.push_back(v);
            }
        }

        void backProp()
        {
            vector<Valueptr> topo;
            unordered_set<Valueptr, Hash> visited;
            buildTopo(shared_from_this(),visited, topo);
            for(auto nodes : topo)
            {
                nodes->backward();
            }
        }

        void print() const
        {
            cout << "Value(data=" << data << ", grad=" << gradient << ", op=" << operation << ", id=" << id << ")" << endl;
        }


    };

size_t Hash::operator()(const Valueptr value) const
{
    return hash<string>()(value.get()->operation) ^ hash<float>()(value.get()->data);
}

int main(){
    
    auto a = Value::create(1.0, "+");
    auto b = Value::create(2.0, "+");

    auto c = Value::add(a, b);
    c->print();

    auto d = Value::mul(c, c);
    d->print();
    // auto a = shared_ptr<Value>();
}