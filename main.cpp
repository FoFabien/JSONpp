#include <iostream>
#include "json.hpp"

int main()
{
    JSON json;
    if(json.parse("test.json"))
    {
        std::cout << json << std::endl;
        JSON_obj& ref = *(JSON_obj*)json.get();
        ref["Unicode"] = JSON("\\u02a4");
        ref["Array of Zero"] = JSON(JSON_list(5, JSON(0)));
        json.write("out.json");
    }
    else
        std::cout << "failed to load" << std::endl;
    return 0;
}
