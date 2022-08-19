#ifndef TGAPI_DATATYPES_H
#define TGAPI_DATATYPES_H
enum class COMMAND{
    NONE,GET_PIC,GET_S_PIC
};
struct stTgApiStruct{
    unsigned long UserID;
    COMMAND command;

};
#endif //TGAPI_DATATYPES_H
