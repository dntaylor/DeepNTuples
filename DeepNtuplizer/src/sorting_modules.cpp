


#include "../interface/sorting_modules.h"
#include <iostream>
namespace sorting{

std::vector<size_t> invertSortingVector(const std::vector<sortingClass<size_t> > & in){
    size_t max=0;
    for(const auto& s:in){
        if(s.get()>max && s.get()<1000) max=s.get();
        if(s.get()>1000) std::cout << s.sortValA <<" " << s.sortValB << " " << s.sortValC << " " << s.get() << std::endl;
    }

    std::vector<size_t> out(max+1,0);
    for(size_t i=0;i<in.size();i++){
        if(in.at(i).get()>1000) continue;
        out.at(in.at(i).get())=i;
    }
    return out;
}

}
