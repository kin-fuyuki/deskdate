#include <tiny/term.h>

tiny::ErrorLevel tiny::level{6};
#include <cstdio>
#include <filesystem>
#include <vector>
#include <fstream>
#include <map>
#include <thread>
#include <chrono>
#include <sstream>
#include <tiny/term.h>
std::string root;
std::string output;
enum TYPE:unsigned char{
    DIR=0,
    FILEE=1
};
struct DATA{
    TYPE type;
    void* datapointer;
};
std::map<std::string,DATA>* tree=nullptr;
std::vector<std::string> oldpaths;
extern void (*ERROR[])(char*);
bool scanchanges();
void edit();
void writemenu(std::ofstream&f,std::string name,std::string fullpath,std::map<std::string,DATA>*n,int depth);
std::vector<std::string> loadsort(std::string path);
int main(int argc,char**argv) {
    if (argc<3){ERROR[0]("");}
    root=argv[1];
    output=argv[2];
    while (true) {
        if (std::filesystem::exists(std::filesystem::path(root) / "R"))
       if (scanchanges()){edit();}
       std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool scanchanges() {
    std::filesystem::remove(std::filesystem::path(root) / "R");
    std::vector<std::string> newpaths;
    for(auto&p:std::filesystem::recursive_directory_iterator(root)){
        if(p.is_regular_file()&&p.path().extension()==".desktop"){
            newpaths.push_back(p.path().string());
        }
    }
    if(newpaths==oldpaths){return false;}
    oldpaths=newpaths;
    std::map<std::string,DATA>*newtree=new std::map<std::string,DATA>();
    for(auto&path:newpaths){
        std::filesystem::path p(path);
        std::filesystem::path rel=std::filesystem::relative(p.parent_path(),root);
        std::map<std::string,DATA>*curr=newtree;
        for(auto&part:rel){
            std::string pstr=part.string();
            if(pstr==".")continue;
            if(!(*curr)[pstr].datapointer){(*curr)[pstr]={DIR,new std::map<std::string,DATA>()};}
            curr=(std::map<std::string,DATA>*)(*curr)[pstr].datapointer;
        }
        std::string fname=p.filename().string();
        (*curr)[fname]={FILEE,(void*)new std::string(fname)};
    }
    if(tree){delete tree;}
    tree=newtree;
    return true;
}
std::vector<std::string> loadsort(std::string path){
    std::vector<std::string> order;
    std::ifstream f(path+"/sort.txt");
    if(!f.is_open())return order;
    std::string line;
    while(std::getline(f,line)){
        if(!line.empty()&&line[0]!='#'){order.push_back(line);}
    }
    f.close();
    return order;
}



















void writemenu(std::ofstream& f, std::string name, std::string fullpath, std::map<std::string, DATA>* n, int depth) {
    std::string indent(depth * 2, ' ');
    f << indent << "<Menu>\n";
    f << indent << "  <Name>" << name << "</Name>\n";
    f << indent << "  <DirectoryDir>" << fullpath << "</DirectoryDir>\n";
    f << indent << "  <AppDir>" << fullpath << "</AppDir>\n";
    f << indent << "  <Include>\n";
    for (auto& p : *n) {
        if (p.second.type == FILEE)
            f << indent << "    <Filename>" << *(std::string*)p.second.datapointer << "</Filename>\n";
    }
    f << indent << "  </Include>\n";
    for (auto& p : *n) {
        if (p.second.type == DIR) {
            writemenu(f, p.first, fullpath + "/" + p.first, (std::map<std::string, DATA>*)p.second.datapointer, depth + 1);
        }
    }
    f << indent << "  <Layout>\n";
    std::vector<std::string> order = loadsort(fullpath);
    std::map<std::string, bool> handled;
    for (auto& item : order) {
        for (auto& p : *n) {
            if (p.second.type == DIR && !handled[p.first] && p.first == item) {
                f << indent << "    <Menuname>" << p.first << "</Menuname>\n";
                handled[p.first] = true;
            }
        }
    }
    for (auto& p : *n) {
        if (p.second.type == DIR && !handled[p.first]) {
            f << indent << "    <Menuname>" << p.first << "</Menuname>\n";
            handled[p.first] = true;
        }
    }
    for (auto& item : order) {
        std::string dtk = (item.find(".desktop") == std::string::npos) ? item + ".desktop" : item;
        for (auto& p : *n) {
            if (p.second.type == FILEE && !handled[p.first] && (p.first == item || p.first == dtk)) {
                f << indent << "    <Filename>" << *(std::string*)p.second.datapointer << "</Filename>\n";
                handled[p.first] = true;
            }
        }
    }
    for (auto& p : *n) {
        if (p.second.type == FILEE && !handled[p.first]) {
            f << indent << "    <Filename>" << *(std::string*)p.second.datapointer << "</Filename>\n";
        }
    }
    f << indent << "  </Layout>\n";
    f << indent << "</Menu>\n";
}














void edit() {
    std::ofstream f(output);
    f<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    f<<"<!DOCTYPE Menu PUBLIC \"-//freedesktop//DTD Menu 1.0//EN\" \"http://www.freedesktop.org/standards/menu-spec/menu-1.0.dtd\">\n";
    writemenu(f,std::filesystem::path(root).filename().string(),root,tree,0);
    f.close();
}

void (*ERROR[])(char*)={
    [](char*s){tiny::fatal("NO PATH ARGUMENT");exit(1);},
    [](char*s){tiny::fatal("PATH NOT FOUND");exit(1);},
    [](char*s){tiny::warning("EMPTY FOLDER");},
};
