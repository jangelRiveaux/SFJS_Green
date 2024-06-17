//
//  sfjs_instance_pool.h
//  Models
//
//  Created by Jose Angel Riveaux on 06/07/23.
//

#ifndef sfjs_instance_pool_h
#define sfjs_instance_pool_h

#include <vector>
#include <string>

class SFJSInstancePool{
public:
    
    static std::vector<std::string> getSmall(){
        
        std::vector<std::string> result =  {"YFJS_4_4_7_5_0", "YFJS_4_4_7_5_1", "YFJS_4_4_7_5_2", "YFJS_4_4_7_5_3", "YFJS_4_4_7_5_4", "YFJS_4_4_7_5_5", "YFJS_4_4_7_5_6", "YFJS_4_4_7_5_7", "YFJS_4_4_7_5_8", "YFJS_4_4_7_5_9", "YFJS_5_4_7_5_0", "YFJS_5_4_7_5_1", "YFJS_5_4_7_5_2", "YFJS_5_4_7_5_3", "YFJS_5_4_7_5_4", "YFJS_5_4_7_5_5", "YFJS_5_4_7_5_6", "YFJS_5_4_7_5_7", "YFJS_5_4_7_5_8", "YFJS_5_4_7_5_9", "YFJS_6_4_7_5_0", "YFJS_6_4_7_5_1", "YFJS_6_4_7_5_2", "YFJS_6_4_7_5_3", "YFJS_6_4_7_5_4", "YFJS_6_4_7_5_5", "YFJS_6_4_7_5_6", "YFJS_6_4_7_5_7", "YFJS_6_4_7_5_8", "YFJS_6_4_7_5_9"  ,"DA_2_5_1", "DA_2_5_2", "DA_2_5_3", "DA_2_5_4", "DA_2_5_5", "DA_2_5_6", "DA_2_5_7", "DA_2_5_8", "DA_2_5_9", "DA_2_5_10", "DA_2_5_11", "DA_2_5_12", "DA_2_5_13", "DA_2_5_14", "DA_2_5_15", "DA_2_5_16", "DA_2_5_17", "DA_2_5_18", "DA_2_5_19", "DA_2_5_20","DAFJS_3_5_1", "DAFJS_3_5_2", "DAFJS_3_5_3", "DAFJS_3_5_4", "DAFJS_3_5_5", "DAFJS_3_5_6", "DAFJS_3_5_7", "DAFJS_3_5_8", "DAFJS_3_5_9", "DAFJS_3_5_10"};
        
  
        return result;
    };
    
    static std::vector<std::string> getDAFJSInstances(){

        std::vector<std::string> result = {"DAFJS01", "DAFJS02", "DAFJS03", "DAFJS04", "DAFJS05", "DAFJS06", "DAFJS07", "DAFJS08", "DAFJS09", "DAFJS10", "DAFJS11", "DAFJS12", "DAFJS13", "DAFJS14", "DAFJS15", "DAFJS16", "DAFJS17", "DAFJS18", "DAFJS19", "DAFJS20", "DAFJS21", "DAFJS22", "DAFJS23", "DAFJS24", "DAFJS25", "DAFJS26", "DAFJS27", "DAFJS28", "DAFJS29", "DAFJS30"};
        return result;
    };
    
    
    static std::vector<std::string> getYFJSInstances(){

        std::vector<std::string> result = {"YFJS01", "YFJS02", "YFJS03", "YFJS04", "YFJS05", "YFJS06", "YFJS07", "YFJS08", "YFJS09", "YFJS10", "YFJS11", "YFJS12", "YFJS13", "YFJS14", "YFJS15", "YFJS16", "YFJS17", "YFJS18", "YFJS19", "YFJS20"};  
        return result;
    };

    static std::vector<std::string> getSubSetInstances() {

        std::vector<std::string> result = { "YFJS01", "YFJS04", "YFJS11", "YFJS13", "YFJS15", "DAFJS06", "DAFJS07", "DAFJS20", "DAFJS21", "DAFJS25", "DAFJS30" , "YFJS18" };
        return result;
    };

};
#endif /* sfjs_instance_pool_h */
