#define SOL_ALL_SAFETIES_ON 1
#include <iostream>
#include "sol/sol.hpp"


void some_function() {
    std::cout<<"some cpp function"<<std::endl;
}
void other_function() {
    std::cout<<"other cpp function"<<std::endl;
}

class Coin {
public:
    Coin() {
        this->price = 1000;
        std::cout<<"Coin object constructor "<<this<<", price="<<this->price<<std::endl;
    }
    Coin(int price): price(price) {
        std::cout<<"Coin object constructor "<<this<<", price="<<this->price<<std::endl;
    }
    ~Coin() {
        std::cout<<"Coin object "<<this<<" is destroyed from memory"<<std::endl;
    }
    void show_price() {
        std::cout<<"showing Coin object info "<<this<<": price is "<<this->price<<std::endl;
    }
    void set_price(int price) {
        this->price = price;
    }
    int get_price() const {
        return this->price;
    }
private:
    int price;
};

int main() {
    std::cout<<"Hello Sol2 Library from C++"<<std::endl;

    // Sol2, Lua скрипт холбогч системийг ачааллах
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);

    // Lua скриптийг файлаас унших
    sol::load_result lua_file_script = lua.load_file("hello_sol.lua");
    // Уншсан скриптээ ажиллуулах
    lua_file_script();

    // Санах ой дээр Lua скрипт тохируулах
    const auto& lua_string = R"(
        print('executing Lua script defined in C++ memory')
        local a = 'test'
        for i=1,3,1 do print(i) end
        print('a variable : '..a)
    )";
    sol::load_result lua_memory_script = lua.load(lua_string);
    // Санах ой дээр хадгалсан скриптийг ажиллуулах
    lua_memory_script();


    // Sol оъбектээр Lua скриптээр дуудуулж болох C++ функц тодорхойлох
    lua["cppsum"] = [](int a, int b) {
        return a+b;
    };
    lua.script(R"(
        print('calling function defined on c++ from Lua script cppsum(4,5)='..cppsum(4,5))
    )");

    // Lua хэл дээр тодорхойлогдсон функцыш C++ ээс дуудах
    int result = lua["cppsum"](10, 20);
    std::cout<<"calling Lua function from C++, cppsum(10, 20)="<<result<<std::endl;

    // Lua хэл дээр функц доторхойлох
    lua.script(R"(
        function square(a)
            return a*a
        end
    )");
    result =lua["square"](3);
    std::cout<<"calling Lua function from C++, square(3)="<<result<<std::endl;

    // C++ функцийг Lua хэлрүү оруулах
    lua["some_f"] = some_function;
    lua.set_function("other_f", &other_function);
    lua.script(R"(
        print('calling C++ function from Lua')
        some_f()
        other_f()
    )");



    ///////////////////////////////////////////////////////////////////
    // C++ дээр тодорхойлсон классыг Lua хэлтэй холбох
    // https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
    //

    // Coin_type metatable тодорхойлох
    sol::usertype<Coin> Coin_type = lua.new_usertype<Coin>("Coin", 
        sol::constructors<Coin(), Coin(int)>());
    Coin_type["show_price"] = &Coin::show_price;
    Coin_type["price"     ] = sol::property(&Coin::get_price, &Coin::set_price);
    /*
    Coin_type.set_function("another_show", [](Coin& self) { 
        std::cout<<"showing price using new define function, price="<<self.get_price()<<std::endl;
        });
    */
    lua.script(R"(
        c1 = Coin.new()
        c2 = Coin.new(1500)
        c1:show_price()
        c2:show_price()
        c1.price = 2500
        c1:show_price()
    )");

    // Lua дээр үүссэн объектийн хаягаар нь C++ класс инстанс байдлаар авах
    Coin& coin = lua["c1"];
    std::cout<<"accessing Lua object as C++ class instance"<<std::endl;
    coin.show_price();
    
    return 0;
}