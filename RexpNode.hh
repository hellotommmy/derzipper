
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>

#include <memory>
using std::unique_ptr;

using std::endl;
using std::cout;
using std::vector;
using std::string;

#define MAXBITLISTLEN 64
#define BITSTORE (MAXBITLISTLEN / ((sizeof(uint)) * 8))
#define CLRB(node)  ({node->bits[0] = 0; for(int i = 0; i < BITSTORE; i ++){node->bits[i+1] = 0;}})

enum tpe {ZERO, ONE, CHAR, SEQ, ALT, STAR} ;
enum tkens {PAREN, OP, NUM} ;

typedef enum tpe type;
typedef enum tkens tokens;
class CNode;
class CList;
class RNode;
RNode * deepcopy(RNode * r);
CList * deepcopy_children(CList * c);
RNode * parseRexpInput(char * t, int * p);
void expect(char c, char * t, int * p);
vector<string> rexp_types = {"ZERO", "ONE", "CHAR", "SEQ", "ALT", "STAR"};

 
class RNode {
    public:
        type type;
        uint * bits;
        char data;
        CList * children;

        RNode(char dataIn, uint * bitsIn) : type(CHAR), bits(bitsIn), data(dataIn), children(nullptr) {}

        RNode(enum tpe typeIn, CList * childrenIn) : type(typeIn), children(childrenIn), data('#'), bits(nullptr) {
            // bits = new uint[BITSTORE + 1];
            // bits[0] = 0; 
            // for(int i = 0; i < BITSTORE; i ++) {
            //     bits[i+1] = 0;
            // }
            if(typeIn != ZERO){
                bits = new uint[BITSTORE + 1];
                for(int i = 0; i < BITSTORE + 1; ++i) {
                    bits[i] = 0;
                }
            }
        }

        RNode(RNode * rnodeIn) : type(rnodeIn->type), bits(nullptr), data(rnodeIn->data), children(nullptr) {
            // cout << "copy constructor called:";
            // cout << endl << rexp_types[rnodeIn->type];
            type = rnodeIn->type;
            
            if(rnodeIn->type != ZERO){
                bits = new uint[BITSTORE + 1];
                for(int i = 0; i < BITSTORE + 1; ++i) {
                    bits[i] = rnodeIn->bits[i];
                }
            }
            //cout << bits << " is the address of bits(possible leak place)\n";

            data = rnodeIn->data;
            children = deepcopy_children(rnodeIn->children);
        }

        // RNode(enum tpe typeIn, RNode * sub) : type(typeIn) {
        //     children = new  CList(sub);
        //     bits = new uint[BITSTORE + 1];
        //     data = '#';

        // }

        RNode() : type(ZERO), bits(nullptr), data('#'), children(nullptr) {};

        ~RNode();


void expect(char c, char * t, int * p);
RNode(RNode * e1, RNode * e2, enum tpe t);
RNode(char * t, int * p, RNode * e, tpe tp);
//star constructor
RNode(char * t, int * p, RNode * e);

RNode(char * t, int * p, char c);


RNode * compParser(char * t, int * p, RNode * e, tpe tp);
RNode * getChild(int ith = 1);
RNode * parseRexpInput(char * t, int * p);
};