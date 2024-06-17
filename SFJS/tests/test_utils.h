//
//  test_utils.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 19/12/23.
//

#ifndef test_utils_h
#define test_utils_h
#include <iostream>
#include <map>

const char* get_arg_val(const char* key, int argc, const char* argv[])
{
    for (int i = 0; i < argc - 1; i++)
    {
        //std::cout << argv[i] << '\n';

        if (std::string(argv[i]) == std::string(key))
            return argv[i + 1];
    }
    return 0;
}

std::map<std::string,std::string> get_parameters_map(int argc, const char* argv[])
{
    std::map<std::string,std::string> parameter_map;
    for (int i = 0; i < argc - 1; i++)
    {
        std::string param(argv[i]);
        if ( param.length()> 0 && param[0] == '-')
            parameter_map[param] = std::string(argv[i+1]);
    }
    return parameter_map;
}
#endif /* test_utils_h */
