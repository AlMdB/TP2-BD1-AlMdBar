#ifndef REGISTER_TYPE_H
#define REGISTER_TYPE_H


class RegisterType{
    enum class type_defined{
        INT,
        ALFA,
        DATETIME,
        VARALFA
    };

    
};


class AlfaInfo{
    int size_alfa;
    public:
        AlfaInfo(int _size_alfa) : size_alfa(_size_alfa){};
}



#endif