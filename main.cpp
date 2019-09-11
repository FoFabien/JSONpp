#include <iostream>
#include "json.hpp"

int main()
{
    JSON json;
    if(json.parse("test.json"))
    {
        std::cout << json << std::endl;
        JSON_obj& ref = *(JSON_obj*)json.get();
        #ifdef JSON_STR_UNICODE
        ref[L"Unicode"] = JSON(L"\\u02a4");
        ref[L"Array of Zero"] = JSON(JSON_list(5, JSON(0)));
        #else
        ref["Unicode"] = JSON("\\u02a4");
        ref["Array of Zero"] = JSON(JSON_list(5, JSON(0)));
        #endif
        json.write("out.json");
    }
    else
        std::cout << "failed to load" << std::endl;
    return 0;
}
