/**
\file   crfract.cpp
\brief  описание функций для поиска файлов дубликатов
\author Курников Павел
\date   2011.12.10
*/

#include <fstream>
#include <sstream>
#include <hash_map>
#include <iostream>
#include <fstream>

#include "files.h";

using namespace std;

file::file()
{
}

file::~file()
{
}

// формирование списка файлов
void file::spisok(std::string path)
{
    string name;
    int size(0);
    struct _finddata_t c_file;
    intptr_t hFile;
    string path1;
    path1 = path + "*";
    int k(0);
    if( (hFile = _findfirst(path1.c_str(), &c_file )) == -1L )
    {
        cout << "No *.* files in current directory! " << endl;
    }
    else
    {
        do
        {
          if (c_file.attrib == _A_SUBDIR)
	        {
		        if (k < 2)  //  задержка на просмотр первых двух папок
		        {
			        ++k;
		        }
		        else
		        {
			        spisok(path+c_file.name+'\\');  //  рекурсивный вызов для поиска файлов в подкаталогах
		        }
		        continue;
	        }
            name = path + c_file.name;
            size = c_file.size;
//            SNode names(name, size);
            SNode names;
            names.name = name;
            names.raz = size;
            names.state = -1;
            sp.push_back(names);
            cout << c_file.name << endl;
            ifstream in;
            in.open(c_file.name, ios::binary);
            cout << endl;
        }
        while( _findnext( hFile, &c_file ) == 0 );
    }
}

// сравнение файлов с одинаковыми размерами и запись в контейнер
void file::dubl_file()
{
    string name1;
    string name2;
    int nm(0);
    int raz(0); 
    int last(0);
    int j(0);
    raz = sp.size();
    for (int i(0); i < sp.size(); i++)
    {     
        SNodeDubl dubl2;
        for (j=(i + 1); j < sp.size(); j++)
        {        
            if ((sp[i].raz == sp[j].raz) && (sp[j].state == -1))
            {
                if  (comparison(sp[i].name, sp[j].name) == true)
                {
                    if (sp[i].state == -1)
                    {
                        sp[i].state = 1;
                        dubl2.spdubl.push_back(i); // запись файлов эталонов
                    }
                    nm = j;
                    sp[j].state = 1;            
                    dubl2.spdubl.push_back(nm);
                    last++;
                    int ger;
                    ger = sp.size();
                }
            }           
        }
        if (dubl2.spdubl.size() > 0) // запись в случае когда когда были найдены файлы дубликаты
        {
            group.push_back(dubl2); // хранятся группы указателей на файлы дубликаты
        }
    } 
}

void file::print_files_of_dublicate()
{
    vector <SNodeDubl>::iterator cur;
    if (group.size() > 0)
    {
        for (int i(0); i < group.size(); i++)
        {
            cout << "Group number " << i + 1 << endl;
            cout << "-------------" << endl;
            for (int j(0); j < group[i].spdubl.size(); j++)
            {
                cout << sp[group[i].spdubl[j]].name << " - " << sp[group[i].spdubl[j]].raz << "_byte" << endl;
            }
            cout << "-------------" << endl;
            
        }
    }
    else
    {
        cout << "Group files of dublicate not found !";
    }
}



// сравнение файлов с одинаковыми размерами
bool file::comparison(std::string n1, std::string n2)
{
    bool res(false);
    const int MAX = 1024;
    char *buff1;
    char *buff2;
    int len(0);  
    int result;
    int file_size1(0);
    int file_size2(0);
    int i(1);    

    struct stat statbuf1;
    struct stat statbuf2;
    len = MAX;
    stat(n1.c_str(), &statbuf1);
    stat(n2.c_str(), &statbuf2);
    file_size1 = statbuf1.st_size; // размер первого файла
    file_size2 = statbuf2.st_size; // размер второго файла
    
    ifstream in1;
    ifstream in2;
    in1.open(n1.c_str(), ios::binary); 
    in2.open(n2.c_str(), ios::binary);
    buff1 = new char[MAX];
    buff2 = new char[MAX];

    for (i; (i * MAX) < file_size1 + MAX; i++)
    {
        in1.read(buff1, len);
        in1.seekg(i * MAX, ios::beg);       //  позиция начиная с i*MAX байт
        in2.read(buff2, len);
        in2.seekg(i * MAX, ios::beg);       //  позиция начиная с i*MAX байт
        result = memcmp(buff1, buff2, MAX - 1);        //  сравнение двух массивов
        
        if (result != 0)
        {
            res = false;  // файлы не равны, выход из цикла
            break;
        }
        else
        {
            res = true;  //  блоки файлов равны, продолжить дальше
        }
    }
    delete[] buff1; // очистка памяти
    delete[] buff2;
    return res;
}
